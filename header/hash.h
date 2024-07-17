#ifndef HASHMAP_HASHMAP_H
#define HASHMAP_HASHMAP_H

#include "parser.h"


struct hash_root *hash_initialize(unsigned int bucket_size);
bool hash_insert(struct hash_root *restrict hashmap, unsigned int key, regex_node *restrict tree, dfa_state *restrict state);
regex_node *hash_get_tree(struct hash_root *restrict hashmap, unsigned int key);
dfa_state *hash_get_dfa(struct hash_root *restrict hashmap, unsigned int key);
bool hash_exists(struct hash_root *restrict hashmap, unsigned int key);
void internal_hash_delete(struct hash_node *restrict node_before);
void *hash_delete(struct hash_root *restrict hashmap, unsigned int key);
void hash_set_hashfunction(struct hash_root *restrict hashmap, unsigned int (*hashfktn)(unsigned int, unsigned int));

#endif