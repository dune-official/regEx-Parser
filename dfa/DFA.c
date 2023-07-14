#include "DFA.h"

DFA **getDFA(unsigned char stateCount) {

	DFA **dfaArray = (DFA **) calloc(stateCount, sizeof(**dfaArray));
	if (NULL == dfaArray) {
		fputs("Failed to init buffer", stderr);
		exit(1);
	}

	int i;
	for (i = 0; i < stateCount; i++) {
		dfaArray[i] = (DFA *) calloc(1, sizeof(*dfaArray[i]));
		if (NULL == dfaArray[i]) {
			fputs("Failed to init buffer", stderr);
			exit(1);
		}

		dfaArray[i]->is_final = false;
		dfaArray[i]->index = i;
	}

	return dfaArray;
}

DFA *addState(DFA **dfa, int newStateCount) {
	DFA **dfaArrayTemp = (DFA **) realloc(dfa, newStateCount);
	if (NULL == dfaArrayTemp) {
		fputs("Failed to init buffer", stderr);
		exit(1);
	}
	dfa = dfaArrayTemp;

	dfa[newStateCount - 1] = (DFA *) calloc(1, sizeof(*dfa[newStateCount - 1]));
	if (NULL == dfa[newStateCount - 1]) {
		fputs("Failed to init buffer", stderr);
		exit(1);
	}

	dfa[newStateCount - 1]->is_final = false;
	dfa[newStateCount - 1]->index = newStateCount - 1;

	return dfa[newStateCount - 1];
}

void setState(DFA **dfa, char state, char symbol, char next_state) {
	dfa[state]->alphabet[symbol - 32] = dfa[next_state];
}


void addFinal(DFA **dfa, char state) {
	dfa[state]->is_final = true;
}

_Bool matchDFA(DFA **restrict dfa, const char *restrict string, int matchLen) {

	DFA *cur_state = dfa[0];

	int i;
	for (i = 0; i < matchLen; i++) {
		cur_state = cur_state->alphabet[string[i] - 32];
	}

	return cur_state->is_final;
}

int *matchAnyDFA(DFA **restrict dfa, const char *restrict string, int matchLen) {
	int *range = NULL, *rngTemp;

	DFA *cur_state = dfa[0];
	range = (int *) calloc(1, sizeof(range));
	if (NULL == range) {
		fputs("Failed to init buffer", stderr);
		exit(1);
	}
	range[0] = 1;

	int i, j = 0;
	for (i = 0; i < matchLen; i++) {
		cur_state = cur_state->alphabet[string[i] - 32];
		if (cur_state->is_final) {

			while (cur_state->is_final) {
				cur_state = cur_state->alphabet[string[i] - 32];
				i++;
			}

			rngTemp = (int *) realloc(range, range[0] + 2);
			if (NULL == rngTemp) {
				fputs("Failed to init buffer", stderr);
				exit(1);
			}

			range[range[0] + 1] = j;
			range[range[0] + 2] = i - 1;
			range[0] += 2;

			cur_state = dfa[0];

		} else {
			j = i;
		}
	}

	return range;
}

void printDelta(DFA **dfa, int stateCount) {
	int i, j;
	for (i = 0; i < stateCount; i++) {
		for (j = 0; j < 94; j++) {
			if (i == 0) {
				putchar(j + 32);
			} else {
				putchar(dfa[i]->alphabet[j]->index + 48);
			}
			putchar(' ');
		}
	}
}