#include "../../header/parser.h"

/* This function creates a new regex node.
 * Input:
 * - unsigned char regex_type: The RegEx type (defined in regex.h);
 * Output:
 * - regex_node *root: The root node;
 */
regex_node *regex_get_node(unsigned char regex_type) {
	regex_node *root = (regex_node *) calloc(1, sizeof(*root));
	if (NULL == root) {
		fputs("Failed to initialize buffer", stderr);
		exit(1);
	}
	root->type = regex_type;
	root->symbol = 0;
	root->hash = 0;
	return root;
}

/* This function creates a new regex symbol node. This represents a base character. The hash is its ASCII value.
 * Input:
 * - unsigned char symbol: The literal symbol;
 * Output:
 * - regex_node *symbol: The symbol node;
 */
regex_node *regex_symbol(unsigned char symbol) {
	regex_node *sym = regex_get_node(SYMBOL);
	sym->symbol = symbol;
	sym->hash = (unsigned long long) symbol;
	return sym;
}

/* This function creates a regex union node. It uses the unmodified Szudzik function as hash.
 * If either side is an empty symbol, the other symbol gets returned on its own.
 * If both hashes match (same expression), the left hand side gets returned.
 *
 * A | B -> A|B
 * A | ee -> A
 * A | A -> A
 *
 * Input:
 * - regex_node *restrict LHS: The left hand side of the union;
 * - regex_node *restrict RHS: The right hand side of the union;
 * Output:
 * - regex_node *union: The union node;
 */
regex_node *regex_union(regex_node *restrict LHS, regex_node *restrict RHS) {
	if (LHS->type == SYMBOL && LHS->symbol == EMPTY) {
		free(LHS);
		return RHS;
	} else if (RHS->type == SYMBOL && RHS->symbol == EMPTY || LHS->hash == RHS->hash) {
		free(RHS);
		return LHS;
	} else {
		regex_node *uni = regex_get_node(UNION);
		uni->LHS = LHS;
		uni->RHS = RHS;
		uni->hash = SZD(LHS->hash, RHS->hash);
		return uni;
	}
}

/* This function creates a regex concatenation node. It uses the modified Szudzik function as hash.
 * If either side is an empty symbol, the empty symbol gets returned.
 * If either side is an epsilon, the other side gets returned.
 * If both hashes match (same expression) and both expressions are kleene closures, the left hand side gets returned.
 *
 * A + B -> AB
 * A + ee -> ee
 * A + ep -> A
 * A* + A* -> A*
 *
 * Input:
 * - regex_node *restrict LHS: The left hand side of the concatenation;
 * - regex_node *restrict RHS: The right hand side of the concatenation;
 * Output:
 * - regex_node *union: The concatenation node;
 */
regex_node *regex_concat(regex_node *restrict LHS, regex_node *restrict RHS) {
	regex_node *cnt;

	switch (LHS->type) {
		case SYMBOL:
			if (LHS->symbol == EMPTY) {
				free(RHS);
				return LHS;
			} else if (LHS->symbol == EPSILON) {
				free(LHS);
				return RHS;
			}
	}

	switch (RHS->type) {
		case SYMBOL:
			if (RHS->symbol == EMPTY) {
				free(LHS);
				return RHS;
			} else if (RHS->symbol == EPSILON) {
				free(RHS);
				return LHS;
			}
	}

	if (LHS->hash == RHS->hash && LHS->type == KLEENE && RHS->type == KLEENE) {
		free(RHS);
		return LHS;
	}

	cnt = regex_get_node(CONCAT);
	cnt->LHS = LHS;
	cnt->RHS = RHS;
	cnt->hash = SMOD(LHS->hash, RHS->hash);
	return cnt;
}

/* This function creates a kleene closure node. It just adds one to its children hash.
 * If the kid is already a kleene closure, it gets returned as is.
 * If the kid is either epsilon or the empty expression, an epsilon gets returned.
 *
 * A -> A*
 * ee -> ep
 * ep -> ep
 * A* -> A*
 *
 * Input:
 * - regex_node *child: The child of the kleene closure;
 * Output:
 * - regex_node *union: The kleene closure node. Here, only the LHS is set;
 */
regex_node *regex_kleene(regex_node *child) {
	if (child->type == KLEENE) {
		return child;
	} else if (child->type == SYMBOL && (child->symbol == EPSILON || child->symbol == EMPTY)) {
		child->symbol = EPSILON;
		return child;
	} else {
		regex_node *kln = regex_get_node(KLEENE);
		kln->LHS = child;
		kln->hash = child->hash + 1;
		return kln;
	}
}

/* This function recursively creates a copy of a given subtree. This is used within regex derivations.
 *
 * Input:
 * - regex_node *child: The subtree;
 * Output:
 * - regex_node *root: The copied tree;
 */
regex_node *regex_copy_tree(regex_node *child) {
	regex_node *root = regex_get_node(child->type);
	root->hash = child->hash;
	if (child->type == SYMBOL) {
		root->symbol = child->symbol;
	} else {
        root->LHS = regex_copy_tree(child->LHS);

		if (NULL != child->RHS) {
			root->RHS = regex_copy_tree(child->RHS);
		}
	}

	return root;
}

/* This function recursively prints out the regex statement in infix notation.
 *
 * Input:
 * - const regex_node * root: The subtree to print;
 */
void regex_print_regexp(const regex_node *root) {
	switch (root->type) {
		case CONCAT:
            regex_print_regexp(root->LHS);
            regex_print_regexp(root->RHS);
			break;
		case UNION:
			putchar('(');
            regex_print_regexp(root->LHS);
			putchar('|');
            regex_print_regexp(root->RHS);
			putchar(')');
			break;
		case KLEENE:
			if (root->LHS->type == UNION) {
                regex_print_regexp(root->LHS);
				putchar('*');
			} else {
				putchar('(');
                regex_print_regexp(root->LHS);
				putchar(')');
				putchar('*');
			}
			break;
		case SYMBOL:
			switch (root->symbol) {
				case EPSILON:
					putchar('$');
					break;
				case EMPTY:
					putchar('{');
					putchar('}');
					break;
				default:
					putchar(root->symbol);
					break;
			}
			break;

			/* excellent debug method */
		default:
			fputs("This should not have happened - panic mode", stderr);
			exit(1);
	}
}
