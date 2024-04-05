//
// Created by ashblade on 06.03.24.
//

#include <sstream>
#include <queue>
#include <thread>

#include "search_engine/LocalSearchEngine.h"
#include "tokenizer/StringTokenizer.h"
#include "tokenizer/FilterTokenizerDecorator.h"
#include "tokenizer/WordCleanerTokenizerDecorator.h"
#include "stemmer/RussianStemmer.h"
#include "stemmer/StopWords.h"


static std::vector<std::string> parseQueryString(const std::string& queryString) {
    // Токенизатор:
    // 1. Парсим саму строку
    // 2. Удаляем знаки пунктуации (скорее всего вокруг самого слова)
    // 3. Удаляем стоп-слова, например, союзы
    isearch::StringTokenizer tokenizer {queryString};
    isearch::WordCleanerTokenizerDecorator cleaner {tokenizer};
    isearch::FilterTokenizerDecorator filter {isearch::StopWords::russian(), cleaner};

    // Стеммер:
    // 1. Приводим к нормализованному представлению
    isearch::RussianStemmer stemmer {};

    std::vector<std::string> query {};
    std::string currentWord {};
    while (cleaner.tryReadNextWord(currentWord)) {
        query.push_back(stemmer.stem(currentWord));
    }

    return query;
}

static std::set<long> getPossibleDocumentIds(const std::vector<std::string>& query, const isearch::InverseIndex& inverseIndex) {
    std::set<long> documentIds {};

    for (const auto &token: query) {
        auto tokenDocuments = inverseIndex.getDocuments(token);
        if (tokenDocuments == nullptr) {
            continue;
        }

        documentIds.insert(tokenDocuments->cbegin(), tokenDocuments->cend());
    }

    return documentIds;
}

static std::vector<long> getPossibleDocumentIds(isearch::IIndexRepository& repository, const std::vector<std::string>& query) {
    auto set = getPossibleDocumentIds(query, repository.getInverseIndex());
    return (std::vector<long>) {set.begin(), set.end()};
}

/// @brief Структура, которая передается в каждый поток для чтения документов и сохранения результатов
struct thread_read_state {
public:
    /// @brief Прочитанные нами документы
    std::vector<isearch::Document> _readDocuments;
    /// @brief Общий массив всех документов
    std::vector<long>& _documentIds;
    /// @brief Индекс начала чтения включительно
    unsigned long _start;
    /// @brief Индекс окончания чтения не включая
    unsigned long _end;
    /// @brief Репозиторий, который мы используем для чтения
    isearch::IIndexRepository& _repository;

    thread_read_state(isearch::IIndexRepository& repository, std::vector<long>& ids, unsigned long start, unsigned long end): _readDocuments(), _documentIds(ids), _start(start), _end(end), _repository(repository)
    { }

    void moveResults(std::vector<isearch::Document>& destination) {
        // Перемещаем прочитанные документы в целевой массив
        for (auto &document: _readDocuments) {
            destination.push_back(std::move(document));
        }

        // Очищаем свое состояние
        _readDocuments.clear();
    }
};

static void documentReadThreadWorker(void* state) {
    auto threadState = reinterpret_cast<thread_read_state*>(state);
    for (unsigned long i = threadState->_start; i < threadState->_end; ++i) {
        // На всякий случай проверяем диапазон
        if (threadState->_documentIds.size() <= i) {
            break;
        }

        auto id = threadState->_documentIds[i];
        auto ptr = threadState->_repository.getDocumentById(id);
        if (ptr != nullptr) {
            threadState->_readDocuments.push_back(std::move(*ptr));
            ptr.reset();
        }
    }
}

static std::vector<isearch::Document> getDocumentsRelevantDocuments(const std::vector<std::string>& query, isearch::IIndexRepository &repository, int parallel) {
    auto ids = getPossibleDocumentIds(repository, query);
    std::vector<isearch::Document> documents {};
    // Если для работы используем только 1 поток - то выполняем сразу
    if (parallel == 1) {
        thread_read_state currentThreadState {repository, ids, 0UL, ids.size()};
        documentReadThreadWorker(&currentThreadState);
        documents = std::move(currentThreadState._readDocuments);
    } else {
        // В противном случае запускаем parallel - 1 потоков и себя же
        std::vector<std::thread> workers {};
        std::vector<std::shared_ptr<thread_read_state>> threadStates {};

        // Для большей производительности равномерно распределяем все документы по потокам
        // Каждому достанется равное кол-во, но при делении может получиться 0, поэтому на всякий случай присваиваем 1
        auto documentsPerThreadCount = std::max(1UL, ids.size() / parallel);

        // Инициализируем фоновые потоки
        for (int threadId = 0; threadId < parallel - 1; ++threadId) {
            auto startIndex = threadId * documentsPerThreadCount;
            auto endIndex = startIndex + documentsPerThreadCount;
            auto workerState = std::make_shared<thread_read_state>(repository, ids, startIndex, endIndex);
            threadStates.emplace_back(workerState);
            auto worker = std::thread(documentReadThreadWorker, workerState.get());
            workers.push_back(std::move(worker));
        }

        // Запускаем себя (последний индекс указываем размером, т.к. вычислить могли неправильно)
        thread_read_state myState {repository, ids, (parallel - 1) * documentsPerThreadCount, ids.size()};
        documentReadThreadWorker(&myState);
        myState.moveResults(documents);

        for (int i = 0; i < workers.size(); ++i) {
            // Ожидаем завершение работы потока
            auto& worker = workers[i];
            worker.join();

            // И копируем результаты
            auto state = threadStates[i];
            state->moveResults(documents);
        }
    }

    return documents;
}

static std::vector<std::string> runRanger(const std::vector<isearch::Document>& documents, const std::vector<std::string>& query, isearch::IRanger& ranger, int max) {
    isearch::DocumentCollection collection {documents};
    return ranger.range(query, collection, max);
}

std::vector<std::string> isearch::LocalSearchEngine::search(const std::string &queryString, int max) {
    auto query = parseQueryString(queryString);
    auto documents = getDocumentsRelevantDocuments(query, _repository, _parallelism);
    return runRanger(documents, query, _ranger, max);
}

isearch::LocalSearchEngine::LocalSearchEngine(isearch::IIndexRepository &repository, isearch::IRanger &ranger,
                                              int parallelism): _parallelism(parallelism), _repository(repository), _ranger(ranger) {
    if (parallelism < 1) {
        throw std::runtime_error("Параллелизм не может быть отрицательным");
    }
}

isearch::LocalSearchEngine::~LocalSearchEngine() = default;


