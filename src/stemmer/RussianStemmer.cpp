//
// Created by ashblade on 18.02.24.
//

#include <sstream>
#include <algorithm>
#include <cassert>
#include "RussianStemmer.h"

static const std::vector<std::string> Vowels = {
        u8"а",
        u8"е",
        u8"ё",
        u8"и",
        u8"о",
        u8"у",
        u8"ы",
        u8"э",
        u8"ю",
        u8"я",
};

static const std::vector<std::string> PerfectGerund = {
        // TODO: начинаться с А или Я
        // Группа 1
        u8"ав",
        u8"авши",
        u8"авшись",
        u8"яв",
        u8"явши",
        u8"явшись",

        // Группа 2
        u8"ив",
        u8"ивши",
        u8"ившись",
        u8"ы",
        u8"ывши",
        u8"ывшись",
};

static const std::vector<std::string> Adjective = {
        u8"ее",
        u8"ие",
        u8"ые",
        u8"ое",
        u8"ими",
        u8"ыми",
        u8"ей",
        u8"ий",
        u8"ый",
        u8"ой",
        u8"ем",
        u8"им",
        u8"ым",
        u8"ом",
        u8"его",
        u8"ого",
        u8"ему",
        u8"ому",
        u8"их",
        u8"ых",
        u8"ию",
        u8"юю",
        u8"ая",
        u8"яя",
        u8"ою",
        u8"ею",
};

static const std::vector<std::string> Participle = {
        // Группа 1
        u8"аем",
        u8"анн",
        u8"авш",
        u8"ающ",
        u8"ащ",
        u8"яем",
        u8"янн",
        u8"явш",
        u8"яющ",
        u8"ящ",

        // Группа 2
        u8"ивш",
        u8"ывш",
        u8"ующ",
};

static const std::vector<std::string> Reflexive = {
        u8"ся",
        u8"сь"
};

// TODO: перед окончаниями группы 1 должна быть А или Я
static const std::vector<std::string> Verb = {
        // Группа 1
        u8"ала",
        u8"ана",
        u8"аете",
        u8"айте",
        u8"али",
        u8"ай",
        u8"ал",
        u8"аем",
        u8"ан",
        u8"ало",
        u8"ано",
        u8"ает",
        u8"ают",
        u8"аны",
        u8"ать",
        u8"аешь",
        u8"анно",

        u8"яла",
        u8"яна",
        u8"яете",
        u8"яйте",
        u8"яли",
        u8"яй",
        u8"ял",
        u8"яем",
        u8"яня",
        u8"яло",
        u8"яно",
        u8"яет",
        u8"яют",
        u8"яны",
        u8"ять",
        u8"яешь",
        u8"янно",

        // Группа 2
        u8"ила",
        u8"ыла",
        u8"ена",
        u8"ейте",
        u8"уйте",
        u8"ите",
        u8"или",
        u8"ыли",
        u8"ей",
        u8"уй",
        u8"ил",
        u8"ыл",
        u8"им",
        u8"ым",
        u8"ен",
        u8"ило",
        u8"ыло",
        u8"ено",
        u8"ят",
        u8"ует",
        u8"уют",
        u8"ит",
        u8"ыт",
        u8"ены",
        u8"ить",
        u8"ыть",
        u8"ишь",
        u8"ую",
        u8"ю",
};

static const std::vector<std::string> Noun = {
        u8"а",
        u8"ев",
        u8"ов",
        u8"ие",
        u8"ье",
        u8"е",
        u8"иями",
        u8"ями",
        u8"ами",
        u8"еш",
        u8"ии",
        u8"и",
        u8"ией",
        u8"ей",
        u8"ой",
        u8"ий",
        u8"й",
        u8"иям",
        u8"ям",
        u8"ием",
        u8"ем",
        u8"ам",
        u8"ом",
        u8"о",
        u8"у",
        u8"ах",
        u8"иях",
        u8"ях",
        u8"ы",
        u8"ь",
        u8"ию",
        u8"ью",
        u8"ю",
        u8"ия",
        u8"ья",
        u8"я",
};

static std::vector<std::string> Derivational = {
        u8"ост",
        u8"ость",
};

static std::vector<std::string> TidyUp = {
        u8"ейш",
        u8"ейше",
        u8"ейше",
        u8"ь",
};

static std::string normalize(const std::string &word, std::string &result) {
    std::transform(word.cbegin(), word.cend(), result.begin(), std::towlower);
    return result;
}

static bool endsWith(const std::string &string, const std::string &ending) {
    if (string.length() < ending.length()) {
        return false;
    }

    return std::equal(ending.rbegin(), ending.rend(), string.rbegin());
}

