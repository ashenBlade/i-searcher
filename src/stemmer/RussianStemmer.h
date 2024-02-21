//
// Created by ashblade on 18.02.24.
//

#ifndef ISEARCH_RUSSIANSTEMMER_H
#define ISEARCH_RUSSIANSTEMMER_H

#include "IStemmer.h"

namespace isearch {
    class RussianStemmer: public IStemmer {
    public:
        std::wstring stem(const std::wstring& word) override;
    };

}


#endif //ISEARCH_RUSSIANSTEMMER_H
