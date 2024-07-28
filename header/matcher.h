#ifndef REGEX_PARSER_MATCHER_H
#define REGEX_PARSER_MATCHER_H

#include "parser.h"

matcher *matcher_match_all(pattern *restrict pattern, const char *restrict string, int str_len);
bool matcher_match_full(pattern *restrict pattern, const char *restrict string, int str_len);
void matcher_print_matches(matcher *restrict matcher_root, const char *restrict string, bool show_indices);
pattern *matcher_get_pattern(char *pattern_string, char pattern_string_len);
char *matcher_get_match(matcher *restrict match_object, char *restrict string);

#endif
