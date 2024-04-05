//
// Created by ashblade on 24.03.24.
//

#ifndef ISEARCH_QUERYOPTIONS_H
#define ISEARCH_QUERYOPTIONS_H

namespace isearch {
    struct QueryOptions {
    public:
        struct bm25_options {
            double k = 2;
            double b = 0.75;
            void validate() const;
        };

        int max;
        int parallelism;
        bm25_options bm25;

        /// @brief Проверить аргументы на допустимость значений
        /// @throws std::runtime_error: Найдены ошибки, в сообщении причина
        void validate() const;

        /// @brief Получить значение параллелизма, с учетом вывода этого значения
        int getParallelism() const;

        /// @brief Значение, означающее, что параллелизм необходимо вывести самим
        static constexpr int shouldInferParallelism = 0;
    };
}

#endif //ISEARCH_QUERYOPTIONS_H
