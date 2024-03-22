//
// Created by ashblade on 20.03.24.
//

#include "serialization/SerializationHelper.h"

isearch::SerializationHelper::SerializationHelper(std::ostream &stream): _stream(stream) {  }

void isearch::SerializationHelper::serialize(const std::string &value) {
    // 1. Длина строки
    auto sizeBe = static_cast<int32_t>(htobe32(value.size()));
    _stream.write(reinterpret_cast<char*>(&sizeBe), sizeof(int32_t));

    // 2. Данные строки в байтах
    _stream.write(value.data(), static_cast<int>(value.size()));
}

void isearch::SerializationHelper::serialize(long value) {
    auto be = static_cast<int64_t>(htobe64(value));
    _stream.write(reinterpret_cast<char*>(&be), sizeof(int64_t));
}




