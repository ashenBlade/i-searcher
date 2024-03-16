//
// Created by ashblade on 13.03.24.
//

#ifndef ISEARCH_IINDEXSERIALIZER_H
#define ISEARCH_IINDEXSERIALIZER_H

#include "common/Document.h"

namespace isearch {
    class IDocumentSerializer {
    public:
        virtual void serialize(const Document& document) = 0;
    };
}

#endif //ISEARCH_IINDEXSERIALIZER_H
