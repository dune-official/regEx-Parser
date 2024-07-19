#ifndef REGEX_PARSER_DFA_H
#define REGEX_PARSER_DFA_H

#define false 0
#define true 1

#include "parser.h"


dfa *dfa_initialize(unsigned char state_count);
dfa_state *dfa_get_state();
void dfa_set_final(dfa_state *state);
matcher *dfa_match_all(dfa *restrict dfa, const char *restrict string, int str_len);
bool dfa_match_full(dfa *restrict dfa, const char *restrict string, int str_len);
bool dfa_match_once_preemptive(dfa *restrict dfa, const char *restrict string, int strLen);
int *matchAnyDFA(dfa *restrict dfa, const char *restrict string, int matchLen);
void printDelta(dfa *dfa_state);

#endif
