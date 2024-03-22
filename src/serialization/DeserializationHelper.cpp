//
// Created by ashblade on 20.03.24.
//

#include "serialization/DeserializationHelper.h"

isearch::DeserializationHelper::DeserializationHelper(std::istream &stream): _stream(stream) {

}

std::string isearch::DeserializationHelper::readString() {
    int stringLength = readInt();
    constexpr int bufferSize = 128;
    char buffer[bufferSize];
    int left = stringLength;
    std::string result {};
    while (0 < left) {
        int toRead = std::min(left, bufferSize);
        _stream.read(buffer, toRead);
        if (!_stream) {
            throw std::runtime_error("Ошибка при чтении строки");
        }
        result.append(buffer, toRead);
        left -= toRead;
    }

    return result;
}

int isearch::DeserializationHelper::readInt() {
    char sizeBuffer[sizeof(int32_t)];
    _stream.read(sizeBuffer, sizeof(int32_t));
    if (!_stream) {
        throw std::runtime_error("Ошибка при чтении размера строки");
    }
    return be32toh(*reinterpret_cast<int*>(sizeBuffer));
}

long isearch::DeserializationHelper::readLong() {
    char buffer[sizeof(int64_t)];
    _stream.read(buffer, sizeof(int64_t));
    if (!_stream) {
        throw std::runtime_error("Ошибка при чтении long");
    }
    return be64toh(*reinterpret_cast<long*>(buffer));
}
