#ifndef REGEX_PARSER_DFA_H
#define REGEX_PARSER_DFA_H

#define false 0
#define true 1

#include "parser.h"


dfa *get_dfa(unsigned char state_count);

dfa_state *get_state();

bool match_dfa(dfa *restrict dfa, const char *restrict string, int str_len);

bool match_dfa_preemptive(dfa *restrict dfa, const char *restrict string, int strLen);

int *matchAnyDFA(dfa *restrict dfa, const char *restrict string, int matchLen);

void printDelta(dfa *dfa_state);

#endif
