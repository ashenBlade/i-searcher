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
        /// @brief Рабочая директория
        /// @example /home/user/documents/
        std::string _workingDirectory;

        /// @brief Полный путь до директории с данными приложения
        std::string getApplicationDataDirectoryPath() const;

        /// @brief Полный путь для индексного файла указанного документа
        std::string getIndexFilePath(long id) const;

        /// @brief Полный путь до директории с индексными файлами
        std::string getIndexFilesDirectoryName() const;

        /// @brief Полный путь файла обратного индекса
        std::string getInverseIndexFilePath() const;
    public:
        /// @param workingDirectory Рабочая директория, из которой ведется работа
        explicit FileSystemIndexRepository(std::string workingDirectory);
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
