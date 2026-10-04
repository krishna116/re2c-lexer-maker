cmake_minimum_required(VERSION 4.0)

# Windows build command:
# cmake -S . -B build  -DCMAKE_TOOLCHAIN_FILE="D:/vcpkg/scripts/buildsystems/vcpkg.cmake"
# cmake --build build --Config Release
# cpack --config build/CPackConfig.cmake

project(re2c-lexer-maker VERSION 2.1.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

if(MSVC)
add_compile_options(/utf-8)
endif()

configure_file(${PROJECT_SOURCE_DIR}/src/template/config.in.text ${PROJECT_SOURCE_DIR}/src/template/config.h)

find_package(libuv CONFIG REQUIRED)
add_library(Re2cLib
src/Re2c.cpp
)
target_link_libraries(Re2cLib PRIVATE 
libuv::uv
)
target_include_directories(Re2cLib PUBLIC
src
)

add_library(LexerMakerLib STATIC
src/LexerConfigParser.cpp
src/CodeGenerator.cpp
)

target_include_directories(LexerMakerLib PUBLIC
src
)

add_subdirectory(tests)

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR ZIP)
include(CPack)
