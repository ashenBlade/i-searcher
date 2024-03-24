//
// Created by ashblade on 06.03.24.
//

#ifndef ISEARCH_STRINGTOKENIZER_H
#define ISEARCH_STRINGTOKENIZER_H

#include "ITokenizer.h"

namespace isearch {
    /// @brief Токенизатор, разбивающий переданную строку на отдельные токены
    class StringTokenizer: public ITokenizer {
    public:
        explicit StringTokenizer(std::string str);
        bool tryReadNextWord(std::string &word) override;
    private:
        /// @brief Строка, которую мы парсим
        std::string _string;
        /// @brief Текущий индекс чтения
        int _index;
    };
}

#endif //ISEARCH_STRINGTOKENIZER_H
