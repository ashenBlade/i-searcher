//
// Created by ashblade on 18.02.24.
//
#include <iostream>
#include <map>
#include <cstring>
#include <unistd.h>

#include "workload/ISearchApplication.h"

static void setupEnvironment();
static void printHelp(int argc, char** argv);
static void initializeRepository(int argc, char** argv);
static void findRelevantFiles(int argc, char** argv);

/// @brief Директория, в которой располагаются файлы приложения
static std::string DataDirectoryName = ".isearch";

/// @brief Директория, в которой находятся все индексные файлы
static std::string IndexFilesDirectoryName = "files";

/// @brief Файл с инверсированным индексом
static std::string InverseIndexFileName = "index";


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

static std::string getWorkingDirectoryPath(std::string userDefinedPath) {
    char buffer[256];
    auto error = realpath(userDefinedPath.data(), buffer);
    if (error == nullptr) {
        throw std::runtime_error("Ошибка при определении директории");
    }
    return buffer;
}

static isearch::ISearchApplication getApplication(int argc, char** argv) {
    auto repositoryPath = getWorkingDirectoryPath(argc, argv);
    return (isearch::ISearchApplication) {repositoryPath, DataDirectoryName, InverseIndexFileName,
                                          IndexFilesDirectoryName};
}

static void initializeRepository(int argc, char** argv) {
    auto application = getApplication(argc, argv);
    application.initialize();
}

struct application_options {
    int max_output = 5;
};

static application_options getApplicationOptions(int argc, char** argv) {
    application_options options {};
    int option;
    while ((option = getopt(argc, argv, "n")) != -1) {
        switch (option) {
            case 'n':
                options.max_output = std::stoi(optarg);
                break;
        }
    }

    return options;
}

static void findRelevantFiles(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Строка запроса не указана" << std::endl;
        exit(1);
    }

    auto queryString = std::string(argv[2]);
    if (queryString.empty()) {
        std::cerr << "Строка запроса пустая" << std::endl;
        exit(1);
    }

    if (argc < 4) {
        std::cerr << "Директория с файлами не указана" << std::endl;
        exit(1);
    }

    auto userDefinedDirectory = std::string(argv[3]);
    if (userDefinedDirectory.empty()) {
        std::cerr << "Директория с файлами не указана" << std::endl;
        exit(1);
    }

    auto workingDirectory = getWorkingDirectoryPath(userDefinedDirectory);
    auto options = getApplicationOptions(argc, argv);
    isearch::ISearchApplication application {workingDirectory, DataDirectoryName, InverseIndexFileName, IndexFilesDirectoryName};

    auto result = application.query(queryString, options.max_output);

    for (const auto &title: result) {
        std::cout << title << std::endl;
    }
}

void setupEnvironment() {
    // Чтобы мог считывать как русские так и английские символы из консоли
    setlocale(LC_ALL, "");
}

void printHelp(int argc, char** argv) {
    std::cout << "Использование: " << argv[0] << " " << "КОМАНДА" << std::endl;
    std::cout << "КОМАНДА может принимать следующие значения:" << std::endl;
    std::cout << "\t" << "init - Инициализировать новый репозиторий для работы" << std::endl;
    std::cout << "\t" << "query ЗАПРОС [-n МАКС] - Найти релевантные документы из указанного запроса. Запрос указывается единственной строкой. -n МАКС - максимальное количество документов для вывода" << std::endl;
}