static bool try_remove_if_ends(std::string &word, const std::string &end) {
    if (endsWith(word, end)) {
        word.erase(word.length() - end.length(), end.length());
        return true;
    }
    return false;
}

static bool try_remove_if_ends_any(std::string &word, const std::vector<std::string> &endings) {
    for (const auto &end: endings) {
        if (try_remove_if_ends(word, end)) {
            return true;
        }
    }

    return false;
}

static bool try_remove_perfect_gerund(std::string &word) {
    return try_remove_if_ends_any(word, PerfectGerund);
}

static void maybe_remove_reflective(std::string &word) {
    try_remove_if_ends_any(word, Reflexive);
}

static bool try_remove_adjectival(std::string &word) {

    return try_remove_if_ends_any(word, Adjective) ||
           try_remove_if_ends_any(word, Participle);
}

static bool try_remove_noun(std::string &word) {
    return try_remove_if_ends_any(word, Noun);
}

static bool try_remove_verb(std::string &word) {
    return try_remove_if_ends_any(word, Verb);
}

static void maybe_remove_ending_i(std::string &word) {
    if (endsWith(word, u8"и")) {
        word.erase(word.length() - 1, 1);
    }
}

/// @brief Получить индексы первой гласной и символа после нее внутри слова
/// @returns Пару из индекса первой гласной и символа после нее, либо npos и npos, если не найдено или гласная находится в конце

static std::pair<std::string::size_type, std::string::size_type> extract_middle_vowel_index(const std::string &word) {
    // Найденный первый индекс
    auto first_index = std::string::npos;
    // Индекс найденного последнего слова (опираться надо на предыдущий индекс)
    auto found_word_index = -1;
    for (int word_index = 0; word_index < Vowels.size(); ++word_index) {
        auto current_index = word.find(Vowels[word_index]);
        if (current_index != std::string::npos && current_index < first_index) {
            first_index = current_index;
            found_word_index = word_index;
        }
    }

    // Если гласной нет, то тут все просто - возвращаем, что не нашли
    if (first_index == std::string::npos) {
        return std::make_pair(std::string::npos, std::string::npos);
    }

    assert(0 <= found_word_index && found_word_index < Vowels.size());

    // В противном случае, необходимо проверить, что найденная гласная - не последний символ.
    // Проверка нужна больше для оптимизации, т.к. работа будет вестись с подстрокой после последней гласной, а операции с пустой строкой - это no-op
    auto next_character_index = first_index + Vowels[found_word_index].length();
    return word.length() <= next_character_index
           ? std::make_pair(std::string::npos, std::string::npos)
           : std::make_pair(first_index, next_character_index);
}

static void maybe_remove_derivational(std::string &word) {
    try_remove_if_ends_any(word, Derivational);
}

static void tidy_up(std::string &word) {
    try_remove_if_ends_any(word, TidyUp);
}

std::string isearch::RussianStemmer::stem(const std::string &word) {
    /*
     * Используется алгоритм стемминга Snowflake.
     * Описан на странице https://snowballstem.org/algorithms/russian/stemmer.html
     * Реализация построена на основании описанного алгоритма
     */

    // Вначале нормализуем строку и разбиваем ее на 2 части - до (включительно) и после первой гласной
    auto pair = extract_middle_vowel_index(word);
    auto first_vowel_index = std::get<0>(pair);
    auto next_char_index = std::get<1>(pair);
    // TODO: если последняя гласная, то
    if (first_vowel_index == std::string::npos || first_vowel_index == word.length() - 1) {
        // Гласных нет вообще или только последняя буква гласная - т.е. изменить нельзя
        return word;
    }

    // Так как работа должна вестить только с частью слова после первой гласной, то сразу обрежем и нормализуем
    auto rv = word.substr(next_char_index);

    // P.S. Не знаю, могут ли быть ошибки, если итерация и изменение одной и той же строки
    normalize(rv, rv);

    // В самом начале удаляем Perfect Gerund (не нашел аналога названия в русском языке)
    if (try_remove_perfect_gerund(rv)) {
        // Ничего
    } else {
        // По возможности удаляем -ся/-сь - окончания возвратной формы глагола
        maybe_remove_reflective(rv);

        // Дальше удаляем одно из
        try_remove_adjectival(rv)  // Прилагательное
        || try_remove_verb(rv)     // Глагол
        || try_remove_noun(rv);    // Существительное
    }

    // По возможности удаляем последнюю 'и'
    maybe_remove_ending_i(rv);

    // Убираем окончания различных производных форм
    maybe_remove_derivational(rv);

    tidy_up(rv);

    // Создаем готовую строку - конкатенация префикса и измененной части
    auto result = word.substr(0, first_vowel_index + 1);
    normalize(result, result);
    result.append(rv);

    return result;
}

