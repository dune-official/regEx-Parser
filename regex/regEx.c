#include "regex.h"
#include "../hash/hash.h"
#include "../queue/queue.h"

/* Generates the minimized DFA for matching the given regex pattern.
 * This function should provide a significant improvement in speed,
 * when, for example, matching against a very long input and using a very large and/or convoluted tree */
DFA **patternToDFA(regexNode *pattern) {
	regexNode *newTree, *ptrR;
	ptrR = pattern;

	queue *q = initialize_queue();

	DFA **dfa = getDFA(1);
	dfa[0]->is_final = isNullable(ptrR);
	DFA *currentDFA, *ptr;
	int dfaSize = 1;

	unsigned long long ptrRHash;

	hash_insert(ptrR->hash, ptrR, dfa[0]);
	enqueue(ptrR->hash, q);

	char i;

	while (!is_empty_queue(q)) {
		ptrRHash = dequeue(q);
		ptrR = hash_getRegExTree(ptrRHash);
		currentDFA = hash_getDFA(ptrRHash);

		for (i = 0; i < 94; i++) {

			newTree = copyTree(ptrR);
			newTree = derive(newTree, i+32);

			if (NULL != (ptr = hash_getDFA(newTree->hash))) {
				currentDFA->alphabet[i] = ptr;
			} else {
				ptr = addState(dfa, ++dfaSize);
				ptr->is_final = isNullable(newTree);

				hash_insert(newTree->hash, newTree, ptr);
				enqueue(newTree->hash, q);
			}
		}
	}

	return dfa;
}

_Bool isNullable(regexNode *rN) {
	switch (rN->type) {
		case UNION:
			return isNullable(rN->LHS) || isNullable(rN->RHS) ? true : false;
		case CONCAT:
			return isNullable(rN->LHS) && isNullable(rN->RHS) ? true : false;
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

/* - derives a regular expression rN in respect to a
 * - big thanks go out to the G Janosch Brzozowski */
regexNode *derive(regexNode *rN, char a) {

	regexNode *cN, *returnNode;

	switch (rN->type) {
		case UNION:
			returnNode = union_re(derive(rN->LHS, a), derive(rN->RHS, a));
			free(rN);
			return returnNode;
		case CONCAT:
			if (!isNullable(rN->LHS)) {
				returnNode = concat(derive(rN->LHS, a), rN->RHS);
				free(rN);
				return returnNode;
			} else {
				cN = copyTree(rN->RHS);
				returnNode = union_re(concat(derive(rN->LHS, a), cN), derive(rN->RHS, a));
				free(rN);
				return returnNode;
			}
		case KLEENE:
			cN = copyTree(rN);
			returnNode = concat(derive(rN->LHS, a), cN);
			free(rN);
			return returnNode;
		case SYMBOL:
			if (a == rN->symbol) {
				returnNode = symbol(EPSILON);
				free(rN);
				return returnNode;
			} else {
				returnNode = symbol(EMPTY);
				free(rN);
				return returnNode;
			}
		default:
			return NULL;
	}
}

_Bool match(regexNode *restrict pattern, char *string, int matchLen) {
	int i;
	for (i = 0; i < matchLen; i++) {
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