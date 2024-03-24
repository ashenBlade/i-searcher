//
// Created by ashblade on 24.03.24.
//

#ifndef ISEARCH_FILESYSTEMINDEXREPOSITORY_H
#define ISEARCH_FILESYSTEMINDEXREPOSITORY_H

#include "storage/IIndexRepository.h"

namespace isearch {
    /// @brief Интерфейс для хранения индексных файлов в файловой системе
    class FileSystemIndexRepository: public isearch::IIndexRepository {
    private:
        /// @brief Полный путь до директории с данными для приложения.
        /// Оканчивается .isearch
        /// @example /home/user/documents/.isearch
        std::string _applicationDataDirectory;

        /// @brief Получить полный путь для индексного файла указанного документа
        std::string getIndexFilePath(long id) const;

        /// @brief Получить полный путь до директории с индексными файлами
        std::string getIndexFilesDirectoryName() const;

        /// @brief Получить полный путь файла обратного индекса
        std::string getInverseIndexFilePath() const;
    public:
        explicit FileSystemIndexRepository(std::string applicationDataDirectory);
        std::shared_ptr<isearch::Document> getDocumentById(long id) override;
        isearch::InverseIndex getInverseIndex() override;

        /// @brief Проверить и при необходимости создать директорию для данных приложения
        void createAppDataDirectory();

        /// @brief Сохранить обратный индекс в файл приложения
        void saveInverseIndex(const isearch::InverseIndex& inverseIndex);

        /// @brief Сохранить индексные файлы для указанных документов
        void saveDocuments(const std::vector<isearch::Document>& documents);
    };
}

#endif //ISEARCH_FILESYSTEMINDEXREPOSITORY_H
