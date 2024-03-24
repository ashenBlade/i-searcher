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
        static bool isRelevantFile(const std::string &filename) ;
    public:
        explicit DocumentParser(std::string directoryPath);

        /// @brief Спарсить все документы из директории
        std::vector<isearch::Document> parseDocuments() const;
    };
}

#endif //ISEARCH_DOCUMENTPARSER_H
