cmake_minimum_required(VERSION 3.15)
project(kuo VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include)

add_executable(kuo
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/codegen.cpp
)

install(TARGETS kuo DESTINATION bin)

enable_testing()

add_test(
    NAME hello_world
    COMMAND kuo ${CMAKE_SOURCE_DIR}/examples/hello.kuo
)

add_test(
    NAME fibonacci
    COMMAND kuo ${CMAKE_SOURCE_DIR}/examples/fibonacci.kuo
)

add_test(
    NAME functions
    COMMAND kuo ${CMAKE_SOURCE_DIR}/examples/functions.kuo
)
