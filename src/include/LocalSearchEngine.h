//
// Created by ashblade on 06.03.24.
//

#ifndef ISEARCH_LOCALSEARCHENGINE_H
#define ISEARCH_LOCALSEARCHENGINE_H

#include "ISearchEngine.h"

namespace isearch {
    class LocalSearchEngine: public ISearchEngine {
    public:
        explicit LocalSearchEngine(DocumentCollection documents);
        std::vector<std::string> search(const std::string &query, int max) override;
    private:
        DocumentCollection _documents;
    };
}


#endif //ISEARCH_LOCALSEARCHENGINE_H
