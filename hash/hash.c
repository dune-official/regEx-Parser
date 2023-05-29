#include "hash.h"

/* Feel free to edit the size according to your needs */
#define MAXSIZE 769

hash *h_array[MAXSIZE];

int hash_function(unsigned long long value) {
	return value % MAXSIZE;
}

DFA *hash_getDFA(unsigned long long key) {
	int hash_value = hash_function(key);
	if (NULL == h_array[hash_value]) {
		return NULL;
	} else {
		/* traverse through the chain */
		hash *ptr = h_array[hash_value];
		while (NULL != ptr) {
			if (ptr->key == key) {
				return ptr->dfaState;
			}
			ptr = ptr->next;
		}
		return NULL;
	}
}

regexNode *hash_getRegExTree(unsigned long long key) {
	int hash_value = hash_function(key);
	if (NULL == h_array[hash_value]) {
		return NULL;
	} else {
		/* traverse through the chain */
		hash *ptr = h_array[hash_value];
		while (NULL != ptr) {
			if (ptr->key == key) {
				return ptr->tree;
			}
			ptr = ptr->next;
		}
		return NULL;
	}
}

void hash_insert(unsigned long long key, regexNode *restrict tree, DFA *restrict state) {
	int hash_value = hash_function(key);
	if (NULL == h_array[hash_value]) {
		h_array[hash_value] = (hash *) calloc(1, sizeof(*h_array[hash_value]));
		if (NULL == h_array[hash_value]) {
			fputs("Failed to allocate bucked", stderr);
			exit(-1);
		}
		h_array[hash_value]->key = key;
		h_array[hash_value]->tree = tree;
		h_array[hash_value]->dfaState = state;
		h_array[hash_value]->next = NULL;
	} else {
		hash *ptr = h_array[hash_value];
		if (ptr->key == key) {
			h_array[hash_value]->tree = tree;
			h_array[hash_value]->dfaState = state;
			return;
		}
		while (NULL != ptr->next) {
			if (ptr->next->key == key) {
				h_array[hash_value]->tree = tree;
				h_array[hash_value]->dfaState = state;
				return;
			}
			ptr = ptr->next;
		}
		ptr->next = (hash *) calloc(1, sizeof(*ptr->next));
		if (NULL == h_array[hash_value]) {
			fputs("Failed to allocate bucked", stderr);
			exit(-1);
		}
		ptr->next->key = key;
		h_array[hash_value]->tree = tree;
		h_array[hash_value]->dfaState = state;
		ptr->next->next = NULL;
	}
}

void delete(unsigned long long key) {
	int hash_value = hash_function(key);
	if (NULL != h_array[hash_value]) {
		hash *ptr = h_array[hash_value];
		hash *ptr2;
		if (ptr->key == key) {
			ptr2 = ptr->next;
			free(ptr);
			h_array[hash_value] = ptr2;
			return;
		}
		while (NULL != ptr->next) {
			if (ptr->next->key == key) {
				ptr2 = ptr->next->next;
				free(ptr->next);
				ptr->next = ptr2;
				return;
			}
			ptr = ptr->next;
		}
		return;
	}
}