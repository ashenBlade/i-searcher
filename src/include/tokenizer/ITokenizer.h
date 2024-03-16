//
// Created by ashblade on 21.02.24.
//

#ifndef ISEARCH_TOKENIZER_H
#define ISEARCH_TOKENIZER_H

#include <string>

namespace isearch {
    class ITokenizer {
    public:
        /// @brief Прочитать очередное слово
        /// @param word - Прочитанное слово, если успешно
        /// @returns true - слово прочитано, false - достигнут конец и слов больше нет
        /// @remarks В @param word Будет записано значение прочитанного слова, если было, в противном случае, значение не определено.
        virtual bool tryReadNextWord(std::string& word) = 0;

        virtual ~ITokenizer() = default;
    };
}

#endif //ISEARCH_TOKENIZER_H
