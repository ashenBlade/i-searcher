//
// Created by ashblade on 05.04.24.
//
#include "workload/InitOptions.h"

#include <system_error>
#include <thread>

void isearch::InitOptions::validate() const {
    if (parallelism < 0) {
        throw std::runtime_error("Параллелизм должен быть либо положительным, либо равен 0 для автоматического определения");
    }
}

int isearch::InitOptions::getParallelism() const {
    if (parallelism == 0) {
        return static_cast<int>(std::thread::hardware_concurrency());
    }

    return parallelism;
}

