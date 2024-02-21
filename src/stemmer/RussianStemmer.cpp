//
// Created by ashblade on 18.02.24.
//

#include <sstream>
#include <algorithm>
#include "RussianStemmer.h"

static const wchar_t Vowels[] = {
        L'а',
        L'е',
        L'ё',
        L'и',
        L'о',
        L'у',
        L'ы',
        L'э',
        L'ю',
        L'я',
};

static const std::vector<std::wstring> PerfectGerund = {
        // TODO: начинаться с А или Я
        // Группа 1
        L"ав",
        L"авши",
        L"авшись",
        L"яв",
        L"явши",
        L"явшись",

        // Группа 2
        L"ив",
        L"ивши",
        L"ившись",
        L"ы",
        L"ывши",
        L"ывшись",
};

static const std::vector<std::wstring> Adjective = {
        L"ее",
        L"ие",
        L"ые",
        L"ое",
        L"ими",
        L"ыми",
        L"ей",
        L"ий",
        L"ый",
        L"ой",
        L"ем",
        L"им",
        L"ым",
        L"ом",
        L"его",
        L"ого",
        L"ему",
        L"ому",
        L"их",
        L"ых",
        L"ию",
        L"юю",
        L"ая",
        L"яя",
        L"ою",
        L"ею",
};

static const std::vector<std::wstring> Participle = {
        // Группа 1
        L"аем",
        L"анн",
        L"авш",
        L"ающ",
        L"ащ",
        L"яем",
        L"янн",
        L"явш",
        L"яющ",
        L"ящ",

        // Группа 2
        L"ивш",
        L"ывш",
        L"ующ",
};

static const std::vector<std::wstring> Reflexive = {
        L"ся",
        L"сь"
};

// TODO: перед окончаниями группы 1 должна быть А или Я
static const std::vector<std::wstring> Verb = {
        // Группа 1
        L"ала",
        L"ана",
        L"аете",
        L"айте",
        L"али",
        L"ай",
        L"ал",
        L"аем",
        L"ан",
        L"ало",
        L"ано",
        L"ает",
        L"ают",
        L"аны",
        L"ать",
        L"аешь",
        L"анно",

        L"яла",
        L"яна",
        L"яете",
        L"яйте",
        L"яли",
        L"яй",
        L"ял",
        L"яем",
        L"яня",
        L"яло",
        L"яно",
        L"яет",
        L"яют",
        L"яны",
        L"ять",
        L"яешь",
        L"янно",

        // Группа 2
        L"ила",
        L"ыла",
        L"ена",
        L"ейте",
        L"уйте",
        L"ите",
        L"или",
        L"ыли",
        L"ей",
        L"уй",
        L"ил",
        L"ыл",
        L"им",
        L"ым",
        L"ен",
        L"ило",
        L"ыло",
        L"ено",
        L"ят",
        L"ует",
        L"уют",
        L"ит",
        L"ыт",
        L"ены",
        L"ить",
        L"ыть",
        L"ишь",
        L"ую",
        L"ю",
};

static const std::vector<std::wstring> Noun = {
        L"а",
        L"ев",
        L"ов",
        L"ие",
        L"ье",
        L"е",
        L"иями",
        L"ями",
        L"ами",
        L"еш",
        L"ии",
        L"и",
        L"ией",
        L"ей",
        L"ой",
        L"ий",
        L"й",
        L"иям",
        L"ям",
        L"ием",
        L"ем",
        L"ам",
        L"ом",
        L"о",
        L"у",
        L"ах",
        L"иях",
        L"ях",
        L"ы",
        L"ь",
        L"ию",
        L"ью",
        L"ю",
        L"ия",
        L"ья",
        L"я",
};

static std::vector<std::wstring> Derivational = {
        L"ост",
        L"ость",
};

static std::vector<std::wstring> TidyUp = {
        L"ейш",
        L"ейше",
        L"ейше",
        L"ь",
};

static std::wstring normalize(const std::wstring& word, std::wstring& result) {
    std::transform(word.cbegin(), word.cend(), result.begin(), std::towlower);
    return result;
}

static bool endsWith(const std::wstring& string, const std::wstring& ending) {
    if (string.length() < ending.length()) {
        return false;
    }

    return std::equal(ending.rbegin(), ending.rend(), string.rbegin());
}

static bool try_remove_if_ends(std::wstring &word, const std::wstring &end) {
    if (endsWith(word, end)) {
        word.erase(word.length() - end.length(), end.length());
        return true;
    }
    return false;
}

static bool try_remove_if_ends_any(std::wstring &word, const std::vector<std::wstring>& endings) {
    for (const auto &end: endings) {
        if (try_remove_if_ends(word, end)) {
            return true;
        }
    }

    return false;
}

static bool try_remove_perfect_gerund(std::wstring& word) {
    return try_remove_if_ends_any(word, PerfectGerund);
}

static void maybe_remove_reflective(std::wstring &word){
    try_remove_if_ends_any(word, Reflexive);
}

static bool try_remove_adjectival(std::wstring &word) {

    return  try_remove_if_ends_any(word, Adjective) ||
            try_remove_if_ends_any(word, Participle);
}

static bool try_remove_noun(std::wstring &word) {
    return try_remove_if_ends_any(word, Noun);
}

static bool try_remove_verb(std::wstring &word) {
    return try_remove_if_ends_any(word, Verb);
}

static void maybe_remove_ending_i(std::wstring &word) {
    if (endsWith(word, L"и")) {
        word.erase(word.length() - 1, 1);
    }
}

static std::wstring::size_type get_first_vowel_index(const std::wstring &word) {
    return word.find_first_of(Vowels, 0);
}

static void maybe_remove_derivational(std::wstring &word) {
    try_remove_if_ends_any(word, Derivational);
}

static void tidy_up(std::wstring& word) {
    try_remove_if_ends_any(word, TidyUp);
}

std::wstring isearch::RussianStemmer::stem(const std::wstring &word) {
    /*
     * Используется алгоритм стемминга Snowflake.
     * Описан на странице https://snowballstem.org/algorithms/russian/stemmer.html
     * Реализация построена на основании описанного алгоритма
     */

    // Вначале нормализуем строку и разбиваем ее на 2 части - до (включительно) и после первой гласной
    auto first_vowel_index = get_first_vowel_index(word);
    if (first_vowel_index == std::wstring::npos || first_vowel_index == word.length() - 1) {
        // Гласных нет вообще или только последняя буква гласная - т.е. изменить нельзя
        return word;
    }

    // Так как работа должна вестить только с частью слова после первой гласной, то сразу обрежем и нормализуем
    auto rv = word.substr(first_vowel_index + 1);

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

