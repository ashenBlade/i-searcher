//
// Created by ashblade on 21.02.24.
//

#include "tokenizer/StreamTokenizer.h"
#include <system_error>

isearch::StreamTokenizer::StreamTokenizer(std::istream &stream) noexcept: _stream(stream), _end(false) {  }

static void skipWhiteSpaces(std::istream &stream) {
    while (stream && isspace(stream.peek())) {
        (void) stream.get();
    }
}

bool isearch::StreamTokenizer::tryReadNextWord(std::string& word) {
    if (_end) {
        return false;
    }

    if (!_stream) {
        _end = true;
        return false;
    }

    skipWhiteSpaces(_stream);
    if (_stream.fail()) {
        _end = true;
        return false;
    }

    // Самое длинное слово - 35 символов, но сделаем больше на всякий случай
    constexpr int maxBufferLength = 256;
    char newWordBuffer[maxBufferLength];

    // Индекс для вставки нового слова
    constexpr int noDataIndex = -1;
    int index = noDataIndex;

    char read = 0;
    while (_stream.get(read)) {
        if (_stream.fail() || isspace(read))
        {
            if (_stream.fail()) {
                _end = true;
            }
            break;
        }

        newWordBuffer[++index] = read;
        if (index == maxBufferLength - 1) {
            break;
        }
    }

    if (index == noDataIndex) {
        return false;
    }

    word.append(newWordBuffer, index + 1);
    return true;
}

isearch::StreamTokenizer::~StreamTokenizer() { _end = true; }
