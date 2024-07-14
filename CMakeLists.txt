cmake_minimum_required(VERSION 3.25)
project(regEx_parser C)

set(CMAKE_C_STANDARD 99)
add_compile_options(-Werror -O2)

add_executable(regEx_parser
        main.c

        header/parser.h
        header/data_structures.h
        header/alloc_s.h
        header/DFA.h
        header/hash.h
        header/processPattern.h
        header/queue.h
        header/regex.h
        header/seekable.h

        src/dfa/dfa.c
        src/hash/hash.c
        src/parser/prattParser.c
        src/parser/inputLexer.c
        src/queue/queue.c
        src/regex/regEx.c
        src/regex/regExHelper.c
        src/seekable/seekable.c
)
