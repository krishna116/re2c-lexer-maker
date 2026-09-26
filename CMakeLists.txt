cmake_minimum_required(VERSION 4.0)

project(re2c-lexer-maker VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

configure_file(${PROJECT_SOURCE_DIR}/src/template/config.in.text ${PROJECT_SOURCE_DIR}/src/template/config.h)

add_library(LexerMakerLib STATIC
src/LexerConfigParser.cpp
src/CodeGenerator.cpp
)

target_include_directories(LexerMakerLib PUBLIC
src
)

add_subdirectory(tests)