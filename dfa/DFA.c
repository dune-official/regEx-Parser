#include "DFA.h"

dfa *get_dfa(unsigned char stateCount) {

    dfa *dfaWrapper = (dfa *) calloc(stateCount, sizeof(*dfaWrapper));
    if (NULL == dfaWrapper) {
        fputs("Failed to initialize buffer", stderr);
        exit(1);
    }

    dfaWrapper->start = (dfa_state *) calloc(1, sizeof(*dfaWrapper->start));
    if (NULL == dfaWrapper->start) {
        fputs("Failed to initialize buffer", stderr);
        exit(1);
    }
    dfaWrapper->state_count = 1;
    return dfaWrapper;
}

dfa_state *get_state() {
    dfa_state *toReturn = (dfa_state *) calloc(1, sizeof(*toReturn));
    if (NULL == toReturn) {
        fputs("Failed to initialize buffer", stderr);
        exit(1);
    }
    return toReturn;
}

_Bool matchDFA(dfa *restrict dfa, const char *restrict string, int strLen) {

	dfa_state *cur_state = dfa->start;

	int i;

	for (i = 0; i < strLen; i++) {
		cur_state = cur_state->alphabet[string[i] - 32];
	}

	return cur_state->is_final;
}

__attribute__((unused)) _Bool matchDFAPreemptive(dfa *restrict dfa, const char *restrict string, int strLen) {
    dfa_state *cur_state = dfa->start;

    int i;
    for (i = 0; i < strLen; i++) {
        cur_state = cur_state->alphabet[string[i] - 32];
        if (cur_state->is_dead) {
            return false;
        }
    }

    return cur_state->is_final;
}