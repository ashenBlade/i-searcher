//
// Created by ashblade on 24.02.24.
//

#ifndef ISEARCH_IRANGER_H
#define ISEARCH_IRANGER_H

#include "DocumentCollection.h"

namespace isearch {
    class ISearchEngine {
    public:
        /// @brief Произвести поиск по документам, используя указанную строку запроса
        /// @returns Список из ранжированных документов, соответствующих результату поиска
        virtual std::vector<std::string> search(const std::string &query, int max) = 0;
    };
}

#endif //ISEARCH_IRANGER_H
