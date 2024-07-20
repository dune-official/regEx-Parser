#include "../../header/parser.h"

/* This function adds the buffer to the matcher root and creates a next struct.
 *
 * Input:
 * - matcher *restrict matcher_root: The root of the matcher structure;
 * - const char *restrict chr: The current buffer;
 * Output:
 * - matcher *matcher_root: The next pointer of the struct;
 */
matcher *matcher_add_match(matcher *restrict matcher_root, const unsigned int *restrict buffer) {
    matcher_root->from = buffer[0];
    matcher_root->to = buffer[1];
    NEWSTRUCT(matcher, matcher_root->next)
    return matcher_root->next;
}

void matcher_print_matches(matcher *restrict matcher_root, const char *restrict string, bool show_indices) {
    unsigned int i;
    while (matcher_root->next != NULL) {

        if (show_indices) {
            printf("%2d - %2d: ", matcher_root->from, matcher_root->to);
        }

        for (i = matcher_root->from; i <= matcher_root->to; i++) {
            putchar(string[i]);
        }
        putchar('\n');
        matcher_root = matcher_root->next;
    }
}

/* This function matches all final state hits in the string. The output is a matcher linked list
 *  that keeps the pointers to the original string.
 *
 * Input:
 * - dfa *restrict dfa: The dfa of the regex pattern;
 * - const char *restrict string: The string to match against;
 * - int str_len: The string length;
 * Output:
 * - matcher *match_root: The root of the matcher linked list;
 */
matcher *matcher_match_all(dfa *restrict dfa, const char *restrict string, int str_len) {
    dfa_state *cur_state = dfa->start;
    unsigned int i;
    NEW(matcher, matcher_root, 1)

    /* we set the buffer to {1, 0} so that the dead state does not match the unmodified {0, 0} */
    unsigned int buffer[2] = {1, 0};
    matcher *ptr = matcher_root;

    for (i = 0; i < str_len; i++) {
        cur_state = cur_state->alphabet[string[i] - 32];
        if (cur_state->is_final) {
            buffer[1] = i;
            continue;
        }

        if (cur_state->is_dead) {
            cur_state = dfa->start;
            if (buffer[1] >= buffer[0]) {
                ptr = matcher_add_match(ptr, buffer);
            }
            buffer[0] = i+1;
            continue;
        }
    }

    return matcher_root;
}

/* This function matches a given DFA to its input fully (once from top to bottom).
 *
 * Input:
 * - dfa *restrict dfa: The dfa of the regex pattern;
 * - const char *restrict string: The string to match;
 * - int str_len: The length of the string;
 * Output:
 * - bool is_final: true if the last state the DFA landed on is a final state (match);
 */
bool matcher_match_full(dfa *restrict dfa, const char *restrict string, int str_len) {

    dfa_state *cur_state = dfa->start;
    int i;

    for (i = 0; i < str_len; i++) {
        cur_state = cur_state->alphabet[string[i] - 32];
    }

    return cur_state->is_final;
}
