//
// Created by ashblade on 20.03.24.
//

#include "serialization/InverseIndexSerializer.h"
#include "serialization/SerializationHelper.h"


isearch::InverseIndexSerializer::InverseIndexSerializer(std::ostream &stream) noexcept: _stream(stream)
{ }



void isearch::InverseIndexSerializer::serialize(const InverseIndex& index) {
    /*
     * Содержимое файла:
     *
     * | Количество пар | Токен  | Количество документов | Номер документа |
     * |     Int64      | String |       Int64           |      Int64      |
     *
     * Количество пар - кол-во пар "Токен", "Кол-во документов", "Номер документа"
     * Токен - строка, которая содержится в указанных файлах.
     * Количество документов - количество полей "Номер документа" идущих далее.
     * Номер документа - номер документа, в котором содержится указанный файл.
     *
     *
     */

    SerializationHelper helper {_stream};

    // Количество пар
    helper.serialize(index.get_tokens_count());

    // Информация о токенах
    for (const auto &token_info: index) {
        // Название токена
        helper.serialize(token_info.first);
        auto &documents = token_info.second;
        // Если документ пуст (вряд-ли) - то пишем 0 и идем дальше
        if (!documents || documents->empty()) {
            helper.serialize(0);
            continue;
        }

        // Количество документов
        helper.serialize(static_cast<long>(documents->size()));
        // ID документов
        for (const auto &doc: *documents) {
            helper.serialize(doc);
        }
    }
}


