#include "regex.h"

regexNode *symbol(char symbol) {
	regexNode *sym = getNode(SYMBOL);
	sym->symbol = symbol;
	sym->hash = (symbol - 30);
	return sym;
}

regexNode *union_re(regexNode *LHS, regexNode *RHS) {
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
		return uni;
	}
}

regexNode *concat(regexNode *LHS, regexNode *RHS) {
	regexNode *cnt;
	switch (LHS->type) {
		case SYMBOL:
			if (LHS->symbol == EPSILON) {
				free(LHS);
				return RHS;
			} else if (LHS->symbol == EMPTY) {
				free(RHS);
				return LHS;
			}
	}

	switch (RHS->type) {
		case SYMBOL:
			if (RHS->symbol == EPSILON) {
				free(RHS);
				return LHS;
			} else if (RHS->symbol == EMPTY) {
				free(LHS);
				return RHS;
			}
	}

	cnt = getNode(CONCAT);
	cnt->LHS = LHS;
	cnt->RHS = RHS;
	cnt->hash = SMOD(LHS->hash, RHS->hash);
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
		return kln;
	}
}

regexNode *copyTree(regexNode *child) {
	regexNode *newNode = getNode(child->type);
	newNode->hash = child->hash;
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
					printf("$");
					break;
				case EMPTY:
					printf("{}");
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
