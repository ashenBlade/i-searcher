//
// Created by ashblade on 20.03.24.
//

#include "common/InverseIndex.h"

isearch::InverseIndex::InverseIndex(isearch::InverseIndex::inverse_index_data&& data): _data(data) {

}

isearch::InverseIndex::InverseIndex(isearch::InverseIndex &&other) noexcept: _data(std::move(other._data)) { }

std::shared_ptr<std::vector<long>> isearch::InverseIndex::getDocuments(const std::string &token) const {
    auto it = _data.find(token);
    if (it != _data.end()) {
        return it->second;
    }

    return nullptr;
}

long isearch::InverseIndex::get_tokens_count() const {
    return static_cast<long>(_data.size());
}

isearch::InverseIndex::inverse_index_iterator isearch::InverseIndex::begin() const {
    return _data.cbegin();
}

isearch::InverseIndex::inverse_index_iterator isearch::InverseIndex::end() const {
    return _data.cend();
}

isearch::InverseIndex isearch::InverseIndex::build(const std::vector<isearch::Document> &documents) {
    isearch::InverseIndex::inverse_index_data payload {};

    // Проходимся по каждому токену в каждом документе и добавляем документ в список содержащих токен
    for (const auto &document: documents) {
        for (const auto &token_data: document) {
            // Предполагаю, что повторов токенов (и соответственно id документов) нет

            auto token = token_data.first;
            auto it = payload.find(token);
            if (it == payload.end()) {
                auto vector = std::make_shared<std::vector<long>>();
                vector->push_back(document.id());
                payload[token] = vector;
            } else {
                it->second->push_back(document.id());
            }
        }
    }

    return isearch::InverseIndex(std::move(payload));
}


