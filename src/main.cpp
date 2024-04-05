//
// Created by ashblade on 18.02.24.
//
#include <iostream>
#include <map>
#include <cstring>
#include <unistd.h>
#include <boost/program_options.hpp>
#include "workload/ISearchApplication.h"

namespace po = boost::program_options;

static void setupEnvironment();
static void printHelp(int argc, char** argv);
static void initializeRepository(int argc, char** argv);
static void findRelevantFiles(int argc, char** argv);

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
    auto workingDirectory = getWorkingDirectoryPath(argc, argv);
    return (isearch::ISearchApplication) {workingDirectory};
}

static po::options_description getInitOptionsDescription(int *parallelism) {
    po::options_description od {"Параметры init"};
    od.add_options()
            ("parallel,p", po::value<int>(parallelism), "Количество одновременных потоков, либо 0 для автоматического определения");
    return od;
}

static isearch::InitOptions getInitOptions(int argc, char** argv) {
    isearch::InitOptions options {};
    po::options_description od = getInitOptionsDescription(&options.parallelism);
    po::command_line_parser parser{argc, argv};
    parser.options(od);
    parser.allow_unregistered();
    auto parsed = parser.run();
    po::variables_map vm{};
    po::store(parsed, vm);
    po::notify(vm);
    return options;
}

static void initializeRepository(int argc, char** argv) {
    auto application = getApplication(argc, argv);
    auto options = getInitOptions(argc, argv);
    application.initialize(options);
}

static po::options_description getQueryOptionsDescription(int *v1, double* k, double *b, int *parallelism) {
    po::options_description od {"Параметры запроса"};
    od.add_options()
            ("max,m", po::value<int>(v1), "Максимальное количество документов в выводе")
            ("bm25-k,k", po::value<double>(k), "Значение k для алгоритма BM25")
            ("bm25-b,b", po::value<double>(b), "Значение b для алгоритма BM25")
            ("parallel,p", po::value<int>(parallelism), "Количество одновременных потоков, либо 0 для автоматического определения");
    return od;
}

static isearch::QueryOptions getQueryOptions(int argc, char** argv) {
    isearch::QueryOptions options {};
    po::options_description od = getQueryOptionsDescription(&options.max, &options.bm25.k, &options.bm25.b, &options.parallelism);
    po::command_line_parser parser{argc, argv};
    parser.options(od);
    parser.allow_unregistered();
    auto parsed = parser.run();
    po::variables_map vm{};
    po::store(parsed, vm);
    po::notify(vm);

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

    isearch::QueryOptions queryOptions = getQueryOptions(argc, argv);
    try {
        queryOptions.validate();
    } catch (const std::runtime_error& ex) {
        std::cerr << ex.what() << std::endl;
        exit(1);
    }

    auto workingDirectory = getWorkingDirectoryPath(userDefinedDirectory);
    isearch::ISearchApplication application {workingDirectory};

    auto result = application.query(queryString, queryOptions);

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
    std::cout << "\t" << "init ДИРЕКТОРИЯ - Инициализировать новый репозиторий для работы.\n\t\tДИРЕКТОРИЯ - директория, в которой производить поиск, может быть относительным путем, например, '.'" << std::endl;
    std::cout << "\t" << "query ЗАПРОС ДИРЕКТОРИЯ [параметры] - Найти релевантные документы из указанного запроса. \n\t\tЗАПРОС - указывается единственной строкой\n\t\tДИРЕКТОРИЯ - директория, в которой производить поиск, может быть относительным путем, например, '.'" << std::endl;
    isearch::QueryOptions options{};
    auto queryDescription = getQueryOptionsDescription(&options.max, &options.bm25.k, &options.bm25.b, &options.parallelism);
    std::cout << "\t\t" << queryDescription << std::endl;

    isearch::InitOptions initOptions {};
    auto initDescription = getInitOptionsDescription(&initOptions.parallelism);
    std::cout << "\t\t" << initDescription << std::endl;
}
