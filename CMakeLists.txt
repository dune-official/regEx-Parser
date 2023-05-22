cmake_minimum_required(VERSION 3.25)
project(regEx_parser C)

set(CMAKE_C_STANDARD 99)
add_compile_options(-Werror -O2)

add_executable(regEx_parser main.c regex.h regEx.c regExHelper.c inputParser.c stack.c stack.h)
