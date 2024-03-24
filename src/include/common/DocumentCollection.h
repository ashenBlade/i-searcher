//
// Created by ashblade on 26.02.24.
//

#ifndef ISEARCH_DOCUMENTCOLLECTION_H
#define ISEARCH_DOCUMENTCOLLECTION_H

#include "Document.h"
#include <vector>

namespace isearch {
    /// @brief Представление множества документов с удобными методами
    class DocumentCollection {
    private:
        const std::vector<Document>& _documents;
        mutable bool _hasAvgDocumentSize;
        mutable Document::size_type _avgDocumentSize;
    public:
        explicit DocumentCollection(const std::vector<Document>& documents) noexcept;
        explicit DocumentCollection(std::vector<Document>&& documents) noexcept;
        DocumentCollection(const DocumentCollection& other) = delete;

        /// @brief Общее число документов
        size_t size() const;

        /// @brief Получить количество документов, которое содержит указанное слово
        size_t count(const std::string &word) const;

        /// @brief Средний размер документа
        size_t avg_count() const;

        class DocumentIterator: public std::iterator<std::input_iterator_tag, Document> {
        private:
            DocumentCollection& _collection;
            size_t _index;
        public:
            friend class DocumentCollection;
            explicit DocumentIterator(DocumentCollection& collection, size_t index) noexcept;
            DocumentIterator(const DocumentIterator& other) noexcept;
            DocumentIterator(DocumentIterator&& other) noexcept;

            bool operator==(const DocumentIterator &other) const noexcept;
            bool operator!=(const DocumentIterator &other) const noexcept;

            DocumentIterator& operator++();
            DocumentIterator operator++(int);
            const Document& operator*();
            const Document* operator->();
        };

        DocumentIterator begin() noexcept;
        DocumentIterator end() noexcept;
    };
}

#endif //ISEARCH_DOCUMENTCOLLECTION_H
