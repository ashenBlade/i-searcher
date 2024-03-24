//
// Created by ashblade on 24.02.24.
//

#ifndef ISEARCH_ISEARCHENGINE_H
#define ISEARCH_ISEARCHENGINE_H

#include <vector>
#include <string>

namespace isearch {
    /// @brief Поисковый движок для нахождения релевантных документов
    class ISearchEngine {
    public:
        /// @brief Произвести поиск по документам, используя указанную строку запроса
        /// @returns Список из ранжированных документов, соответствующих результату поиска
        virtual std::vector<std::string> search(const std::string &query, int max) = 0;

        virtual ~ISearchEngine() = default;
    };
}

#endif //ISEARCH_IRANGER_H
