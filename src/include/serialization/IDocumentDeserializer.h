//
// Created by ashblade on 13.03.24.
//

#ifndef ISEARCH_IDOCUMENTDESERIALIZER_H
#define ISEARCH_IDOCUMENTDESERIALIZER_H

#include "common/Document.h"

namespace isearch {
    class IDocumentDeserializer {
    public:
        virtual Document deserialize() = 0;
    };
}

#endif //ISEARCH_IDOCUMENTDESERIALIZER_H
