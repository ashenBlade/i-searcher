//
// Created by ashblade on 06.03.24.
//

#include <sstream>

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

static std::set<long> getPossibleDocumentIds(isearch::IIndexRepository& repository, const std::vector<std::string>& query) {
    auto inverseIndex = repository.getInverseIndex();
    return getPossibleDocumentIds(query, inverseIndex);
}

static std::vector<isearch::Document> getDocumentsRelevantDocuments(const std::vector<std::string>& query, isearch::IIndexRepository &repository) {
    auto ids = getPossibleDocumentIds(repository, query);
    std::vector<isearch::Document> documents {};
    for (const auto &id: ids) {
        auto result = repository.getDocumentById(id);
        if (result != nullptr) {
            documents.emplace_back(std::move(*result));
            result.reset();
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
    auto documents = getDocumentsRelevantDocuments(query, _repository);
    return runRanger(documents, query, _ranger, max);
}

isearch::LocalSearchEngine::LocalSearchEngine(isearch::IIndexRepository &repository, isearch::IRanger& ranger):
    _repository(repository), _ranger(ranger)
{  }

isearch::LocalSearchEngine::~LocalSearchEngine() = default;


