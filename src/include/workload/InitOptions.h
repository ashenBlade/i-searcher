//
// Created by ashblade on 05.04.24.
//

#ifndef ISEARCH_INITOPTIONS_H
#define ISEARCH_INITOPTIONS_H

namespace isearch {
    class InitOptions {
    public:
        int parallelism;

        /// @brief Проверить все аргументы на правильность
        void validate() const;

        /// @brief Получить значение параллелизма с учетом автоопределения
        int getParallelism() const;
    };
}

#endif //ISEARCH_INITOPTIONS_H
