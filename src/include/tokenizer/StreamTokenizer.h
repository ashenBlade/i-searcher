//
// Created by ashblade on 21.02.24.
//

#ifndef ISEARCH_STREAMTOKENIZER_H
#define ISEARCH_STREAMTOKENIZER_H

#include "ITokenizer.h"
#include "iostream"

namespace isearch {
    /// @brief Токенизатор, читающий слова из входного потока до достижения его конца
    class StreamTokenizer: public ITokenizer {
    private:
        std::istream& _stream;
        bool _end;
    public:
        explicit StreamTokenizer(std::istream &stream) noexcept;
        bool tryReadNextWord(std::string &word) override;

        StreamTokenizer() = delete;
        ~StreamTokenizer() override;
    };
}


#endif //ISEARCH_STREAMTOKENIZER_H
