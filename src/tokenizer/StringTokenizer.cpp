//
// Created by ashblade on 06.03.24.
//

#include <algorithm>

#include "tokenizer/StringTokenizer.h"

isearch::StringTokenizer::StringTokenizer(std::string str): _string(std::move(str)), _index(0) { }

bool isearch::StringTokenizer::tryReadNextWord(std::string &word) {
    int startIndex = _index;
    while (_index < _string.size()) {
        // Сейчас, мы используем utf-8 в качестве кодировки
        // Слова разделяются пустыми строками (пробел, табуляция, новая строка и т.д.) и в utf-8 эти символы однобайтовые,
        // поэтому можем спокойно использовать isspace
        char ch = _string[_index];
        if (std::isspace(ch)) {
            if (_index == startIndex) {
                _index = ++startIndex;
            } else {
                break;
            }
        }

        _index++;
    }

    // Если строка не оканчивалась пустой строкой
    if (startIndex != _index) {
        word = _string.substr(startIndex, _index - startIndex + 1);
        return true;
    }

    return false;
}
