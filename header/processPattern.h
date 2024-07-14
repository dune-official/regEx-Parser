#ifndef REGEX_PARSER_PROCESSPATTERN_H
#define REGEX_PARSER_PROCESSPATTERN_H

// #include "regex.h"
// #include "seekable.h"
#include "parser.h"

#define BACKSLASH 0x6

enum Precedence {
	PR_LOWEST,
	PR_UNION,
	PR_CONCAT,
	PR_QUANT,
	PR_KLEENE,
    PR_SETRANGE
};


seek *lexer_tokenize(const char *input_string, char length);

regex_node *parser_parse(seek *tokenstream, char precedence);

void parser_advance(seek *tokenstream);

regex_node *parser_parse_group(seek *tokenstream);

regex_node *parser_parse_escaped(char escaped);

regex_node *parser_parse_concat(regex_node *restrict LHS, seek *restrict tokenstream);

regex_node *parser_parse_union(regex_node *restrict LHS, seek *restrict tokenstream);

regex_node *parser_parse_set(seek *restrict tokenstream, char precedence);

regex_node *parser_balanced_tree(char from, char to);

#endif
