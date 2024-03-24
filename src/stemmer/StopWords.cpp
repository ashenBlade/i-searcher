//
// Created by ashblade on 24.03.24.
//

#include "stemmer/StopWords.h"

static const std::set<std::string> RussianStopWords {
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

const std::set<std::string> &isearch::StopWords::russian() {
    return RussianStopWords;
}
