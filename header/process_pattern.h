#ifndef REGEX_PARSER_PROCESS_PATTERN_H
#define REGEX_PARSER_PROCESS_PATTERN_H

// #include "regex.h"
// #include "seekable.h"
#include "parser.h"

#define BACKSLASH 0x6
#define INTEGER 0x07

enum Precedence {
	PR_LOWEST,
	PR_UNION,
	PR_CONCAT,
	PR_QUANT,
	PR_KLEENE,
    PR_SETRANGE,
};


seek *lexer_tokenize(const char *input_string, char length);
regex_node *parser_parse(seek *tokenstream, char precedence);

#endif
