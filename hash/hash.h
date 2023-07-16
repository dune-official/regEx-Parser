#ifndef HASH_HASH_H
#define HASH_HASH_H

#include "../regex/regex.h"

typedef struct hash_node {
	struct hash_node *next;
	unsigned long long key;
	regexNode *tree;
	dfa_state *dfaState;
} hash;

extern dfa_state *hash_getDFA(unsigned long long key);
extern regexNode *hash_getRegExTree(unsigned long long key);
extern void hash_insert(unsigned long long key, regexNode *restrict tree, dfa_state *restrict state);

#endif