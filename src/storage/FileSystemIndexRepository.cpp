//
// Created by ashblade on 24.03.24.
//

#include <utility>
#include <fstream>
#include <cstring>
#include <sys/stat.h>

#include "storage/FileSystemIndexRepository.h"
#include "serialization/BinaryDocumentDeserializer.h"
#include "serialization/InverseIndexDeserializer.h"
#include "serialization/BinaryDocumentSerializer.h"
#include "serialization/InverseIndexSerializer.h"

static const std::string InverseIndexFileName = "index";

static const std::string IndexFileNamesDirectoryName = "files";

isearch::FileSystemIndexRepository::FileSystemIndexRepository(std::string applicationDataDirectory):
_applicationDataDirectory(std::move(applicationDataDirectory))
{  }

std::string isearch::FileSystemIndexRepository::getIndexFilesDirectoryName() const {
    return _applicationDataDirectory + '/' + IndexFileNamesDirectoryName;
}

std::string isearch::FileSystemIndexRepository::getIndexFilePath(long id) const {
    return _applicationDataDirectory + '/' + IndexFileNamesDirectoryName + '/' + std::to_string(id);
}

std::shared_ptr<isearch::Document> isearch::FileSystemIndexRepository::getDocumentById(long id) {
    std::string documentIndexPath = getIndexFilePath(id);
    std::ifstream indexFile {documentIndexPath};
    if (!indexFile) {
        return nullptr;
    }

    try {
        isearch::BinaryDocumentDeserializer deserializer{indexFile};
        return std::make_shared<isearch::Document>(deserializer.deserialize(id));
    } catch (const std::exception& ex) {
        std::cerr << "Ошибка парсинга индексного файла " << id << ": " << ex.what() << std::endl;
    }

    return nullptr;
}

isearch::InverseIndex isearch::FileSystemIndexRepository::getInverseIndex() {
    auto inverseIndexFilePath = getInverseIndexFilePath();
    std::ifstream inverseIndexFile {inverseIndexFilePath};

    if (!inverseIndexFile) {
        throw std::runtime_error("Ошибка открытия файла обратного индекса: " + std::string(std::strerror(errno)));
    }

    try {
        isearch::InverseIndexDeserializer deserializer {};
        return deserializer.deserialize(inverseIndexFile);
    } catch (const std::exception& ex) {
        throw std::runtime_error("Ошибка парсинга файла обратного индекса: " + std::string(ex.what()));
    }
}

std::string isearch::FileSystemIndexRepository::getInverseIndexFilePath() const {
    return _applicationDataDirectory + '/' + InverseIndexFileName;
}

void isearch::FileSystemIndexRepository::saveInverseIndex(const isearch::InverseIndex &inverseIndex) {
    std::ofstream file {getInverseIndexFilePath()};
    if (!file) {
        throw std::runtime_error("Не удалось создать файл обратного индекса");
    }

    try {
        isearch::InverseIndexSerializer serializer{file};
        serializer.serialize(inverseIndex);
    } catch (const std::exception& ex) {
        throw std::runtime_error("Ошибка при сохранении файла обратного индекса: " + std::string(ex.what()));
    }
}

static void saveDocument(const isearch::Document& document, const std::string& indexDirectory) {
    auto indexFilePath = indexDirectory + '/' + std::to_string(document.id());
    std::ofstream file {indexFilePath};
    if (!file) {
        std::cerr << "Не удалось сохранить индексный файл " << indexFilePath << " для документа " << document.title() << std::endl;
        return;
    }

    try {
        isearch::BinaryDocumentSerializer serializer {file};
        serializer.serialize(document);
    } catch (const std::exception& ex) {
        std::cerr << "Ошибка при сохранении индексного файла " << indexFilePath << " для документа " << document.title() << ". Причина: " << ex.what() << std::endl;
    }
}

static void createDirectoryIfNotExists(const std::string& path) {
    // В начале проверим, что ничего по этому пути нет
    struct stat st {};
    if (stat(path.data(), &st) == 0) {
        // Проверяем, что по указанному пути - директория
        if (S_ISDIR(st.st_mode)) {
            return;
        }

        throw std::runtime_error(std::strerror(EISDIR));
    }

    // Дальше, создаем директорию
    constexpr int directoryMode = 0700;
    if (mkdir(path.data(), directoryMode) != 0) {
        throw std::runtime_error(std::strerror(errno));
    }
}

static void createIndexFileDirectory(const std::string& path) {
    try {
        createDirectoryIfNotExists(path);
    } catch (const std::runtime_error& ex) {
        throw std::runtime_error("Ошибка при создании директории индексных файлов: " + std::string(ex.what()));
    }
}

void isearch::FileSystemIndexRepository::saveDocuments(const std::vector<isearch::Document> &documents) {
    createIndexFileDirectory(getIndexFilesDirectoryName());
    for (const auto &document: documents) {
        saveDocument(document, getIndexFilePath(document.id()));
    }
}

void isearch::FileSystemIndexRepository::createAppDataDirectory() {
    try {
        createDirectoryIfNotExists(_applicationDataDirectory);
    } catch (const std::runtime_error& ex) {
        throw std::runtime_error("Ошибка при создании директории данных приложения: " + std::string(ex.what()));
    }
}

