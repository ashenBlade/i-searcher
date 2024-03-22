//
// Created by ashblade on 26.02.24.
//


#include "common/Document.h"

isearch::Document::Document(long id, std::string title, const std::map<std::string, long> &&contents):
  _id(id), _title(std::move(title)), _contents(contents), _totalWordsCount(0), _hasTotalWordsCount(false) { }

isearch::Document::size_type isearch::Document::count(const std::string &word) const noexcept {
    return _contents.count(word);
}

isearch::Document::size_type isearch::Document::size() const noexcept {
    if (_hasTotalWordsCount) {
        return _totalWordsCount;
    }

    auto totalCount = 0L;
    for (const auto &data: _contents) {
        totalCount += data.second;
    }

    _totalWordsCount = totalCount;
    _hasTotalWordsCount = true;
    return totalCount;
}

std::string isearch::Document::title() const noexcept {
    return _title;
}

isearch::Document::const_iterator isearch::Document::begin() const {
    return _contents.cbegin();
}

isearch::Document::const_iterator isearch::Document::end() const {
    return _contents.cend();
}

isearch::Document::size_type isearch::Document::unique_words_count() const noexcept {
    return _contents.size();
}

isearch::Document::Document(isearch::Document &&other) noexcept:
        _contents(std::move(other._contents)),
        _hasTotalWordsCount(other._hasTotalWordsCount),
        _totalWordsCount(other._totalWordsCount) {
    other._contents.clear();
    other._hasTotalWordsCount = false;
    other._totalWordsCount = -1;
}

long isearch::Document::id() const noexcept {
    return _id;
}
