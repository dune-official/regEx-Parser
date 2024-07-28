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
            printf("%2d %2d ", matcher_root->from, matcher_root->to);
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
 * - pattern *restrict pattern: The pattern of the regex pattern;
 * - const char *restrict string: The string to match against;
 * - int str_len: The string length;
 * Output:
 * - matcher *match_root: The root of the matcher linked list;
 */
matcher *matcher_match_all(pattern *restrict pattern, const char *restrict string, int str_len) {
    dfa_state *cur_state = pattern->start;
    unsigned int i;
    NEW(matcher, matcher_root, 1)

    unsigned int buffer[2] = {0, 0};
    bool matched = false;

    matcher *ptr = matcher_root;

    for (i = 0; i < str_len; i++) {

        cur_state = cur_state->alphabet[string[i] - 32];
        if (cur_state->is_final) {

            matched = true;
            buffer[1] = i;
            continue;
        }

        if (cur_state->is_dead) {
            cur_state = pattern->start;
            if (matched) {
                matched = false;
                ptr = matcher_add_match(ptr, buffer);
                i--;
            }
            buffer[0] = i+1;
            continue;
        }
    }

    if (matched) {
        ptr = matcher_add_match(ptr, buffer);
    }

    return matcher_root;
}

/* This function matches a given DFA to its input fully (once from top to bottom).
 *
 * Input:
 * - pattern *restrict pattern: The pattern of the regex pattern;
 * - const char *restrict string: The string to match;
 * - int str_len: The length of the string;
 * Output:
 * - bool is_final: true if the last state the DFA landed on is a final state (match);
 */
bool matcher_match_full(pattern *restrict pattern, const char *restrict string, int str_len) {

    dfa_state *cur_state = pattern->start;
    int i;

    for (i = 0; i < str_len; i++) {
        cur_state = cur_state->alphabet[string[i] - 32];
    }

    return cur_state->is_final;
}

pattern *matcher_get_pattern(char *pattern_string, char pattern_string_len) {
    seek *tokenstream = lexer_tokenize(pattern_string, pattern_string_len);
    regex_node *tree = parser_parse(tokenstream, PR_LOWEST);

#ifdef DEBUG
    putchar('>');
    regex_print_regexp(tree);
    putchar('\n');
#endif

    return regex_pattern_to_dfa(tree);
}

char *matcher_get_match(matcher *restrict match_object, char *restrict string) {
    unsigned copysize = match_object->to - match_object->from + 1;
    NEW(char, buffer, copysize)
    strncpy(buffer, string + match_object->from, copysize);
    return buffer;
}
