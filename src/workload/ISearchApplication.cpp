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

isearch::ISearchApplication::ISearchApplication(std::string workingDirectory):
_workingDirectory(std::move(workingDirectory)) { }


static std::vector<isearch::Document> parseDocumentsFromWorkingDirectory(const std::string& workingDirectory) {
    isearch::DocumentParser parser {workingDirectory};
    return parser.parseDocuments();
}

void isearch::ISearchApplication::initialize() {
    // Парсим все документы из указанной директории
    auto documents = parseDocumentsFromWorkingDirectory(_workingDirectory);

    // Создаем директорию для данных приложения
    auto repository = isearch::FileSystemIndexRepository(_workingDirectory);
    repository.createAppDataDirectory();

    // Сохраняем индексные файлы документов
    repository.saveDocuments(documents);

    // Строим и сохраняем обратный индекс
    auto inverseIndex = isearch::InverseIndex::build(documents);
    repository.saveInverseIndex(inverseIndex);
}

static isearch::BM25Ranger createRanger(const isearch::QueryOptions& options) {
    return {options.bm25.k, options.bm25.b};
}

std::vector<std::string> isearch::ISearchApplication::query(const std::string &queryString, const isearch::QueryOptions& options) {
    options.validate();
    isearch::FileSystemIndexRepository repository {_workingDirectory};
    isearch::BM25Ranger ranger = createRanger(options);
    isearch::LocalSearchEngine searchEngine {repository, ranger};
    return searchEngine.search(queryString, options.max);
}
