//
// Created by ashblade on 18.02.24.
//

#ifndef ISEARCH_STEMMER_H
#define ISEARCH_STEMMER_H

#include <string>

namespace isearch {
    class Stemmer {
    public:
        virtual std::wstring stem(const std::wstring& word) = 0;
    };
}

#endif //ISEARCH_STEMMER_H
