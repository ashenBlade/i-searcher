//
// Created by ashblade on 18.02.24.
//
#include <iostream>

#include "stemmer/RussianStemmer.h"
#include "tokenizer/StreamTokenizer.h"
#include "tokenizer/FilterTokenizerDecorator.h"

static void setupEnvironment();

int main(int argc, char** argv) {
    setupEnvironment();

    auto tokenizer = isearch::StreamTokenizer(std::wcin);
    std::set<std::wstring> stopWords = {
            L"мир",
            L"привет",
    };
    auto wrapped = isearch::FilterTokenizerDecorator(stopWords, tokenizer);
    std::wstring str {};
    while (wrapped.tryReadNextWord(str)) {
        std::wcout << str << std::endl;
        str = std::wstring();
    }
    return 0;
}

void setupEnvironment() {

    // Чтобы мог считывать как русские так и английские символы из консоли
    setlocale(LC_ALL, "");
}
