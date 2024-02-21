//
// Created by ashblade on 18.02.24.
//
#include <iostream>

#include "stemmer/RussianStemmer.h"

int main(int argc, char** argv) {
    auto stemmer = isearch::RussianStemmer();
    std::wstring word = L"важную";
    auto normalized = stemmer.stem(word);
    std::wcout << normalized << std::endl;
    return 0;
}