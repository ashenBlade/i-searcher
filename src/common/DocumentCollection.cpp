//
// Created by ashblade on 26.02.24.
//

#include "common/DocumentCollection.h"
#include <algorithm>

isearch::DocumentCollection::DocumentCollection(const std::vector<Document> &documents) noexcept:
    _documents(documents),
    _avgDocumentSize(-1),
    _hasAvgDocumentSize(false) {  }

isearch::DocumentCollection::DocumentCollection(std::vector<Document> &&documents) noexcept:
        _documents(documents),
        _avgDocumentSize(-1),
        _hasAvgDocumentSize(false) {  }

size_t isearch::DocumentCollection::size() const {
    return _documents.size();
}

size_t isearch::DocumentCollection::count(const std::string &word) const {
    return std::count_if(_documents.begin(), _documents.end(), [=](const Document &doc) {return doc.count(word) > 0;});
}

static size_t calculateMedianDocumentSize(const std::vector<isearch::Document> &documents) {
    std::vector<size_t> sizes {};
    sizes.reserve(documents.size());
    for (const auto &doc: documents) {
        sizes.push_back(doc.size());
    }

    // Поиск реализуем с помощью n-ой статистики вместо сортировки всего массива - может быть быстрее
    auto medianIndex = sizes.size() / 2;
    auto medianIterator = sizes.begin() + (long) medianIndex;
    std::nth_element(sizes.begin(), medianIterator, sizes.end());
    return sizes[medianIndex];
}

size_t isearch::DocumentCollection::avg_count() const {
    if (_hasAvgDocumentSize) {
        return _avgDocumentSize;
    }

    // В качестве среднего размера документа используем медиану, а не среднее арифметическое или моду
    auto avg = calculateMedianDocumentSize(_documents);
    _avgDocumentSize = avg;
    _hasAvgDocumentSize = true;
    return avg;
}

isearch::DocumentCollection::DocumentIterator::DocumentIterator(isearch::DocumentCollection &collection, size_t index) noexcept:
    _collection(collection),
    _index(index) {  }

isearch::DocumentCollection::DocumentIterator isearch::DocumentCollection::begin() noexcept {
    return DocumentIterator(*this, 0);
}

isearch::DocumentCollection::DocumentIterator isearch::DocumentCollection::end() noexcept {
    return DocumentIterator(*this, _documents.size());
}

isearch::DocumentCollection::DocumentIterator &isearch::DocumentCollection::DocumentIterator::operator++() {
    _index++;
    return *this;
}

isearch::DocumentCollection::DocumentIterator isearch::DocumentCollection::DocumentIterator::operator++(int) {
    auto saved = *this;
    ++(*this);
    return saved;
}

isearch::Document &isearch::DocumentCollection::DocumentIterator::operator*() {
    return _collection._documents[_index];
}

isearch::Document *isearch::DocumentCollection::DocumentIterator::operator->() {
    return _collection._documents.data() + _index;
}

isearch::DocumentCollection::DocumentIterator::DocumentIterator(const isearch::DocumentCollection::DocumentIterator &other) noexcept:
    _collection(other._collection), _index(other._index) { }


isearch::DocumentCollection::DocumentIterator::DocumentIterator(isearch::DocumentCollection::DocumentIterator &&other) noexcept:
        _collection(other._collection), _index(other._index) {
    // Если индекс равен размеру коллекции, то дошли до конца
    other._index = other._collection._documents.size() + 1;
}

bool
isearch::DocumentCollection::DocumentIterator::operator==(const isearch::DocumentCollection::DocumentIterator &other) const noexcept {
    return this->_index == other._index;
}

bool isearch::DocumentCollection::DocumentIterator::operator!=(
        const isearch::DocumentCollection::DocumentIterator &other) const noexcept {
    return this->_index != other._index;
}



