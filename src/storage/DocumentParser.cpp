//
// Created by ashblade on 24.03.24.
//

#include <dirent.h>
#include <thread>
#include <cstring>
#include <memory>
#include <fstream>

#include "storage/DocumentParser.h"
#include "tokenizer/StreamTokenizer.h"
#include "tokenizer/WordCleanerTokenizerDecorator.h"
#include "tokenizer/FilterTokenizerDecorator.h"
#include "stemmer/StopWords.h"
#include "stemmer/RussianStemmer.h"

bool isearch::DocumentParser::isRelevantFile(const std::string &filename) {
    constexpr const char* ending = ".txt";
    return 4 <= filename.size() && std::equal(filename.begin() + static_cast<long>(filename.size()) - 4, filename.end(), ending);
}

static isearch::Document parseDocument(long id, const std::string& filename, std::istream& stream) {
    isearch::StreamTokenizer streamTokenizer {stream};
    isearch::WordCleanerTokenizerDecorator cleanerDecorator{streamTokenizer};
    isearch::FilterTokenizerDecorator filterDecorator {isearch::StopWords::russian(), cleanerDecorator};
    isearch::RussianStemmer stemmer {};

    std::map<std::string, long> frequencies {};
    std::string word;
    while (filterDecorator.tryReadNextWord(word)) {
        auto stemmed = stemmer.stem(word);
        ++frequencies[stemmed];
    }

    return isearch::Document(id, filename, std::move(frequencies));
}

std::vector<isearch::Document> isearch::DocumentParser::parseAllDocumentsInDirectory() const {
    DIR* dir = opendir(_directoryPath.data());
    if (dir == nullptr) {
        throw std::runtime_error("Не удалось открыть директорию для чтения файлов: " + std::string(std::strerror(errno)));
    }

    // Id, которые будут назначаться документам
    long id = 0L;
    std::vector<isearch::Document> documents {};
    dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        if (!isRelevantFile(filename)) {
            continue;
        }

        auto documentPath = _directoryPath + '/' + filename;
        std::ifstream file {documentPath};
        if (!file) {
            std::cerr << "Не удалось открыть файл: " << documentPath << std::endl;
            continue;
        }

        auto document = parseDocument(id, filename, file);
        documents.push_back(std::move(document));
        ++id;
    }

    closedir(dir);
    return documents;
}

isearch::DocumentParser::DocumentParser(std::string directoryPath): _directoryPath(std::move(directoryPath))
{ }

struct thread_read_data {
    std::vector<std::string>& _filenames;
    long _start;
    long _end;
    const std::string& _directory;
    std::vector<isearch::Document> _parsedDocuments;

    explicit thread_read_data(std::vector<std::string>& filenames, long start, long end, const std::string& directory): _filenames(filenames), _start(start), _end(end), _directory(directory), _parsedDocuments()
    { }

    void parseDocuments() {
        for (long i = _start; i < _end; ++i) {
            auto documentPath = _directory + '/' + _filenames[i];
            std::ifstream file {documentPath};
            if (!file) {
                std::string errorMessage {"Не удалось открыть файл: "};
                errorMessage.append(documentPath);
                errorMessage.append("\n");
                std::cerr << errorMessage;
                continue;
            }

            auto document = parseDocument(i, documentPath, file);
            _parsedDocuments.push_back(std::move(document));
        }
    }

    void moveResults(std::vector<isearch::Document>& destination) {
        for (auto &document: _parsedDocuments) {
            destination.push_back(std::move(document));
        }
        _parsedDocuments.clear();
    }
};

static void threadWorker(thread_read_data *data) {
    data->parseDocuments();
}

std::vector<isearch::Document> isearch::DocumentParser::parseAllDocumentsInDirectoryParallel(int parallelCount) const {
    // Вначале читаем все названия файлов
    auto filenames = getAllDirectoryFileNames();

    long filesPerThread = std::max(1L, static_cast<long>(filenames.size() / parallelCount));

    // Инициализируем рабочие потоки
    std::vector<std::thread> workers {};
    std::vector<std::shared_ptr<thread_read_data>> data {};
    for (int i = 0; i < parallelCount - 1; ++i) {
        long startIndex = i * filesPerThread;
        long endIndex = startIndex + filesPerThread;
        auto threadData = std::make_shared<thread_read_data>(filenames, startIndex, endIndex, _directoryPath);
        auto thread = std::thread(threadWorker, threadData.get());
        data.push_back(threadData);
        workers.push_back(std::move(thread));
    }

    // Работаем сами
    thread_read_data myData {filenames, (parallelCount - 1) * filesPerThread, static_cast<long>(filenames.size()), _directoryPath};
    myData.parseDocuments();

    std::vector<isearch::Document> documents {};
    myData.moveResults(documents);

    // Собираем результаты с других потоков
    for (int i = 0; i < parallelCount - 1; ++i) {
        auto& worker = workers[i];
        worker.join();
        data[i]->moveResults(documents);
    }

    return documents;
}

std::vector<std::string> isearch::DocumentParser::getAllDirectoryFileNames() const {
    DIR* dir = opendir(_directoryPath.data());
    if (dir == nullptr) {
        throw std::runtime_error("Не удалось открыть директорию для чтения файлов: " + std::string(std::strerror(errno)));
    }

    // Id, которые будут назначаться документам
    std::vector<std::string> filenames {};
    dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        if (!isRelevantFile(filename)) {
            continue;
        }

        filenames.push_back(filename);
    }

    closedir(dir);
    return filenames;
}
