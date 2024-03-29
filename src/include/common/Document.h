//
// Created by ashblade on 24.02.24.
//

#ifndef ISEARCH_DOCUMENT_H
#define ISEARCH_DOCUMENT_H

#include <string>
#include <map>

namespace isearch {
    /// @brief Документ - это индексированный текст
    class Document {
    public:
        using size_type = size_t;

        explicit Document(long id, std::string title, const std::map<std::string, long>&& contents);
        Document(Document&& other) noexcept;
        Document(const Document&) = delete;

        /// @brief Получить количество слов равных переданному в документе
        size_type count(const std::string &word) const noexcept;

        /// @brief Получить количество различных слов в документе
        size_type tokens_count() const noexcept;

        /// @brief Получить общее число слов в документе
        size_type size() const noexcept;

        /// @brief Получить название документа
        std::string title() const noexcept;

        /// @brief Id документа
        long id() const noexcept;

    using const_iterator = std::map<std::string, long>::const_iterator;
    const_iterator begin() const;
    const_iterator end() const;

    private:
        long _id;
        std::string _title;
        std::map<std::string, long> _contents;
        mutable size_type _totalWordsCount;
        mutable bool _hasTotalWordsCount;
    };
}

#endif //ISEARCH_DOCUMENT_H
