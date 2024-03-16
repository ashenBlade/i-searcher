//
// Created by ashblade on 13.03.24.
//

#include "serialization/BinaryDocumentSerializer.h"
#include "serialization/BinaryDocumentDeserializer.h"

isearch::BinaryDocumentSerializer::BinaryDocumentSerializer(std::ostream &output): _output(output) { }


class BinarySerializer {
private:
    std::ostream& _stream;
public:
    explicit BinarySerializer(std::ostream &stream): _stream(stream) {}
    void write(const std::string &string) {
        // 1. Длина строки
        auto sizeBe = static_cast<int32_t>(htobe32(string.size()));
        _stream.write(reinterpret_cast<char*>(&sizeBe), sizeof(int32_t));

        // 2. Данные строки в байтах
        _stream.write(string.data(), static_cast<int>(string.size()));
    }

    void write(long value) {
        auto be = static_cast<int64_t>(htobe64(value));
        _stream.write(reinterpret_cast<char*>(&be), sizeof(int64_t));
    }
};

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
    BinarySerializer helper {_output};

    // Название документа
    helper.write(document.title());

    // Количество различных слов
    helper.write(static_cast<int64_t>(document.unique_words_count()));

    // Сохраняем частоту каждого слова
    for (const auto &item: document) {
        helper.write(item.first);
        helper.write(item.second);
    }

    _output.flush();
}


