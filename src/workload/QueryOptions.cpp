//
// Created by ashblade on 24.03.24.
//

#include <stdexcept>
#include <thread>

#include "workload/QueryOptions.h"

void isearch::QueryOptions::bm25_options::validate() const {
    if (b < 0 || 1 < b) {
        throw std::runtime_error("BM25: Коэффициент b должен быть в диапазоне от 0 до 1");
    }

    if (k < 0) {
        throw std::runtime_error("BM25: Коэффициент k не может быть отрицательным");
    }
}

void isearch::QueryOptions::validate() const {
    if (max < 0) {
        throw std::runtime_error("Максимальное количество файлов в выводе не может быть отрицательным");
    }

    if (parallelism < 1 && parallelism != shouldInferParallelism) {
        throw std::runtime_error("Параллелизм может быть либо положительным, либо равен " + std::to_string(shouldInferParallelism) + " для автоматического определения");
    }

    bm25.validate();
}

int isearch::QueryOptions::getParallelism() const {
    if (parallelism == shouldInferParallelism) {
        return static_cast<int>(std::thread::hardware_concurrency());
    }

    return parallelism;
}
