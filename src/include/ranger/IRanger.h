//
// Created by ashblade on 06.03.24.
//

#ifndef ISEARCH_IRANGER_H
#define ISEARCH_IRANGER_H

#include "common/DocumentCollection.h"
#include <vector>

namespace isearch {
    /// @brief Интерфейс алгоритма ранжирования документов
    class IRanger {
    public:
        /// @brief Выполнить ранжирование документов по указанному вектору поиска
        /// @returns Список названий документов
        virtual std::vector<std::string> range(const std::vector<std::string> &search_vector, DocumentCollection &documents, int max) = 0;
    };
}

#endif //ISEARCH_IRANGER_H
