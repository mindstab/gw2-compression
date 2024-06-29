cmake_minimum_required(VERSION 3.20)
project(gw2-compression LANGUAGES CXX)

SET(INCLUDES
    include/gw2DatTools/compression/inflateDatFileBuffer.h
    include/gw2DatTools/compression/inflateTextureFileBuffer.h
    include/gw2DatTools/exception/Exception.h
)

SET(SOURCES
    src/gw2DatTools/compression/HuffmanTree.h
    src/gw2DatTools/compression/HuffmanTree.i
    src/gw2DatTools/compression/huffmanTreeUtils.cpp
    src/gw2DatTools/compression/huffmanTreeUtils.h
    src/gw2DatTools/compression/inflateDatFileBuffer.cpp
    src/gw2DatTools/compression/inflateTextureFileBuffer.cpp
    src/gw2DatTools/exception/Exception.cpp
    src/gw2DatTools/utils/BitArray.h
    src/gw2DatTools/utils/BitArray.i
)

add_library(${PROJECT_NAME} STATIC ${INCLUDES} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

add_library(gw2::compression ALIAS ${PROJECT_NAME})