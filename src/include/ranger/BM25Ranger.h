//
// Created by ashblade on 06.03.24.
//

#ifndef ISEARCH_BM25RANGER_H
#define ISEARCH_BM25RANGER_H

#include "IRanger.h"

namespace isearch {
    /// @brief Алгоритм ранжирования BM25
    /// @author https://ru.wikipedia.org/wiki/Okapi_BM25
    class BM25Ranger: public IRanger {
    private:
        /// @brief Коэффициент насыщенности
        double _k;

        /// @brief Коэффициент влияния средней длины
        double _b;

        /// @brief Степень параллелизма, которую необходимо использовать
        int _parallelism;
    public:
        BM25Ranger(double k, double b, int parallelism);
        std::vector<std::string> range(const std::vector<std::string> &search_vector, isearch::DocumentCollection &documents, int max) override;
    };
}

#endif //ISEARCH_BM25RANGER_H
