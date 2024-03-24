//
// Created by ashblade on 21.03.24.
//

#include "workload/ISearchApplication.h"
#include "common/Document.h"
#include "tokenizer/StreamTokenizer.h"
#include "tokenizer/WordCleanerTokenizerDecorator.h"
#include "tokenizer/FilterTokenizerDecorator.h"
#include "stemmer/RussianStemmer.h"
#include "serialization/BinaryDocumentSerializer.h"
#include "common/InverseIndex.h"
#include "serialization/InverseIndexSerializer.h"
#include "tokenizer/StringTokenizer.h"
#include "serialization/InverseIndexDeserializer.h"
#include "serialization/BinaryDocumentDeserializer.h"
#include "ranger/BM25Ranger.h"
#include "search_engine/LocalSearchEngine.h"
#include "storage/FileSystemIndexRepository.h"
#include "storage/DocumentParser.h"

#include <dirent.h>
#include <sys/stat.h>
#include <system_error>
#include <utility>
#include <vector>
#include <fstream>
#include <iostream>

isearch::ISearchApplication::ISearchApplication(std::string repositoryDirectory, std::string applicationDirectory, std::string inverseIndexFileName, std::string indexDirectoryName):
_workingDirectory(std::move(repositoryDirectory)), _applicationDirectory(std::move(applicationDirectory)), _inverseIndexFileName(std::move(inverseIndexFileName)), _indexDirectoryName(std::move(indexDirectoryName))
{ }


static isearch::FileSystemIndexRepository createRepository(std::string applicationPath) {
    return isearch::FileSystemIndexRepository {std::move(applicationPath)};
}

static isearch::InverseIndex buildInverseIndex(const std::vector<isearch::Document>& documents) {
    isearch::InverseIndex::inverse_index_data payload {};

    // Проходимся по каждому токену в каждом документе и добавляем документ в список содержащих токен
    for (const auto &document: documents) {
        for (const auto &token_data: document) {
            // Предполагаю, что повторов токенов (и соответственно id документов) нет

            auto token = token_data.first;
            auto it = payload.find(token);
            if (it == payload.end()) {
                auto vector = std::make_shared<std::vector<long>>();
                vector->push_back(document.id());
                payload[token] = vector;
            } else {
                it->second->push_back(document.id());
            }
        }
    }

    return isearch::InverseIndex(std::move(payload));
}

static std::vector<isearch::Document> parseDocumentsFromWorkingDirectory(const std::string& workingDirectory) {
    isearch::DocumentParser parser {workingDirectory};
    return parser.parseDocuments();
}

void isearch::ISearchApplication::initialize() {
    // Парсим все документы из указанной директории
    auto documents = parseDocumentsFromWorkingDirectory(_workingDirectory);
    auto applicationDir = getApplicationDirPath();

    // Создаем директорию для данных приложения
    auto repository = createRepository(applicationDir);
    repository.createAppDataDirectory();

    // Сохраняем индексные файлы документов
    repository.saveDocuments(documents);

    // Строим и сохраняем обратный индекс
    auto inverseIndex = buildInverseIndex(documents);
    repository.saveInverseIndex(inverseIndex);
}

std::string isearch::ISearchApplication::getApplicationDirPath() const {
    return _workingDirectory + '/' + _applicationDirectory;
}

constexpr double k = 2;
constexpr double b = 0.75;

std::vector<std::string> isearch::ISearchApplication::query(const std::string &queryString, int max) {
    if (max < 0) {
        throw std::runtime_error("Количество документов в ответе не может быть отрицательным");
    }

    if (max == 0) {
        return {};
    }

    isearch::FileSystemIndexRepository repository {getApplicationDirPath()};
    isearch::BM25Ranger ranger {k, b};
    isearch::LocalSearchEngine searchEngine {repository, ranger};
    return searchEngine.search(queryString, max);
}
