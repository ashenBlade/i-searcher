//
// Created by ashblade on 24.03.24.
//

#ifndef ISEARCH_DOCUMENTPARSER_H
#define ISEARCH_DOCUMENTPARSER_H

#include <vector>
#include "common/Document.h"

namespace isearch {
    /// @brief Вспомогательный класс для парсинга всех документов из указанной директории
    class DocumentParser {
    private:
        /// @brief Полный путь до директории, из которой необходимо спарсить документы
        std::string _directoryPath;
        /// @brief Проверить, что файл релевантен
        static bool isRelevantFile(const std::string &filename);

        /// @brief Получить названия всех файлов в директории
        std::vector<std::string> getAllDirectoryFileNames() const;
    public:
        explicit DocumentParser(std::string directoryPath);

        /// @brief Спарсить все документы из директории
        std::vector<isearch::Document> parseAllDocumentsInDirectory() const;

        /// @brief Спарсить все документы из директории, используя параллельные потоки
        std::vector<isearch::Document> parseAllDocumentsInDirectoryParallel(int parallelCount) const;
    };
}

#endif //ISEARCH_DOCUMENTPARSER_H
