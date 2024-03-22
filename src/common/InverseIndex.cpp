//
// Created by ashblade on 20.03.24.
//

#include "common/InverseIndex.h"

isearch::InverseIndex::InverseIndex(isearch::InverseIndex::inverse_index_data&& data): _data(data) {

}

isearch::InverseIndex::InverseIndex(isearch::InverseIndex &&other) noexcept: _data(std::move(other._data)) { }

std::shared_ptr<std::vector<long>> isearch::InverseIndex::get_documents(const std::string &token) const {
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


