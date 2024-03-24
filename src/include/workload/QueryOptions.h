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
        bm25_options bm25;

        /// @brief Проверить аргументы на допустимость значений
        /// @throws std::runtime_error: Найдены ошибки, в сообщении причина
        void validate() const;
    };
}

#endif //ISEARCH_QUERYOPTIONS_H
