//
// Created by ashblade on 20.03.24.
//

#include "serialization/SerializationHelper.h"

isearch::SerializationHelper::SerializationHelper(std::ostream &stream): _stream(stream) {  }

void isearch::SerializationHelper::serialize(const std::string &value) {
    constexpr int n = sizeof(int32_t);
    auto value_length = static_cast<int32_t>(value.size());

    // 1. Длина строки
    auto sizeBe = htobe32(value_length);
    _stream.write(reinterpret_cast<char*>(&sizeBe), n);

    // 2. Данные строки в байтах
    _stream.write(value.data(), value_length);
}

void isearch::SerializationHelper::serialize(long value) {
    auto be = static_cast<int64_t>(htobe64(value));
    _stream.write(reinterpret_cast<char*>(&be), sizeof(int64_t));
}




