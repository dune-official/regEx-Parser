#ifndef HASH_HASH_H
#define HASH_HASH_H

#include "parser.h"

dfa_state *hash_getDFA(unsigned long long key);
regex_node *hash_getRegExTree(unsigned long long key);
void hash_insert(unsigned long long key, regex_node *restrict tree, dfa_state *restrict state);

#endif