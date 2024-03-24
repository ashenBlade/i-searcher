//
// Created by ashblade on 06.03.24.
//

#ifndef ISEARCH_LOCALSEARCHENGINE_H
#define ISEARCH_LOCALSEARCHENGINE_H

#include "search_engine/ISearchEngine.h"
#include "storage/IIndexRepository.h"
#include "ranger/IRanger.h"

namespace isearch {
    /// @brief Поисковый движок, использующий индексы локальной файловой системы
    class LocalSearchEngine: public ISearchEngine {
    public:
        explicit LocalSearchEngine(IIndexRepository& repository, isearch::IRanger& ranger);
        LocalSearchEngine(LocalSearchEngine&& other) = delete;
        LocalSearchEngine(const LocalSearchEngine& other) = delete;

        std::vector<std::string> search(const std::string &query, int max) override;
        ~LocalSearchEngine() override;
    private:
        IIndexRepository& _repository;
        IRanger& _ranger;
    };
}


#endif //ISEARCH_LOCALSEARCHENGINE_H
