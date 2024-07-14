#ifndef HASH_HASH_H
#define HASH_HASH_H

#include "parser.h"

dfa_state *hash_get_dfa(unsigned long long key);
regex_node *hash_get_regex_tree(unsigned long long key);
void hash_insert(unsigned long long key, regex_node *restrict tree, dfa_state *restrict state);

#endif