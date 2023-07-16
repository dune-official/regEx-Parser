#ifndef REGEX_PARSER_DFA_H
#define REGEX_PARSER_DFA_H

#define false 0
#define true 1

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct DFA_state {
    _Bool is_final, is_dead;

    struct DFA_state *alphabet[94];
} dfa_state;

typedef struct DFA_wrapper {
    struct DFA_state *start;
    __attribute__((unused)) int state_count;
} dfa;


extern dfa *get_dfa(unsigned char stateCount);

extern dfa_state *get_state();

extern _Bool matchDFA(dfa *restrict dfa, const char *restrict string, int matchLen);

__attribute__((unused)) extern _Bool matchDFAPreemptive(dfa *restrict dfa, const char *restrict string, int strLen);

__attribute__((unused)) int *matchAnyDFA(dfa *restrict dfa, const char *restrict string, int matchLen);

__attribute__((unused)) extern void printDelta(dfa *dfa_state);

#endif
