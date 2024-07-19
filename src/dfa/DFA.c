#include "../../header/parser.h"

/* This function adds the buffer to the matcher root and creates a next struct.
 *
 * Input:
 * - matcher *restrict matcher_root: The root of the matcher structure;
 * - const char *restrict chr: The current buffer;
 * Output:
 * - matcher *matcher_root: The next pointer of the struct;
 */
matcher *dfa_add_matcher(matcher *restrict matcher_root, const char *restrict chr) {
    NEWSTRUCT(char, matcher_root->buffer)
    strcpy(matcher_root->buffer, chr);
    NEWSTRUCT(matcher, matcher_root->next)
    return matcher_root->next;
}

/* This function creates a new Deterministic Finite Automaton root node.
 *
 * Input:
 * - unsigned char state_count: The state count;
 * Output:
 * - dfa *wrapper: The DFA that just got created;
 */
dfa *dfa_initialize(unsigned char state_count) {

    NEW(dfa, wrapper, state_count)
    NEWSTRUCT(dfa_state, wrapper->start)

    wrapper->state_count = 1;
    return wrapper;
}

/* This function sets the given state to a final state and sets its add matcher function to the correct one.
 *
 * Input:
 * - dfa_state *state: The state to set;
 */
void dfa_set_final(dfa_state *state) {
    state->is_final = true;
}

/* This function allocates and returns a new DFA state.
 *
 * Output:
 * - dfa_state *state: The state;
 */
dfa_state *dfa_get_state() {
    NEW(dfa_state, state, 1)
    return state;
}

matcher *dfa_match_all(dfa *restrict dfa, const char *restrict string, int str_len) {
    dfa_state *cur_state = dfa->start;
    int i;
    NEW(matcher, matcher_root, 1)
    NEW(char, buffer, str_len)

    int buffer_pos = 0;
    matcher *ptr = matcher_root;

    for (i = 0; i < str_len; i++) {
        cur_state = cur_state->alphabet[string[i] - 32];

        if (cur_state->is_final) {
            buffer[buffer_pos++] = string[i];
            continue;
        }

        if (cur_state->is_dead) {
            cur_state = dfa->start;

            if (buffer_pos != 0) {
                ptr = dfa_add_matcher(ptr, buffer);
                memset(buffer, 0x00, str_len);
                buffer_pos = 0;
            }
        }
    }

    if (buffer_pos != 0) {
        ptr = dfa_add_matcher(ptr, buffer);
    }

    return matcher_root;
}

/* This function matches a given DFA to its input fully (once from top to bottom).
 *
 * Input:
 * - dfa *restrict dfa: The DFA to match against;
 * - const char *restrict string: The string to match;
 * - int str_len: The length of the string;
 * Output:
 * - bool is_final: true if the last state the DFA landed on is a final state (match);
 */
bool dfa_match_full(dfa *restrict dfa, const char *restrict string, int str_len) {

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
__attribute__((unused)) bool dfa_match_once_preemptive(dfa *restrict dfa, const char *restrict string, int strLen) {
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