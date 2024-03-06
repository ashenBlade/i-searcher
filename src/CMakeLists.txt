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
        tokenizer/StringTokenizer.cpp
        tokenizer/FilterTokenizerDecorator.cpp
        tokenizer/WordCleanerTokenizerDecorator.cpp

        common/Document.cpp
        common/DocumentCollection.cpp

        search_engine/LocalSearchEngine.cpp

        ranger/BM25Ranger.cpp
)

add_executable(${PROJECT_NAME} ${sources})

target_sources(${PROJECT_NAME} PRIVATE ${sources})
target_include_directories(${PROJECT_NAME} PUBLIC include)


