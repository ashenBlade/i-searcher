//
// Created by ashblade on 24.03.24.
//

#ifndef ISEARCH_STOPWORDS_H
#define ISEARCH_STOPWORDS_H

#include <set>
#include <string>

namespace isearch {
    class StopWords {
    public:
        static const std::set<std::string>& russian();
    };
}

#endif //ISEARCH_STOPWORDS_H
