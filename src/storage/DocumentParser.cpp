//
// Created by ashblade on 24.03.24.
//

#include <dirent.h>
#include <cstring>
#include <memory>
#include <fstream>

#include "storage/DocumentParser.h"
#include "tokenizer/StreamTokenizer.h"
#include "tokenizer/WordCleanerTokenizerDecorator.h"
#include "tokenizer/FilterTokenizerDecorator.h"
#include "stemmer/StopWords.h"
#include "stemmer/RussianStemmer.h"

bool isearch::DocumentParser::isRelevantFile(const std::string &filename) {
    constexpr const char* ending = ".txt";
    return 4 <= filename.size() && std::equal(filename.begin() + static_cast<long>(filename.size()) - 4, filename.end(), ending);
}

static isearch::Document parseDocument(long id, const std::string& filename, std::istream& stream) {
    isearch::StreamTokenizer streamTokenizer {stream};
    isearch::WordCleanerTokenizerDecorator cleanerDecorator{streamTokenizer};
    isearch::FilterTokenizerDecorator filterDecorator {isearch::StopWords::russian(), cleanerDecorator};
    isearch::RussianStemmer stemmer {};

    std::map<std::string, long> frequencies {};
    std::string word;
    while (filterDecorator.tryReadNextWord(word)) {
        auto stemmed = stemmer.stem(word);
        ++frequencies[stemmed];
    }

    return isearch::Document(id, filename, std::move(frequencies));
}

std::vector<isearch::Document> isearch::DocumentParser::parseDocuments() const {
    DIR* dir = opendir(_directoryPath.data());
    if (dir == nullptr) {
        throw std::runtime_error("Не удалось открыть директорию для чтения файлов: " + std::string(std::strerror(errno)));
    }

    // Id, которые будут назначаться документам
    long id = 0L;
    std::vector<isearch::Document> documents {};
    dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        if (!isRelevantFile(filename)) {
            continue;
        }

        auto documentPath = _directoryPath + '/' + filename;
        std::ifstream file {documentPath};
        if (!file) {
            std::cerr << "Не удалось открыть файл: " << documentPath << std::endl;
            continue;
        }

        auto document = parseDocument(id, filename, file);
        documents.push_back(std::move(document));
        ++id;
    }

    closedir(dir);
    return documents;
}

isearch::DocumentParser::DocumentParser(std::string directoryPath): _directoryPath(std::move(directoryPath))
{ }
