CMAKE_MINIMUM_REQUIRED(VERSION 3.22)

project(
        isearch
        VERSION 1.0
        LANGUAGES CXX
)

SET(sources
        main.cpp
        stemmer/RussianStemmer.cpp
        tokenizer/StreamTokenizer.cpp
        tokenizer/FilterTokenizerDecorator.cpp
        tokenizer/WordCleanerTokenizerDecorator.cpp
)

add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME} PRIVATE ${sources})
target_include_directories(${PROJECT_NAME} PUBLIC include)


