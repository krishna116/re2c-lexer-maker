#pragma once

//===================================================================
// Cmake Code Template
//===================================================================
constexpr const char* CmakeCodeTemplate = R"~(cmake_minimum_required(VERSION 4.0)

project(@ClassName@Lib VERSION 0.1.0 LANGUAGES C CXX)

set(LexerCppFileName "${CMAKE_CURRENT_SOURCE_DIR}/@ClassName@.cpp")  # Please customize location.
set(LexerLexFileName "${CMAKE_CURRENT_SOURCE_DIR}/@ClassName@.l")    # Please customize location.
set(LexerLibraryName "@ClassName@Lib")

add_library(${LexerLibraryName} STATIC
${LexerCppFileName}
)

target_include_directories(${LexerLibraryName} PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(${LexerLibraryName} PUBLIC 
cxx_std_17
)

find_program(RE2C_EXECUTABLE
NAMES re2c re2c.exe
)

if(RE2C_EXECUTABLE)
  message("find re2c = ${RE2C_EXECUTABLE}")
  add_custom_command(
    OUTPUT ${LexerCppFileName}
    COMMAND ${RE2C_EXECUTABLE} "${LexerLexFileName}" -o "${LexerCppFileName}"
    DEPENDS "${LexerLexFileName}"
    COMMENT "${LexerCppFileName}"
    VERBATIM
  )
  add_custom_target(
    generate_MyLexer_cpp
    DEPENDS "${LexerCppFileName}"
  )
  add_dependencies(${LexerLibraryName} generate_MyLexer_cpp)
else()
  message(FATAL_ERROR "Cannot find programm re2c.")
endif()

)~";