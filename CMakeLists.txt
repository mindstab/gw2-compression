cmake_minimum_required(VERSION 3.20)
project(gw2-compression LANGUAGES CXX)

SET(INCLUDES
    include/compression/InflateDatFileBuffer.hpp
    include/compression/InflateTextureFileBuffer.hpp
)

SET(SOURCES
    src/compression/HuffmanTree.hpp
    src/compression/HuffmanTreeUtils.cpp
    src/compression/HuffmanTreeUtils.hpp
    src/compression/InflateDatFileBuffer.cpp
    src/compression/InflateTextureFileBuffer.cpp
    src/compression/BitArray.hpp
)

add_library(${PROJECT_NAME} STATIC ${INCLUDES} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

add_library(gw2::compression ALIAS ${PROJECT_NAME})