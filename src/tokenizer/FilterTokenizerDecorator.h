//
// Created by ashblade on 21.02.24.
//

#ifndef ISEARCH_FILTERTOKENIZERDECORATOR_H
#define ISEARCH_FILTERTOKENIZERDECORATOR_H


#include <set>
#include "ITokenizer.h"

namespace isearch {
    /// @brief Декоратор для токенизатора, который удаляет из потока указанные слова, как правило это стоп-слова
    class FilterTokenizerDecorator: public ITokenizer {
    private:
        /// @brief Множество стоп-слов, которые нужно убрать
        std::set<std::string>& _excluded;
        /// @brief Реализация, ответ которой нужно фильтровать
        ITokenizer& _wrapped;
    public:
        explicit FilterTokenizerDecorator(std::set<std::string> &excluded, ITokenizer &wrapped);
        bool tryReadNextWord(std::string &word) override;
    };
}

#endif //ISEARCH_FILTERTOKENIZERDECORATOR_H
