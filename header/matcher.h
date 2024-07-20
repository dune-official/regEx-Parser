#ifndef REGEX_PARSER_MATCHER_H
#define REGEX_PARSER_MATCHER_H

#include "parser.h"

matcher *matcher_match_all(dfa *restrict dfa, const char *restrict string, int str_len);
bool matcher_match_full(dfa *restrict dfa, const char *restrict string, int str_len);
void matcher_print_matches(matcher *restrict matcher_root, const char *restrict string, bool show_indices);

#endif
