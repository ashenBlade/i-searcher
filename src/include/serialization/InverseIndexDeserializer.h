//
// Created by ashblade on 21.03.24.
//

#ifndef ISEARCH_INVERSEINDEXDESERIALIZER_H
#define ISEARCH_INVERSEINDEXDESERIALIZER_H

#include "common/InverseIndex.h"

namespace isearch {
    class InverseIndexDeserializer {
    public:
        InverseIndex deserialize(std::istream &stream);
    };
}

#endif //ISEARCH_INVERSEINDEXDESERIALIZER_H
