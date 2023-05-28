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

_Bool isNullable(regexNode *rN) {
	switch (rN->type) {
		case UNION:
			return rN->isNullable ^ rN->negated;
		default:
			return rN->isNullable;
	}
}

/* - derives a regular expression rN in respect to a
 * - big thanks go out to the G Janosch Brzozowski */
regexNode *derive(regexNode *rN, char a) {

	regexNode *cN;

	switch (rN->type) {
		case UNION:
			return union_re(derive(rN->LHS, a), derive(rN->RHS, a));
		case CONCAT:
			if (!isNullable(rN->LHS)) {
				return concat(derive(rN->LHS, a), rN->RHS);
			} else {
				cN = copyTree(rN->RHS);
				return union_re(concat(derive(rN->LHS, a), cN), derive(rN->RHS, a));
			}
		case KLEENE:
			cN = copyTree(rN);
			return concat(derive(rN->LHS, a), cN);
		case SYMBOL:
			if (a == rN->symbol) {
				return symbol(EPSILON);
			} else {
                return symbol(EMPTY);
			}
		default:
			return NULL;
	}
}

_Bool match(regexNode *restrict pattern, char *restrict string) {
	int i;
	for (i = 0; i < strnlen(string, 255); i++) {
		pattern = derive(pattern, string[i]);

        /* preemptively exit the match process, since an empty string will never be more than empty */
        switch (pattern->type) {
            case SYMBOL:
                if (pattern->symbol == EMPTY) {
                    return false;
                }
        }
	}

	return isNullable(pattern);
}

_Bool matchAny(regexNode *restrict pattern, char *restrict string) {
    int i, j;
    regexNode *copiedPattern = copyTree(pattern);

    for (i = 0; ;) {

    }

    return false;
}
