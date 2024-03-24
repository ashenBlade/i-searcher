//
// Created by ashblade on 20.03.24.
//

#ifndef ISEARCH_INVERSEINDEX_H
#define ISEARCH_INVERSEINDEX_H

#include <string>
#include <map>
#include <vector>
#include <memory>

namespace isearch {
    class InverseIndex {
    public:
        using inverse_index_data = std::map<std::string, std::shared_ptr<std::vector<long>>>;
    private:
        /// @brief Отображение токена на множество номеров документов, которые содержат этот токен
        inverse_index_data _data;
    public:
        explicit InverseIndex(inverse_index_data&& data);
        InverseIndex(InverseIndex&& other) noexcept;
        InverseIndex(const InverseIndex&) = delete;

        /// @brief Получить ссылку на список документов, которые содержат этот токен, либо null, если такого токена не найдено
        std::shared_ptr<std::vector<long>> getDocuments(const std::string &token) const;

        /// @brief Получить общее число различных токенов
        long get_tokens_count() const;

        using inverse_index_iterator = inverse_index_data::const_iterator;

        inverse_index_iterator begin() const;
        inverse_index_iterator end() const;
    };
}

#endif //ISEARCH_INVERSEINDEX_H
