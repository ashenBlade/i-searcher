//
// Created by ashblade on 13.03.24.
//

#include "serialization/BinaryDocumentDeserializer.h"
#include "serialization/DeserializationHelper.h"
#include <system_error>
#include <cstring>

isearch::BinaryDocumentDeserializer::BinaryDocumentDeserializer(std::istream &input): _input(input) { }

static std::string readTitle(isearch::DeserializationHelper &helper) {
    try {
        return helper.readString();
    } catch (const std::exception &ex) {
        throw std::runtime_error("Ошибка при чтении заголовка документа: " + std::string(ex.what()));
    }
}

isearch::Document isearch::BinaryDocumentDeserializer::deserialize(long id) {
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

    auto title = readTitle(helper);

    auto tokensCount = helper.readLong();
    std::map<std::string, long> contents {};
    for (int i = 0; i < tokensCount; ++i) {
        auto token = helper.readString();
        auto count = helper.readLong();
        contents[token] = count;
    }
    return isearch::Document(id, title, std::move(contents));
}
