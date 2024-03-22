//
// Created by ashblade on 20.03.24.
//

#ifndef ISEARCH_SERIALIZATIONHELPER_H
#define ISEARCH_SERIALIZATIONHELPER_H

#include <iostream>

namespace isearch {
    class SerializationHelper {
    private:
        std::ostream& _stream;
    public:
        explicit SerializationHelper(std::ostream& stream);
        void serialize(const std::string& value);
        void serialize(long value);
    };
}

#endif //ISEARCH_SERIALIZATIONHELPER_H
