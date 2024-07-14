#include "../../header/parser.h"

/* This function creates a new Deterministic Finite Automaton root node.
 *
 * Input:
 * - unsigned char state_count: The state count;
 * Output:
 * - dfa *wrapper: The DFA that just got created;
 */
dfa *get_dfa(unsigned char state_count) {

    NEW(dfa, wrapper, state_count)
    NEWSTRUCT(dfa_state, wrapper->start)

    wrapper->state_count = 1;
    return wrapper;
}

/* This function allocates and returns a new DFA state.
 *
 * Output:
 * - dfa_state *state: The state;
 */
dfa_state *get_state() {
    NEW(dfa_state, state, 1)
    return state;
}

/* This function matches a given DFA to its input.
 *
 * Input:
 * - dfa *restrict dfa: The DFA to match against;
 * - const char *restrict string: The string to match;
 * - int str_len: The length of the string;
 * Output:
 * - bool is_final: true if the last state the DFA landed on is a final state (match);
 */
bool matchDFA(dfa *restrict dfa, const char *restrict string, int str_len) {

	dfa_state *cur_state = dfa->start;

	int i;

	for (i = 0; i < str_len; i++) {
		cur_state = cur_state->alphabet[string[i] - 32];
	}

	return cur_state->is_final;
}

/* This function matches a given DFA to its input, however, tries to exit preemptively.
 *
 * Input:
 * - dfa *restrict dfa: The DFA to match against;
 * - const char *restrict string: The string to match;
 * - int str_len: The length of the string;
 * Output:
 * - bool is_final: true if the last state the DFA landed on is a final state (match);
 */
__attribute__((unused)) bool matchDFAPreemptive(dfa *restrict dfa, const char *restrict string, int strLen) {
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