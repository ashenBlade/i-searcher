//
// Created by ashblade on 06.03.24.
//

#include <sstream>

#include "LocalSearchEngine.h"
#include "tokenizer/StringTokenizer.h"
#include "tokenizer/FilterTokenizerDecorator.h"
#include "tokenizer/WordCleanerTokenizerDecorator.h"
#include "stemmer/RussianStemmer.h"

isearch::LocalSearchEngine::LocalSearchEngine(isearch::DocumentCollection documents): _documents(std::move(documents)) { }

static std::set<std::string> stopwords = {
        "а",
        "в",
        "и",
        "во",
        "к",
        "бы",
        "вон",
        "с",
        "со",
        "г",
        "да",
        "е",
        "за",
        "ли",
        "м",
        "на",
        "ну",
        "о",
        "об",
        "обо",
        "уж",
        "уже",
        "хотя",
        "что",
        "чтоб",
        "чуть",
};

static std::vector<std::string> splitTokens(const std::string &query) {
    auto stringTokenizer = isearch::StringTokenizer(query);
    auto cleanerTokenizer = isearch::WordCleanerTokenizerDecorator(stringTokenizer);
    auto filterTokenizer = isearch::FilterTokenizerDecorator(stopwords, cleanerTokenizer);
    auto stemmer = isearch::RussianStemmer();
    std::string word {};
    std::vector<std::string> words {};
    while (filterTokenizer.tryReadNextWord(word)) {
        auto stemmed = stemmer.stem(word);
        if (!stemmed.empty()) {
            words.push_back(stemmed);
        }
    }

    return words;
}

std::vector<std::string> isearch::LocalSearchEngine::search(const std::string &query, int max) {
    // Разбиваем строку запроса на отдельные токены
    auto tokens = splitTokens(query);


    // Заглушка
    return {};
}


