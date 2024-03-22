//
// Created by ashblade on 20.03.24.
//

#ifndef ISEARCH_DESERIALIZATION_HELPER_H
#define ISEARCH_DESERIALIZATION_HELPER_H

#include <iostream>

namespace isearch {
    class DeserializationHelper {
    private:
        std::istream& _stream;
    public:
        explicit DeserializationHelper(std::istream &stream);
        std::string readString();
        long readLong();
        int readInt();
    };
}

#endif //ISEARCH_DESERIALIZATION_HELPER_H
