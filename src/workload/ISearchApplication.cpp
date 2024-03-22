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

static void createDirIfNotExists(const std::string& path) {
    // В начале проверим, что ничего по этому пути нет
    struct stat st = {0};
    if (stat(path.data(), &st)) {
        // Проверяем, что по указанному пути - директория
        if (S_ISDIR(st.st_mode)) {
            return;
        }

        throw std::runtime_error("По указанному пути лежит файл, а не директория");
    }

    // Дальше, создаем директорию
    if (mkdir(path.data(), 0700)) {
        perror("createDirIfNotExists");
        throw std::runtime_error("Ошибка при создании директории: " + path);
    }
}

/// @brief Проверить, что указанный файл подходит для индексирования.
/// На данный момент используем только '.txt' файлы
static bool isRelevantFile(const std::string &filename) {
    constexpr const char* ending = ".txt";
    return 4 <= filename.size() && std::equal(filename.begin(), filename.end(), ending);
}

static std::set<std::string> RussianExcludedWords {
        "и",
        "или",
        "но",
};

static isearch::Document parseDocument(long id, const std::string& filename, std::istream& stream) {
    isearch::StreamTokenizer streamTokenizer {stream};;
    isearch::WordCleanerTokenizerDecorator cleanerDecorator{streamTokenizer};
    isearch::FilterTokenizerDecorator filterDecorator {RussianExcludedWords, cleanerDecorator};
    isearch::RussianStemmer stemmer {};

    std::map<std::string, long> frequencies {};
    std::string word;
    while (filterDecorator.tryReadNextWord(word)) {
        auto stemmed = stemmer.stem(word);
        ++frequencies[stemmed];
    }

    return isearch::Document(id, filename, std::move(frequencies));
}

