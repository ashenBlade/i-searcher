//
// Created by ashblade on 13.03.24.
//

#include "serialization/BinaryDocumentDeserializer.h"
#include <system_error>
#include <cstring>

isearch::BinaryDocumentDeserializer::BinaryDocumentDeserializer(std::istream &input): _input(input) { }

class DeserializationHelper {
private:
    std::istream &_input;
public:
    explicit DeserializationHelper(std::istream  &input): _input(input) { }
    long readLong() {
        char buffer[sizeof(int64_t)];
        _input.read(buffer, sizeof(int64_t));
        if (!_input) {
            throw std::runtime_error("Ошибка при чтении long");
        }
        return be64toh(*reinterpret_cast<long*>(buffer));
    }

    int readInt() {
        char sizeBuffer[sizeof(int32_t)];
        _input.read(sizeBuffer, sizeof(int32_t));
        if (!_input) {
            throw std::runtime_error("Ошибка при чтении размера строки");
        }
        return be32toh(*reinterpret_cast<int*>(sizeBuffer));
    }

    std::string readString() {
        int stringLength = readInt();
        constexpr int bufferSize = 128;
        char buffer[bufferSize];
        int left = stringLength;
        std::string result {};
        while (0 < left) {
            int toRead = std::min(left, bufferSize);
            _input.read(buffer, toRead);
            if (!_input) {
                throw std::runtime_error("Ошибка при чтении строки");
            }
            result.append(buffer, toRead);
            left -= toRead;
        }

        return result;
    }
};

isearch::Document isearch::BinaryDocumentDeserializer::deserialize() {
    /*
     * Формат документа:
     *
     * |  Название документа  | Общее количество элементов |  Токен   | Количество токенов |
     * |       String         |           int64            |  String  |        int64       |
     *
     * Поля:
     * - Название документа - название исходного документа, которому принадлежат данные,
     * - Общее количество элементов - количество пар токен-количество, используется для дальнейшего парсинга данных
     * - Токен - строка, представляющая токен
     * - Количество токенов - сколько раз этот токен встречается частота
     *
     * Типы:
     * - int32 - 4 байтное знаковое целое
     * - String - строка, состоящая из 2 частей: длина строки в байтах (int32) и массив байтов (длина массива указана в "длина строки")
     *
     * Все данные сериализуются в big-endian
     */

    DeserializationHelper helper {_input};
    auto title = helper.readString();
    auto tokensCount = helper.readLong();
    std::map<std::string, long> contents {};
    for (int i = 0; i < tokensCount; ++i) {
        auto token = helper.readString();
        auto count = helper.readLong();
        contents[token] = count;
    }
    return isearch::Document(title, std::move(contents));
}