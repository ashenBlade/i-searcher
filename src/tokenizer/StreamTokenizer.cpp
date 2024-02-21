//
// Created by ashblade on 21.02.24.
//

#include "StreamTokenizer.h"
#include <system_error>

isearch::StreamTokenizer::StreamTokenizer(std::wistream &stream) noexcept: _stream(stream), _end(false) {  }

static void skipWhiteSpaces(std::wistream &stream) {
    while (stream && iswspace(stream.peek())) {
        (void) stream.get();
    }
}

bool isearch::StreamTokenizer::tryReadNextWord(std::wstring& word) {
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
    wchar_t newWordBuffer[maxBufferLength];

    // Индекс для вставки нового слова
    constexpr int noDataIndex = -1;
    int index = noDataIndex;

    while (_stream.good()) {
        wchar_t read = 0;
        _stream >> read;
        if (_stream.fail() || iswspace(read))
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
