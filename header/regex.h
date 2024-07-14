#ifndef REGEX_PARSER_REGEX_H
#define REGEX_PARSER_REGEX_H

#include "parser.h"

/* RegEx types */
#define SYMBOL 0x1
#define UNION 0x2
#define KLEENE 0x3
#define CONCAT 0x4

/* RegEx special char */
#define EMPTY 0x80
#define EPSILON 0x81

#define true 1
#define false 0

/* Szudzik-Functions */
#define SZD(x, y) ((x) >= (y) ? ((x) * (x)) + (x) + (y) : (y) * (y) + (x))
#define SMOD(x, y) ((x) >= (y) ? ((x) * (x)) + (x) + (y) : (y) * (y) + (x))

// typedef struct regexMatchResultStruct {
// 	struct regexMatchResultStruct *next;
// 	unsigned int from, to;
// 	char *string;
// } regexMatchResult;

/* regex_node constructors */

regex_node *regex_symbol(unsigned char symbol);

regex_node *regex_union(regex_node *restrict LHS, regex_node *restrict RHS);

regex_node *regex_concat(regex_node *restrict LHS, regex_node *restrict RHS);

regex_node *regex_kleene(regex_node *child);

/* derivation functions */

bool regex_is_nullable(regex_node *rN);

regex_node *regex_derive(regex_node *rN, char a);

/* regex_match functions */
__attribute__((unused)) extern _Bool regex_match(regex_node *restrict pattern, char *restrict string, int matchLen);
// extern regexMatchResult *matchAny(regex_node *pattern, char *string);

/* tree functions */
void regex_print_regexp(const regex_node *root);

regex_node *regex_copy_tree(regex_node *child);
// extern regex_node *parser_parse(char *string);

dfa *regex_pattern_to_dfa(regex_node *pattern);

#endif
