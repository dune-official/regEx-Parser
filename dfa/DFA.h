#ifndef REXEX_PARSER_DFA_H
#define REXEX_PARSER_DFA_H

#define false 0
#define true 1

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct DFA_state {
	_Bool is_final;
	unsigned char index;

	struct DFA_state *alphabet[94];

} DFA;

extern DFA **getDFA(unsigned char stateCount);

extern DFA *addState(DFA **dfaArray, int newStateCount);

extern void setState(DFA **dfa, char state, char symbol, char nextState);

extern void addFinal(DFA **dfa, char state);

extern _Bool matchDFA(DFA **restrict dfa, const char *restrict string, int matchLen);

int *matchAnyDFA(DFA **restrict dfa, const char *restrict string, int matchLen);

extern void printDelta(DFA **dfa, int stateCount);

#endif
