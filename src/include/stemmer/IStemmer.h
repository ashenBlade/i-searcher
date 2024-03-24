//
// Created by ashblade on 18.02.24.
//

#ifndef ISEARCH_ISTEMMER_H
#define ISEARCH_ISTEMMER_H

#include <string>

namespace isearch {
    /// @brief Алгоритм стемминга слова
    class IStemmer {
    public:
        /// @brief Привести переданное слово к каноническому виду (базовая форма)
        /// @param word - Слово, которое нужно привести к каноническому виду
        /// @returns Слово в каконическом виде
        virtual std::string stem(const std::string& word) = 0;
    };
}

#endif //ISEARCH_ISTEMMER_H
