//
// Created by ashblade on 21.02.24.
//

#ifndef ISEARCH_STREAMTOKENIZER_H
#define ISEARCH_STREAMTOKENIZER_H

#include "ITokenizer.h"
#include "iostream"

namespace isearch {
    class StreamTokenizer: public ITokenizer {
    private:
        std::wistream& _stream;
        bool _end;
    public:
        explicit StreamTokenizer(std::wistream &stream) noexcept;
        bool tryReadNextWord(std::wstring &word) override;

        StreamTokenizer() = delete;
        ~StreamTokenizer() override;
    };
}


#endif //ISEARCH_STREAMTOKENIZER_H
