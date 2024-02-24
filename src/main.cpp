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

    auto tokenizer = isearch::StreamTokenizer(std::cin);
    std::set<std::string> stopWords = {
            u8"мир",
            u8"привет",
    };
    auto wrapped = isearch::FilterTokenizerDecorator(stopWords, tokenizer);
    std::string str {};
    while (wrapped.tryReadNextWord(str)) {
        std::cout << str << std::endl;
        str = std::string();
    }
    return 0;
}

void setupEnvironment() {

    // Чтобы мог считывать как русские так и английские символы из консоли
    setlocale(LC_ALL, "");
}
