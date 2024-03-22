//
// Created by ashblade on 13.03.24.
//

#ifndef ISEARCH_STREAMDOCUMENTSERIALIZER_H
#define ISEARCH_STREAMDOCUMENTSERIALIZER_H

#include <iostream>
#include "common/Document.h"

namespace isearch {
    /// @brief Сериализатор, который сохраняет документ в бинарном представлении
    class BinaryDocumentSerializer {
    private:
        /// @brief Поток, в который необходимо производить сериализацию
        std::ostream& _output;
    public:
        explicit BinaryDocumentSerializer(std::ostream &output);
        void serialize(const isearch::Document &document);
    };
}

#endif //ISEARCH_STREAMDOCUMENTSERIALIZER_H
