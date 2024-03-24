//
// Created by ashblade on 13.03.24.
//

#ifndef ISEARCH_BINARYDOCUMENTDESERIALIZER_H
#define ISEARCH_BINARYDOCUMENTDESERIALIZER_H

#include <iostream>

#include "common/Document.h"

namespace isearch {
    /// @brief Десериализатор документа из собственного бинарного формата
    class BinaryDocumentDeserializer {
    private:
        std::istream& _input;
    public:
        explicit BinaryDocumentDeserializer(std::istream &input);
        isearch::Document deserialize(long id);
    };
}

#endif //ISEARCH_BINARYDOCUMENTDESERIALIZER_H
