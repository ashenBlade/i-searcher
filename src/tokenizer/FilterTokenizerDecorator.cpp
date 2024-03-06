//
// Created by ashblade on 21.02.24.
//

#include "tokenizer/FilterTokenizerDecorator.h"

bool isearch::FilterTokenizerDecorator::tryReadNextWord(std::string &word) {
    std::string currentRead {};
    while (_wrapped.tryReadNextWord(currentRead)) {
        if (0 < _excluded.count(currentRead)) {
            currentRead = std::string();
            continue;
        }

        word = currentRead;
        return true;
    }
    return false;
}

isearch::FilterTokenizerDecorator::FilterTokenizerDecorator(std::set<std::string>& excluded,
                                                            isearch::ITokenizer &wrapped): _excluded(excluded), _wrapped(wrapped)
                                                            { }

