//
// Created by ashblade on 21.03.24.
//

#ifndef ISEARCH_ISEARCHAPPLICATION_H
#define ISEARCH_ISEARCHAPPLICATION_H

#include <string>

namespace isearch {
    class ISearchApplication {
    private:
        /// @brief Текущая рабочая директория (в ней ищем файлы и другие данные)
        std::string _workingDirectory;
        /// @brief Название директории с данными приложения
        /// @example .isearch
        std::string _applicationDirectory;
        /// @brief Название файла инвертированного индекса
        /// @example inverse-index
        std::string _inverseIndexFileName;
        /// @brief Название директории с индексами документов
        /// @example documents
        std::string _indexDirectoryName;

        std::string getApplicationDirPath() const;
        std::string getInverseIndexPath() const;
        std::string getIndexDirectoryPath() const;
    public:
        explicit ISearchApplication(std::string repositoryDirectory, std::string applicationDirectory, std::string inverseIndexFileName, std::string indexDirectoryName);

        /// @brief Инициализировать репозиторий
        void initializeRepository();
    };
}

#endif //ISEARCH_ISEARCHAPPLICATION_H
