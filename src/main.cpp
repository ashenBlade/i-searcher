//
// Created by ashblade on 18.02.24.
//
#include <iostream>
#include <map>

#include "stemmer/RussianStemmer.h"
#include "tokenizer/StreamTokenizer.h"
#include "tokenizer/FilterTokenizerDecorator.h"
#include "tokenizer/WordCleanerTokenizerDecorator.h"
#include "DocumentCollection.h"

static void setupEnvironment();

int main(int argc, char** argv) {
    setupEnvironment();

    auto collection = isearch::DocumentCollection(std::vector<isearch::Document>{
        isearch::Document(std::map<std::string, int> {
                {"hello", 123},
                {u8"мир", 11}
        }),
        isearch::Document(std::map<std::string, int> {
                {u8"привет", 1},
                {"аааааа", 11}
        }),
        isearch::Document(std::map<std::string, int> {
                {u8"привет", 1},
                {"аааааа", 11},
        })
    });
    std::cout << "Количество документов: " << collection.size() << std::endl
              << "Средний размер документа: " << collection.avg_count() << std::endl;


    for (const auto &doc: collection) {
        std::cout << "Размер документа: " << doc.size() << std::endl;
    }


    return 0;
}

void setupEnvironment() {

    // Чтобы мог считывать как русские так и английские символы из консоли
    setlocale(LC_ALL, "");
}
