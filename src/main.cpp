//
// Created by ashblade on 18.02.24.
//
#include <iostream>
#include <map>
#include <fstream>

#include "stemmer/RussianStemmer.h"
#include "common/DocumentCollection.h"
#include "serialization/BinaryDocumentSerializer.h"
#include "serialization/BinaryDocumentDeserializer.h"

static void setupEnvironment();

int main(int argc, char** argv) {
    setupEnvironment();
    const std::string filename = "file.index";

//    isearch::Document document {
//        "hello, world",
//        std::map<std::string, long> {
//                {"hello", 123},
//                {"word", 344}
//        }
//    };
//
//    std::fstream file(filename, std::ios::out);
//    if (!file) {
//        std::cout << "Ошибка открытия файла";
//        return 1;
//    }
//
//    isearch::BinaryDocumentSerializer serializer(file);
//    try {
//        serializer.serialize(document);
//    } catch (const std::exception& ex) {
//        std::cout << "Ошибка во время сериализации документа в файл: " << ex.what() << std::endl;
//        return 2;
//    }
//
//    std::cout << "Файл сериализован успешно";

    std::fstream file(filename, std::ios::in);
    if (!file) {
        std::cout << "Ошибка открытия файла";
        return 1;
    }

    isearch::BinaryDocumentDeserializer deserializer(file);

    try {
        auto document = deserializer.deserialize();
        std::cout << "Название документа: " << document.title() << std::endl;
        std::cout << "Количество строк: " << document.size() << std::endl;
        for (const auto &pair: document) {
            std::cout << pair.first << ": " << pair.second << std::endl;
        }
    } catch (const std::exception &ex) {
        std::cout << "Ошибка при десериализации: " << ex.what() << std::endl;
        return 2;
    }

    return 0;
}

void setupEnvironment() {

    // Чтобы мог считывать как русские так и английские символы из консоли
    setlocale(LC_ALL, "");
}
