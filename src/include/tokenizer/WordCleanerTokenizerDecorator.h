//
// Created by ashblade on 24.02.24.
//

#ifndef ISEARCH_WORDCLEANERTOKENIZERDECORATOR_H
#define ISEARCH_WORDCLEANERTOKENIZERDECORATOR_H

#include "ITokenizer.h"

namespace isearch {
    class WordCleanerTokenizerDecorator: public ITokenizer {
    private:
        ITokenizer& _tokenizer;
    public:
        explicit WordCleanerTokenizerDecorator(ITokenizer& tokenizer);
        bool tryReadNextWord(std::string &word) override;
    };
}


#endif //ISEARCH_WORDCLEANERTOKENIZERDECORATOR_H
