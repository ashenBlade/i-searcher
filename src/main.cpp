//
// Created by ashblade on 18.02.24.
//
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <memory>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

#include "stemmer/RussianStemmer.h"
#include "tokenizer/StringTokenizer.h"
#include "tokenizer/WordCleanerTokenizerDecorator.h"
#include "tokenizer/FilterTokenizerDecorator.h"
#include "common/DocumentCollection.h"
#include "serialization/BinaryDocumentSerializer.h"
#include "serialization/BinaryDocumentDeserializer.h"
#include "tokenizer/StreamTokenizer.h"
#include "workload/ISearchApplication.h"

static void setupEnvironment();
static void printHelp(int argc, char** argv);
static void initializeRepository(int argc, char** argv);
static void findRelevantFiles(int argc, char** argv);

/// @brief Директория, в которой располагаются файлы приложения
static std::string MainDirectory = ".isearch";

/// @brief Директория, в которой находятся все индексные файлы
static std::string FilesDirectory = "files";

/// @brief Файл с инверсированным индексом
static std::string InverseIndexFile = "index";


int main(int argc, char** argv) {
    setupEnvironment();

    if (argc < 2) {
        printHelp(argc, argv);
        return 1;
    }

    if (std::strcmp(argv[1], "--help") == 0) {
        printHelp(argc, argv);
        return 0;
    }

    if (std::strcmp(argv[1], "init") == 0) {
        try {
            initializeRepository(argc, argv);
            return 0;
        } catch (const std::exception& ex) {
            std::cerr << "Ошибка при инициализации репозитория: " << ex.what();
            return 1;
        }
    }

    if (std::strcmp(argv[1], "query") == 0) {
        try {
            findRelevantFiles(argc, argv);
            return 0;
        } catch (const std::exception& ex) {
            std::cerr << "Ошибка при поиске документов: " << ex.what();
            return 1;
        }
    }

    std::cout << "Неизвестная команда: " << argv[1];
    printHelp(argc, argv);

    return 1;
}

/// @brief Получить директорию, которую будем инициализировать
static std::string getWorkingDirectoryPath(int argc, char** argv) {
    std::string result;
    // Рабочая директория может быть указана, либо нужно использовать рабочую
    if (argc > 2) {
        // Путь, который указал пользователь (может быть относительным, '.')
        // Необходимо превратить его в полный
        char buffer[256];
        auto error = realpath(argv[2], buffer);
        if (error == nullptr) {
            throw std::runtime_error("Ошибка при определении директории");
        }
        result = buffer;
    } else {
        // Пользователь не указал путь, поэтому используем текущую директорию
        result = getcwd(nullptr, 0);
    }

    return result;
}

static void initializeRepository(int argc, char** argv) {
    // В начале находим директорию, в которой будем работать
    auto repositoryPath = getWorkingDirectoryPath(argc, argv);
    isearch::ISearchApplication application {repositoryPath};
    application.initializeRepository();
}

void setupEnvironment() {
    // Чтобы мог считывать как русские так и английские символы из консоли
    setlocale(LC_ALL, "");
}

void printHelp(int argc, char** argv) {
    std::cout << "Использование: " << argv[0] << " " << "КОМАНДА" << std::endl;
    std::cout << "КОМАНДА может принимать следующие значения:" << std::endl;
    std::cout << "\t" << "init - Инициализировать новый репозиторий для работы" << std::endl;
    std::cout << "\t" << "query ЗАПРОС - Найти релевантные документы из указанного запроса. Запрос указывается единственной строкой" << std::endl;
}
