//
// Created by ashblade on 06.03.24.
//

#include <system_error>
#include <cmath>
#include <algorithm>
#include <thread>
#include <memory>

#include "ranger/BM25Ranger.h"

isearch::BM25Ranger::BM25Ranger(double k, double b, int parallelism): _k(k), _b(b), _parallelism(parallelism) {
    if (b < 0 || 1 < b) {
        throw std::range_error("Коэффициент b может принимать значения от 0 до 1 включительно");
    }
    if (parallelism < 0) {
        throw std::runtime_error("Параллелизм должен быть положительным");
    }
}

static double calculateSmoothedIdf(const std::string& word, isearch::DocumentCollection &documents) {
    // Для подсчета IDF используется сглаженная формула на случай, если подобных сло
    const auto documentsCount = (double) documents.size();
    const auto documentsContainsWord = (double) documents.count(word);
    double calculatedIdf = std::log(
            (documentsCount - documentsContainsWord + 0.5)
            /
            (documentsContainsWord + 0.5)
    );

    // Чтобы отрицательные значения не мутили воду, заменяем их на эпсилон
    constexpr double eps = 0.000001;
    return std::max(calculatedIdf, eps);
}

static double calculateBM25(const isearch::Document &document, const std::vector<std::string> &search_vector, isearch::DocumentCollection &collection, double k, double b) {
    double sum = 0;
    double relativeDocumentSize = (double) collection.size() / (double) collection.avg_count();

    for (const auto &word: search_vector) {
        const auto idf = calculateSmoothedIdf(word, collection);
        const auto frequency = (double) document.count(word);
        const auto score = idf * (
                                         (frequency * (k + 1))
                                         /
                                         (frequency + k * (1 - b + b * relativeDocumentSize))
                                 );
        sum += score;
    }

    return sum;
}

// Специальный класс для работы с сортированным списком данных документов и их весов
// Вместо глобальной сортировки поддерживаем уже отсортированный массив с ограничением по размеру
class SortedDocumentCollection {
public:
    // Пара из названия документа и его веса
    using DocumentInfo = std::pair<std::string, double>;

    std::vector<DocumentInfo> _documentScore;
    int _max;

    explicit SortedDocumentCollection(int max): _documentScore(), _max(max) {}
    ~SortedDocumentCollection() = default;

    void append(const std::string &documentTitle, double score) {
        // В пустой можно всегда вставить элементы
        if (_documentScore.empty()) {
            _documentScore.emplace_back(documentTitle, score);
            return;
        }

        // Так как этот массив уже отсортирован, то для вставки можно применить бинарный поиск
        auto pair = std::make_pair(documentTitle, score);
        _documentScore.insert(
                std::upper_bound(_documentScore.begin(), _documentScore.end(), pair, [](const DocumentInfo& left, const DocumentInfo& right) {return std::get<1>(left) > std::get<1>(right);}),
                pair
        );

        // Если размер превысил максимальный, то удалим последний элемент
        if (_max < _documentScore.size()) {
            _documentScore.erase(_documentScore.end() - 1);
        }
    }

    std::vector<std::string> build() {
        std::vector<std::string> result {};
        result.reserve(_documentScore.size());
        for (const auto &item: _documentScore) {
            result.push_back(std::get<0>(item));
        }
        return result;
    }

    void merge(const SortedDocumentCollection &other) {
        if (&other == this) {
            return;
        }

        for (const auto &pair: other._documentScore) {
            auto filename = std::get<0>(pair);
            auto score = std::get<1>(pair);
            append(filename, score);
        }
    }
};

struct thread_worker_state {
    isearch::DocumentCollection& _collection;
    const std::vector<std::string>& _query;
    double _k;
    double _b;
    long _start;
    long _end;
    SortedDocumentCollection _sorted;

    thread_worker_state(isearch::DocumentCollection& collection, const std::vector<std::string>& query, double k, double b, long start, long end, int max):
        _collection(collection), _query(query), _k(k), _b(b), _start(start), _end(end), _sorted(max) {  }

    void run() {
        const auto& documents = _collection.getDocuments();
        for (long i = _start; i < _end; ++i) {
            auto& document = documents[i];
            const auto score = calculateBM25(document, _query, _collection, _k, _b);
            _sorted.append(document.title(), score);
        }
    }
};

std::vector<std::string>
isearch::BM25Ranger::range(const std::vector<std::string> &search_vector,
                           isearch::DocumentCollection &collection,
                           int max) {
    // Однопоточное выполнение
    if (_parallelism == 1) {
        SortedDocumentCollection sorted {max};

        // Обходим каждый документ и вычисляем его вес по формуле BM25
        // Затем, вставляем его в уже отсортированный список документов
        for (auto &item: collection) {
            const auto score = calculateBM25(item, search_vector, collection, _k, _b);
            sorted.append(item.title(), score);
        }

        // Создаем результирующий список документов
        return sorted.build();
    }

    // Создаем и запускаем фоновые потоки
    auto& documents = collection.getDocuments();
    auto documentsPerThread = documents.size() / _parallelism;

    std::vector<std::thread> workers {};
    std::vector<std::shared_ptr<thread_worker_state>> states {};

    for (int i = 0; i < _parallelism - 1; ++i) {
        auto start = static_cast<long>(i * documentsPerThread);
        auto end = static_cast<long>(start + documentsPerThread);
        auto state = std::make_shared<thread_worker_state>(collection, search_vector, _k, _b, start, end, max);
        std::thread worker {[](thread_worker_state* state) {state->run();}, state.get()};
        workers.push_back(std::move(worker));
        states.push_back(std::move(state));
    }

    // Начинаем расчеты в текущем потоке
    thread_worker_state currentState {collection, search_vector, _k, _b, static_cast<long>((_parallelism - 1) * documentsPerThread), static_cast<long>(documents.size()), max};
    currentState.run();

    // Заканчиваем расчеты в других потоках и объединяем результаты
    for (int i = 0; i < _parallelism - 1; ++i) {
        auto& worker = workers[i];
        worker.join();
        auto state = states[i];
        currentState._sorted.merge(state->_sorted);
    }

    return currentState._sorted.build();
}
