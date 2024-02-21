//
// Created by ashblade on 18.02.24.
//

#ifndef ISEARCH_RUSSIANSTEMMER_H
#define ISEARCH_RUSSIANSTEMMER_H

#include "Stemmer.h"

namespace isearch {
    class RussianStemmer: public Stemmer {
    public:
        std::wstring stem(const std::wstring& word);
    };

}


#endif //ISEARCH_RUSSIANSTEMMER_H
