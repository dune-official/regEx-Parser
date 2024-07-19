#include "../../header/parser.h"

/* Generates the minimized DFA for matching the given regex pattern.
 * This function should provide a significant improvement in speed,
 * when, for example, matching against a very long input and using a very large and/or right leaning tree */
dfa *regex_pattern_to_dfa(regex_node *pattern) {
	regex_node *new_tree, *ptr_r = pattern;

	queue *q = queue_initialize();
    hashmap *hash_root = hash_initialize(769);
	dfa *dfa = dfa_initialize(1);

	dfa->start->is_final = regex_is_nullable(ptr_r);
	dfa_state *current_DFA, *ptr;

	unsigned long long ptr_r_hash;

    /* start with the initial state (the entire tree) */
	hash_insert(hash_root, ptr_r->hash, ptr_r, dfa->start);
    queue_enqueue(ptr_r->hash, q);

	char i;

	while (!queue_is_empty(q)) {
        ptr_r_hash = queue_dequeue(q);
		ptr_r = hash_get_tree(hash_root, ptr_r_hash);
        current_DFA = hash_get_dfa(hash_root, ptr_r_hash);

        /* ASCII printable chars */
		for (i = 0; i < 94; i++) {

            new_tree = regex_copy_tree(ptr_r);
            new_tree = regex_derive(new_tree, (char) (i + 32));

			if (NULL != (ptr = hash_get_dfa(hash_root, new_tree->hash))) {
                current_DFA->alphabet[i] = ptr;
			} else {
				ptr = dfa_get_state();
                ptr->is_final = regex_is_nullable(new_tree);

                if (EMPTY == new_tree->symbol) {
                    ptr->is_dead = 1;
                }

                current_DFA->alphabet[i] = ptr;

				hash_insert(hash_root, new_tree->hash, new_tree, ptr);
                queue_enqueue(new_tree->hash, q);
			}
		}
	}

	return dfa;
}

/* This function recursively determines if a regex node is nullable (Janosch Brzozowski).
 *
 * union: is_nullable(LHS) OR is_nullable(RHS)
 * concat: is_nullable(LHS) AND is_nullable(RHS)
 * symbol: false
 * ep: true
 *
 * Input:
 * - regex_node *rN: The subtree to derive;
 * Output:
 * - regex_node *return_node: The derived node;
 */
bool regex_is_nullable(regex_node *rN) {
	switch (rN->type) {
		case UNION:
			return regex_is_nullable(rN->LHS) || regex_is_nullable(rN->RHS) ? true : false;
		case CONCAT:
			return regex_is_nullable(rN->LHS) && regex_is_nullable(rN->RHS) ? true : false;
		case KLEENE:
			return true;
		case SYMBOL:
			switch (rN->symbol) {
				case EPSILON:
					return true;
			}
		default:
			return false;
	}
}

/* This function recursively derives a regular expression rN in respect to a (Janosch Brzozowski).
 *
 * derive(union, a) -> union(derive(LHS, a), derive(RHS, a))
 * derive(concat, a) -> if not nullable(LHS): concat(derive(LHS, a), RHS), else: union(concat(derive(LHS, a), copiedTree(RHS)), derive(RHS, a))
 * derive(kleene, a) -> concat(derive(LHS, a), copiedTree(RHS))
 * derive(symbol, a) -> symbol == a: ep else: ee
 *
 * Input:
 * - regex_node *rN: The subtree to derive;
 * Output:
 * - regex_node *return_node: The derived node;
 */
regex_node *regex_derive(regex_node *rN, char a) {

	regex_node *cN, *return_node;

	switch (rN->type) {
		case UNION:
            return_node = regex_union(regex_derive(rN->LHS, a), regex_derive(rN->RHS, a));
			free(rN);
			return return_node;
		case CONCAT:
			if (!regex_is_nullable(rN->LHS)) {
                return_node = regex_concat(regex_derive(rN->LHS, a), rN->RHS);
				free(rN);
				return return_node;
			} else {
				cN = regex_copy_tree(rN->RHS);
                return_node = regex_union(regex_concat(regex_derive(rN->LHS, a), cN), regex_derive(rN->RHS, a));
				free(rN);
				return return_node;
			}
		case KLEENE:
			cN = regex_copy_tree(rN);
            return_node = regex_concat(regex_derive(rN->LHS, a), cN);
			free(rN);
			return return_node;
		case SYMBOL:
			if (a == rN->symbol) {
                return_node = regex_symbol(EPSILON);
				free(rN);
				return return_node;
			} else {
                return_node = regex_symbol(EMPTY);
				free(rN);
				return return_node;
			}
		default:
			return NULL;
	}
}

__attribute__((unused)) _Bool regex_match(regex_node *restrict pattern, char *string, int matchLen) {
	int i;
	for (i = 0; i < matchLen; i++) {
		pattern = regex_derive(pattern, string[i]);

		/* preemptively exit the regex_match process, since an empty string will never be more than empty */
		switch (pattern->type) {
			case SYMBOL:
				if (pattern->symbol == EMPTY) {
					return false;
				}
		}
	}
	return regex_is_nullable(pattern);
}