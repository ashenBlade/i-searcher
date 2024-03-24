//
// Created by ashblade on 20.03.24.
//

#ifndef ISEARCH_INVERSEINDEXSERIALIZER_H
#define ISEARCH_INVERSEINDEXSERIALIZER_H

#include <iostream>

#include "common/InverseIndex.h"

namespace isearch {
    /// @brief Сериализатор обратного индекса
    class InverseIndexSerializer {
    private:
        std::ostream& _stream;
    public:
        explicit InverseIndexSerializer(std::ostream& stream) noexcept;
        void serialize(const InverseIndex& index);
    };
}

#endif //ISEARCH_INVERSEINDEXSERIALIZER_H
