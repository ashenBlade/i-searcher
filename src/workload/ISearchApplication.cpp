//
// Created by ashblade on 21.03.24.
//

#include "workload/ISearchApplication.h"
#include "common/Document.h"
#include "tokenizer/WordCleanerTokenizerDecorator.h"
#include "tokenizer/FilterTokenizerDecorator.h"
#include "common/InverseIndex.h"
#include "ranger/BM25Ranger.h"
#include "search_engine/LocalSearchEngine.h"
#include "storage/FileSystemIndexRepository.h"
#include "storage/DocumentParser.h"

#include <utility>
#include <vector>
#include <thread>

isearch::ISearchApplication::ISearchApplication(std::string workingDirectory):
_workingDirectory(std::move(workingDirectory)) { }


static std::vector<isearch::Document> parseDocumentsFromWorkingDirectory(const std::string& workingDirectory) {
    isearch::DocumentParser parser {workingDirectory};
    return parser.parseAllDocumentsInDirectory();
}

struct document_saver_state {
    std::vector<isearch::Document>& _documents;
    long _start;
    long _end;
    isearch::FileSystemIndexRepository& _repository;

    document_saver_state(std::vector<isearch::Document>& documents, long start, long end, isearch::FileSystemIndexRepository& repository)
    : _documents(documents), _start(start), _end(end), _repository(repository) { }
    
    void saveDocuments() {
        for (long i = _start; i < _end; ++i) {
            _repository.saveDocument(_documents[i]);
        }
    }
};

void isearch::ISearchApplication::initialize(const isearch::InitOptions &options) {
    options.validate();

    // Сразу создаем директорию для данных приложения
    auto repository = isearch::FileSystemIndexRepository(_workingDirectory);
    repository.createAppDataDirectories();

    // Если приложение однопоточное, то делаем все дела тут-же (в этом же потоке)
    std::vector<isearch::Document> documents;
    if (options.parallelism == 1) {
        // Парсим все документы из указанной директории
        documents = parseDocumentsFromWorkingDirectory(_workingDirectory);

        // Сохраняем индексные файлы документов
        repository.saveDocuments(documents);
    } else {
        // Иначе создаем parallel - 1 фоновых потоков
        isearch::DocumentParser parser {_workingDirectory};
        int parallelism = options.getParallelism();
        documents = parser.parseAllDocumentsInDirectoryParallel(parallelism);
        long documentsPerThread = std::max(1L, static_cast<long>(documents.size() / parallelism));

        // Запускаем их
        std::vector<std::thread> workers {};
        std::vector<std::shared_ptr<document_saver_state>> states {};
        for (int i = 0; i < parallelism - 1; ++i) {
            long start = i * documentsPerThread;
            long end = start + documentsPerThread;
            auto state = std::make_shared<document_saver_state>(documents, start, end, repository);
            workers.push_back(std::move(std::thread([](document_saver_state *state) {state->saveDocuments();}, state.get())));
            states.push_back(state);
        }

        // Работаем сами
        document_saver_state currentState {documents, (parallelism - 1) * documentsPerThread, static_cast<long>(documents.size()), repository};
        currentState.saveDocuments();

        // Ожидаем завершение работы фоновых потоков
        for (auto &worker: workers) {
            worker.join();
        }
    }

    // Строим и сохраняем обратный индекс
    auto inverseIndex = isearch::InverseIndex::build(documents);
    repository.saveInverseIndex(inverseIndex);
}

static isearch::BM25Ranger createRanger(const isearch::QueryOptions& options) {
    return {options.bm25.k, options.bm25.b, options.getParallelism()};
}

std::vector<std::string> isearch::ISearchApplication::query(const std::string &queryString, const isearch::QueryOptions& options) {
    options.validate();
    isearch::FileSystemIndexRepository repository {_workingDirectory};
    isearch::BM25Ranger ranger = createRanger(options);
    isearch::LocalSearchEngine searchEngine {repository, ranger, options.getParallelism()};
    return searchEngine.search(queryString, options.max);
}
