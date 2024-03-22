//
// Created by ashblade on 21.03.24.
//

#include "serialization/InverseIndexDeserializer.h"
#include "serialization/DeserializationHelper.h"

inline static void checkNonNegativeCount(long count) {
    if (count < 0) {
        throw std::runtime_error("Количество токенов не может быть отрицательным. Прочитано значение: " + std::to_string(count));
    }
}

isearch::InverseIndex isearch::InverseIndexDeserializer::deserialize(std::istream &stream) {
    isearch::DeserializationHelper helper {stream};
    // Название файла
    auto filename = helper.readString();

    // Сам обратный индекс
    auto tokensCount = helper.readLong();

    std::map<std::string, std::shared_ptr<std::vector<long>>> tokenToDocuments {};
    for (int i = 0; i < tokensCount; ++i) {
        auto token = helper.readString();
        auto documentsCount = helper.readLong();
        checkNonNegativeCount(documentsCount);

        auto documents = std::make_shared<std::vector<long>>();
        if (documentsCount == 0) {
            tokenToDocuments[token] = documents;
            continue;
        }
        documents->reserve(documentsCount);

        for (int i = 0; i < documentsCount; ++i) {
            documents->push_back(helper.readLong());
        }

        tokenToDocuments[token] = documents;
    }

    return isearch::InverseIndex(std::move(tokenToDocuments));
}
