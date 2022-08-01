cmake_minimum_required(VERSION 3.16)

project(lpl VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(lplc
    main.cpp
    lexer.cpp
    parser.cpp
)

set_property(TARGET lplc PROPERTY CXX_STANDARD 20)

if(MSVC)
  target_compile_options(lplc PRIVATE /W4 /WX)
else()
  target_compile_options(lplc PRIVATE -Wall -Werror -Wextra -Wpedantic -pedantic-errors)
endif()