static std::vector<isearch::Document> parseDocuments(const std::string& applicationDirectory) {
    DIR* dir = opendir(applicationDirectory.data());
    if (dir == nullptr) {
        throw std::runtime_error("Не удалось открыть директорию: " + applicationDirectory);
    }

    // Поочередно считываем каждый релевантный файл из директории и пытаемся спарсить его

    // Id, которые будут назначаться документам
    long id = 0L;
    std::vector<isearch::Document> documents {};
    dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        if (!isRelevantFile(filename)) {
            continue;
        }

        auto documentPath = applicationDirectory + '/' + filename;
        std::fstream file {documentPath};
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

static void saveDocument(const isearch::Document& document, const std::string& indexDirectory) {
    auto indexFilePath = indexDirectory + '/' + std::to_string(document.id());
    std::ofstream file {indexFilePath};
    if (!file) {
        std::cerr << "Не удалось сохранить индексный файл " << indexFilePath << " для документа " << document.title() << std::endl;
        return;
    }

    try {
        isearch::BinaryDocumentSerializer serializer{file};
        serializer.serialize(document);
    } catch (const std::exception& ex) {
        std::cerr << "Ошибка при сохранении индексного файла " << indexFilePath << " для документа " << document.title() << ". Причина: " << ex.what() << std::endl;
    }
}

static void saveDocumentIndexFiles(const std::vector<isearch::Document>& documents, std::string&& indexDirectory) {
    createDirIfNotExists(indexDirectory);

    for (auto &document: documents) {
        saveDocument(document, indexDirectory);
    }
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

static void saveInverseIndex(const isearch::InverseIndex& inverseIndex, const std::string& inverseIndexFilePath) {
    std::ofstream file {inverseIndexFilePath};
    if (!file) {
        throw std::runtime_error("Не удалось создать файл обратного индекса");
    }

    try {
        isearch::InverseIndexSerializer serializer{file};
        serializer.serialize(inverseIndex);
    } catch (const std::exception& ex) {
        throw std::runtime_error("Ошибка при сохранении индексного файла: " + std::string(ex.what()));
    }
}

void isearch::ISearchApplication::initialize() {
    // 1. Проверяем, что указанная директория существует
    auto applicationDir = getApplicationDirPath();
    createDirIfNotExists(applicationDir);

    // 2. Парсим каждый файл в директории
    auto documents = parseDocuments(_workingDirectory);

    // 3. Сохраняем индексные файлы каждого документа
    saveDocumentIndexFiles(documents, getIndexDirectoryPath());

    // 4. Строим инвертированный индекс
    auto inverseIndex = buildInverseIndex(documents);

    // 5. Сохраняем инвертированный индекс
    saveInverseIndex(inverseIndex, getInverseIndexPath());
}

std::string isearch::ISearchApplication::getApplicationDirPath() const {
    return _workingDirectory + '/' + _applicationDirectory;
}

std::string isearch::ISearchApplication::getInverseIndexPath() const {
    return _workingDirectory + '/' + _applicationDirectory + '/' + _inverseIndexFileName;
}

std::string isearch::ISearchApplication::getIndexDirectoryPath() const {
    return _workingDirectory + '/' + _applicationDirectory + '/' + _indexDirectoryName;
}

static std::vector<std::string> parseQueryString(const std::string& query) {
    isearch::StringTokenizer tokenizer {query};
    isearch::WordCleanerTokenizerDecorator cleaner {tokenizer};
    isearch::RussianStemmer stemmer {};

    std::vector<std::string> words {};
    std::string word{};
    while (cleaner.tryReadNextWord(word)) {
        words.push_back(stemmer.stem(word));
    }

    return words;
}

static isearch::InverseIndex readInverseIndex(const std::string& inverseIndexFilePath) {
    std::ifstream file {inverseIndexFilePath};
    if (!file) {
        throw std::runtime_error("Не удалось открыть файл обратного индекса");
    }

    isearch::InverseIndexDeserializer deserializer {};
    return deserializer.deserialize(file);
}

static std::set<long> getPossibleDocumentIds(const std::vector<std::string>& query, const isearch::InverseIndex& inverseIndex) {
    
    
    std::set<long> documentIds {};

    for (const auto &token: query) {
        auto tokenDocuments = inverseIndex.get_documents(token);
        if (tokenDocuments == nullptr) {
            continue;
        }

        documentIds.insert(tokenDocuments->cbegin(), tokenDocuments->cend());
    }

    return documentIds;
}

static std::vector<isearch::Document> readAllDocuments(const std::set<long>& documentIds, const std::string& indexFilesPath) {
    std::vector<isearch::Document> documents {};
    
    for (const auto &id: documentIds) {
        std::string indexFilePath = indexFilesPath + '/' + std::to_string(id);
        std::ifstream indexFile {indexFilesPath};
        if (!indexFile) {
            std::cerr << "Ошибка открытия индексного файла с id " << std::to_string(id) << std::endl;
            continue;
        }

        try {
            isearch::BinaryDocumentDeserializer deserializer{indexFile};
            documents.push_back(deserializer.deserialize());
        } catch (const std::exception& ex) {
            std::cerr << "Ошибка во время парсинга индексного файла " << indexFilePath << ": " << ex.what() << std::endl;
        }
    }
    
    return documents;
}

constexpr double k = 2;
constexpr double b = 0.75;

static std::vector<std::string> getRelevantFileNames(const std::vector<isearch::Document>& documents, const std::vector<std::string>& query, int max) {
    isearch::BM25Ranger ranger{k, b};
    auto documentCollection = isearch::DocumentCollection(documents);
    return ranger.range(query, documentCollection, max);
}

std::vector<std::string> isearch::ISearchApplication::query(const std::string &query, int max) {
    /*
     * Шаги:
     * 1. Парсим строку запроса
     * 2. Получаем инвертированный индекс
     * 3. Читаем все необходимые документы (по индексам)
     * 4. Запускаем алгоритм ранжирования
     * 5. Возвращаем названия релевантных документов
     */

    if (max < 0) {
        throw std::runtime_error("Количество документов в ответе не может быть отрицательным");
    }

    if (max == 0) {
        return {};
    }

    auto query = parseQueryString(query);
    auto inverseIndex = readInverseIndex(getInverseIndexPath());
    auto possibleDocumentIds = getPossibleDocumentIds(query, inverseIndex);
    auto documents = readAllDocuments(possibleDocumentIds, getInverseIndexPath());

    return getRelevantFileNames(documents, query, max);
}
