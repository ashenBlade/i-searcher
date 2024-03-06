//
// Created by ashblade on 06.03.24.
//

#ifndef ISEARCH_BM25RANGER_H
#define ISEARCH_BM25RANGER_H

#include "IRanger.h"

namespace isearch {
    /// @brief Ранжировщик, использующий алгоритм BM25 - https://ru.wikipedia.org/wiki/Okapi_BM25
    class BM25Ranger: public IRanger {
    public:
        BM25Ranger(double k, double b);
        std::vector<std::string> range(const std::vector<std::string> &search_vector, isearch::DocumentCollection &documents, int max) override;
    private:
        /// @brief Коэффициент насыщенности
        double _k;

        /// @brief Коэффициент влияния средней длины
        double _b;
    };
}

#endif //ISEARCH_BM25RANGER_H
