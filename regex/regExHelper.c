#include "regex.h"

regexNode *getNode(unsigned char regexType) {
	regexNode *newNode = (regexNode *) calloc(1, sizeof(*newNode));
	if (NULL == newNode) {
		fputs("Failed to init buffer", stderr);
		exit(1);
	}
	newNode->type = regexType;
	newNode->symbol = 0;
	newNode->hash = 0;
	return newNode;
}

regexNode *symbol(char symbol) {
	regexNode *sym = getNode(SYMBOL);
	sym->symbol = symbol;
	sym->hash = (symbol - 30);

    if (symbol == EPSILON) {
        sym->isNullable = true;
    }

	return sym;
}

regexNode *union_re(regexNode *restrict LHS, regexNode * restrict RHS) {
	if (LHS->type == SYMBOL && LHS->symbol == EMPTY) {
		free(LHS);
		return RHS;
	} else if (RHS->type == SYMBOL && RHS->symbol == EMPTY || LHS->hash == RHS->hash) {
		free(RHS);
		return LHS;
	} else {
		regexNode *uni = getNode(UNION);
		uni->LHS = LHS;
		uni->RHS = RHS;
		uni->hash = S(LHS->hash, RHS->hash);

        uni->isNullable = LHS->isNullable || RHS->isNullable;

		return uni;
	}
}

regexNode *concat(regexNode *restrict LHS, regexNode *restrict RHS) {
	regexNode *cnt;
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

	cnt = getNode(CONCAT);
	cnt->LHS = LHS;
	cnt->RHS = RHS;
	cnt->hash = SMOD(LHS->hash, RHS->hash);

    cnt->isNullable = LHS->isNullable && RHS->isNullable;

	return cnt;
}

regexNode *kleene(regexNode *child) {
	if (child->type == KLEENE) {
		return child;
	} else if (child->type == SYMBOL && (child->symbol == EPSILON || child->symbol == EMPTY)) {
		child->symbol = EPSILON;
		return child;
	} else {
		regexNode *kln = getNode(KLEENE);
		kln->LHS = child;
		kln->hash = child->hash + 1;

        kln->isNullable = true;

		return kln;
	}
}

regexNode *copyTree(regexNode *child) {
	regexNode *newNode = getNode(child->type);

    /* remember when these didn't exist? */
	newNode->hash = child->hash;
    newNode->isNullable = child->isNullable;
    newNode->negated = child->negated;

	if (child->type == SYMBOL) {
		newNode->symbol = child->symbol;
	} else {
		newNode->LHS = copyTree(child->LHS);
		if (NULL != child->RHS) {
			newNode->RHS = copyTree(child->RHS);
		}
	}

	return newNode;
}

/* prints out the regex statement in Infix Notation */
void print_regExp(const regexNode *root) {
	switch (root->type) {
		case CONCAT:
			print_regExp(root->LHS);
			print_regExp(root->RHS);
			break;
		case UNION:
			putchar('(');
			print_regExp(root->LHS);
			putchar('|');
			print_regExp(root->RHS);
			putchar(')');
			break;
		case KLEENE:
			if (root->LHS->type == UNION) {
				print_regExp(root->LHS);
				putchar('*');
			} else {
				putchar('(');
				print_regExp(root->LHS);
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
					putchar('{');putchar('}');
					break;
				default:
					putchar(root->symbol);
					break;
			}
			break;

        /* 1a debug methode */
		default:
			fputs("This should not have happened - panic mode", stderr);
			exit(1);
	}
}