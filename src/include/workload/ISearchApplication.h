//
// Created by ashblade on 21.03.24.
//

#ifndef ISEARCH_ISEARCHAPPLICATION_H
#define ISEARCH_ISEARCHAPPLICATION_H

#include <string>
#include <vector>
#include "QueryOptions.h"

namespace isearch {
    /// @brief Объект, представляющий приложение с основными вариантами использования
    class ISearchApplication {
    private:
        /// @brief Текущая рабочая директория (в ней ищем файлы и другие данные)
        std::string _workingDirectory;
    public:
        explicit ISearchApplication(std::string workingDirectory);

        /// @brief Инициализировать директорию - создать файлы индексов и обратный индекс
        void initialize();

        /// @brief Запустить поиск по уже инициализированному репозиторию
        std::vector<std::string> query(const std::string& query, const isearch::QueryOptions& options);
    };
}

#endif //ISEARCH_ISEARCHAPPLICATION_H
