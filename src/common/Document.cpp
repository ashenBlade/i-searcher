//
// Created by ashblade on 26.02.24.
//


#include "Document.h"

isearch::Document::Document(std::string title, const std::map<std::string, int> &&contents):
    _title(std::move(title)), _contents(contents), _totalWordsCount(0), _hasTotalWordsCount(false) { }

isearch::Document::size_type isearch::Document::count(const std::string &word) const noexcept {
    return _contents.count(word);
}

isearch::Document::size_type isearch::Document::size() const noexcept {
    if (_hasTotalWordsCount) {
        return _totalWordsCount;
    }

    auto totalCount = 0;
    for (const auto &data: _contents) {
        totalCount += data.second;
    }

    _totalWordsCount = totalCount;
    _hasTotalWordsCount = true;
    return totalCount;
}
