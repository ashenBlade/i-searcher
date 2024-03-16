//
// Created by ashblade on 13.03.24.
//

#ifndef ISEARCH_BINARYDOCUMENTDESERIALIZER_H
#define ISEARCH_BINARYDOCUMENTDESERIALIZER_H

#include <iostream>
#include "serialization/IDocumentDeserializer.h"

namespace isearch {
    /// @brief Десериализатор из собственного формата
    class BinaryDocumentDeserializer: public IDocumentDeserializer {
    private:
        std::istream& _input;
    public:
        explicit BinaryDocumentDeserializer(std::istream &input);
        Document deserialize() override;
    };
}

#endif //ISEARCH_BINARYDOCUMENTDESERIALIZER_H
