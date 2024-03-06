//
// Created by ashblade on 24.02.24.
//

#ifndef ISEARCH_DOCUMENT_H
#define ISEARCH_DOCUMENT_H

#include <string>
#include <map>

namespace isearch {
    class Document {
    public:
        using size_type = size_t;
        explicit Document(std::string title, const std::map<std::string, int>&& contents);

        /// @brief Получить количество слов равных переданному в документе
        size_type count(const std::string &word) const noexcept;

        /// @brief Получить общее число слов в документе
        size_type size() const noexcept;

        /// @brief Получить название документа
        std::string title() const noexcept;

    private:
        std::string _title;
        std::map<std::string, int> _contents;
        mutable size_type _totalWordsCount;
        mutable bool _hasTotalWordsCount;
    };
}

#endif //ISEARCH_DOCUMENT_H
