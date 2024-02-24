//
// Created by ashblade on 24.02.24.
//

#include "WordCleanerTokenizerDecorator.h"

#include <vector>
#include <set>
#include <algorithm>

static bool is_punctuation(char ch) {
    // Использую функцию из стандартной библиотеки, т.к.
    // 1. Может учитывать локаль
    // 2. Реализация, возможно, быстрее
    // 3. Знаки пунктуации в utf-8 те же самые, что и в ascii (а других мне и не надо)
    return std::ispunct(ch);
}

bool isearch::WordCleanerTokenizerDecorator::tryReadNextWord(std::string &word) {
    std::string str {};
    while (_tokenizer.tryReadNextWord(str)) {
        str.erase(std::remove_if(str.begin(), str.end(), is_punctuation), str.end());
        if (str.empty()) {
            str = std::string();
            continue;
        }

        word = str;
        return true;
    }

    return false;
}

isearch::WordCleanerTokenizerDecorator::WordCleanerTokenizerDecorator(isearch::ITokenizer &tokenizer): _tokenizer(tokenizer) {}
