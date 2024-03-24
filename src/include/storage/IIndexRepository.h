//
// Created by ashblade on 24.03.24.
//

#ifndef ISEARCH_IINDEXREPOSITORY_H
#define ISEARCH_IINDEXREPOSITORY_H

#include "common/InverseIndex.h"
#include "common/Document.h"

namespace isearch {
    /// @brief Интерфейс для доступа к индексным файлам
    class IIndexRepository {
    public:
        /// @brief Найти документ по переданному ID
        /// @returns Указатель на найденный документ, или null если не нашлось
        virtual std::shared_ptr<isearch::Document> getDocumentById(long id) = 0;

        /// @brief Получить обратный индекс документов
        virtual isearch::InverseIndex getInverseIndex() = 0;
    };
}

#endif //ISEARCH_IINDEXREPOSITORY_H
