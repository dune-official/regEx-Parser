cmake_minimum_required(VERSION 3.25)
project(regEx_parser C)

set(CMAKE_C_STANDARD 99)
add_compile_options(-Werror -O2)

add_executable(regEx_parser main.c regex/regex.h regex/regEx.c regex/regExHelper.c stack/stack.c stack/stack.h hash/hash.c hash/hash.h dfa/DFA.c queue/queue.c queue/queue.h parser/inputLexer.c parser/processPattern.h parser/prattParser.c seekable/seekable.c seekable/seekable.h)
