#include "../../header/parser.h"
#define MAXSIZE 769

hash *h_array[MAXSIZE];

int hash_function(unsigned long long value) {
	return (int) (value % MAXSIZE);
}

dfa_state *hash_getDFA(unsigned long long key) {
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

regex_node *hash_getRegExTree(unsigned long long key) {
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

void hash_insert(unsigned long long key, regex_node *restrict tree, dfa_state *restrict state) {
	int hash_value = hash_function(key);

	if (NULL == h_array[hash_value]) {
		h_array[hash_value] = (hash *) calloc(1, sizeof(*h_array[hash_value]));
		if (NULL == h_array[hash_value]) {
			fputs("Failed to initialize buffer", stderr);
			exit(-1);
		}
		h_array[hash_value]->key = key;
		h_array[hash_value]->tree = tree;
		h_array[hash_value]->dfaState = state;
		h_array[hash_value]->next = NULL;
	} else {
		hash *ptr = h_array[hash_value];
        hash *ptr2;
		while (NULL != ptr->next) {
            ptr = ptr->next;
        }
		ptr2 = (hash *) calloc(1, sizeof(*ptr2));
		if (NULL == ptr2) {
			fputs("Failed to initialize buffer", stderr);
			exit(-1);
		}
        ptr2->key = key;
        ptr2->tree = tree;
        ptr2->dfaState = state;
        ptr2->next = NULL;

        ptr->next = ptr2;
	}
}