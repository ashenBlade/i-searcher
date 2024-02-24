//
// Created by ashblade on 21.02.24.
//

#include "FilterTokenizerDecorator.h"

bool isearch::FilterTokenizerDecorator::tryReadNextWord(std::wstring &word) {
    while (_wrapped.tryReadNextWord(word)) {
        if (0 < _excluded.count(word)) {
            continue;
        }

        return true;
    }
    return false;
}

isearch::FilterTokenizerDecorator::FilterTokenizerDecorator(std::set<std::wstring>& excluded,
                                                            isearch::ITokenizer &wrapped): _excluded(excluded), _wrapped(wrapped)
                                                            { }
