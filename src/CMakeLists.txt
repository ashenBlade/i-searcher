CMAKE_MINIMUM_REQUIRED(VERSION 3.22)

project(
        isearch
        VERSION 1.0
        LANGUAGES CXX
)

FIND_PACKAGE(Boost 1.40 REQUIRED COMPONENTS program_options)

SET(sources
        main.cpp

        stemmer/RussianStemmer.cpp
        stemmer/StopWords.cpp

        storage/FileSystemIndexRepository.cpp
        storage/DocumentParser.cpp

        tokenizer/StreamTokenizer.cpp
        tokenizer/StringTokenizer.cpp
        tokenizer/FilterTokenizerDecorator.cpp
        tokenizer/WordCleanerTokenizerDecorator.cpp

        common/Document.cpp
        common/DocumentCollection.cpp
        common/InverseIndex.cpp

        workload/ISearchApplication.cpp
        workload/QueryOptions.cpp
        workload/InitOptions.cpp

        search_engine/LocalSearchEngine.cpp

        ranger/BM25Ranger.cpp

        serialization/BinaryDocumentDeserializer.cpp
        serialization/BinaryDocumentSerializer.cpp
        serialization/InverseIndexSerializer.cpp
        serialization/InverseIndexDeserializer.cpp
        serialization/SerializationHelper.cpp
        serialization/DeserializationHelper.cpp
)

add_executable(${PROJECT_NAME} ${sources})

target_sources(${PROJECT_NAME} PRIVATE ${sources})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${Boost_LIBRARIES})


