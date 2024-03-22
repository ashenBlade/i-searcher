//
// Created by ashblade on 13.03.24.
//

#include "serialization/BinaryDocumentSerializer.h"
#include "serialization/SerializationHelper.h"

isearch::BinaryDocumentSerializer::BinaryDocumentSerializer(std::ostream &output): _output(output) { }

void isearch::BinaryDocumentSerializer::serialize(const isearch::Document &document) {
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
    SerializationHelper helper {_output};

    // Название документа
    helper.serialize(document.title());

    // Количество различных слов
    helper.serialize(static_cast<int64_t>(document.tokens_count()));

    // Сохраняем частоту каждого слова
    for (const auto &item: document) {
        helper.serialize(item.first);
        helper.serialize(item.second);
    }

    _output.flush();
}


