#include "../../header/parser.h"

regex_node *parser_parse_concat(regex_node *restrict LHS, seek *restrict tokenstream);
regex_node *parser_parse_union(regex_node *restrict LHS, seek *restrict tokenstream);
regex_node *parser_parse_range(regex_node *restrict LHS, seek *restrict tokenstream);
regex_node *parser_parse_union_set(regex_node *restrict LHS, seek *restrict tokenstream);
regex_node *parser_parse_quantifier(regex_node *restrict LHS, seek *restrict tokenstream);

regex_node *parser_parse_group(seek *tokenstream);
regex_node *parser_parse_set(seek *restrict tokenstream, char precedence);

regex_node *parser_repeat_pattern(regex_node *pattern, char count);
regex_node *parser_balanced_tree(char from, char to);
regex_node *parser_parse_escaped(char escaped);
regex_node *parser_get_alphanum();
regex_node *parser_get_hex();

void parser_advance(seek *tokenstream);
void parser_error(token *token);
void parser_lookahead_epsilon(seek *tokenstream, char lookahead_control);
void parser_lookahead_concat(seek *tokenstream);
void parser_lookahead_union_set(seek *tokenstream);

/* This function parses the supplied token stream with the pratt parser method.
 * It is supposed to be initialized with the lowest existing precedence, usually 0.
 *
 * Input:
 * - seek *tokenstream: The token stream to parse;
 * - char precedence: The precedence of the current token;
 * Output:
 * - The resulting regex tree;
 */
regex_node *parser_parse(seek *tokenstream, char precedence) {
    regex_node *left;
    token *tkn = (token *) seekable_peek(tokenstream);

    if (!tkn->is_nud) {
        parser_error(tkn);
    }

    /* at first, we check for the null denominators */
    if (tkn->type != '(' && tkn->type != '[') {
        parser_lookahead_concat(tokenstream);
    }

    /* null denominators */
    switch (tkn->type) {
        case SYMBOL:
            left = regex_symbol(tkn->symbol);
            break;
        case '(':
            /* if the following token is a "|", we can assume that there has to be an EPSILON */
            parser_lookahead_epsilon(tokenstream, '|');
            left = parser_parse_group(tokenstream);
            parser_lookahead_concat(tokenstream);
            break;
        case '[':
            left = parser_parse_set(tokenstream, PR_LOWEST);
            parser_lookahead_concat(tokenstream);
            break;
        case '.':
            parser_lookahead_concat(tokenstream);
            left = parser_balanced_tree(' ' - 48, '~' - 48);
            break;
        case BACKSLASH:
            left = parser_parse_escaped(tkn->symbol);
            break;
        default:
            parser_error(tkn);
    }

    parser_advance(tokenstream);

    /* left denominators */
    token *next_token;
    if (NULL == (next_token = (token *) seekable_peek(tokenstream))) {
        return left;
    }

    while (precedence < next_token->precedence) {
        switch (next_token->type) {
            case '*':
                parser_lookahead_concat(tokenstream);
                left = regex_kleene(left);
                parser_advance(tokenstream);
                break;
            case '+':
                parser_lookahead_concat(tokenstream);
                left = regex_concat(left, regex_kleene(regex_copy_tree(left)));
                parser_advance(tokenstream);
                break;
            case '{':
                left = parser_parse_quantifier(left, tokenstream);
                parser_lookahead_concat(tokenstream);
                parser_advance(tokenstream);
                break;
            case '|':
                /* if the following token is a ")" or EOF, we can assume that there has to be an EPSILON */
                parser_lookahead_epsilon(tokenstream, ')');
                left = parser_parse_union(left, tokenstream);
                break;
            case CONCAT:
                left = parser_parse_concat(left, tokenstream);
                break;
            default:
                parser_error(tkn);
        }

        if (NULL == (next_token = (token *) seekable_peek(tokenstream))) {
            break;
        }
    }

    return left;
}

/* This function starts a parse for a set.
 *
  * Input:
 * - seek *tokenstream: The token stream to parse;
 * - char precedence: The precedence of the current token;
 * Output:
 * - The resulting regex subtree;
 */
regex_node *parser_parse_set(seek *tokenstream, char precedence) {
    token *cur_token, *next_token;
    regex_node *left;

    parser_advance(tokenstream);

    if (NULL == (cur_token = (token *) seekable_peek(tokenstream))) {
        fputs("Error: Expected token, got EOF", stderr);
        exit(1);
    }

    if (!cur_token->is_nud) {
        parser_error(cur_token);
    }

    switch (cur_token->type) {
        case '^':
            break;
        case SYMBOL:
            parser_lookahead_union_set(tokenstream);
            left = regex_symbol(cur_token->symbol);
            break;
        default:
            /* inside a set, we treat everything like a symbol */
            parser_lookahead_union_set(tokenstream);
            left = regex_symbol(cur_token->type);
            break;
    }
    parser_advance(tokenstream);

    next_token = (token *) seekable_peek(tokenstream);
    if (next_token->type == ']') {
        return left;
    }

    while (precedence < next_token->precedence) {
        switch (next_token->type) {
            case '-':
                left = parser_parse_range(left, tokenstream);
                break;
            case UNION:
                left = parser_parse_union_set(left, tokenstream);
                break;
            default:
                parser_error(next_token);
        }

        next_token = (token *) seekable_peek(tokenstream);
        if (next_token->type == ']') {
            return left;
        }
    }

    return left;
}

/* This function parses a regex group.
 * TODO: Create a regex capture group.
 *
 * Input:
 * - seek *tokenstream: The token stream to parse;
 */
regex_node *parser_parse_group(seek *tokenstream) {
    parser_advance(tokenstream);
    regex_node *expression = parser_parse(tokenstream, PR_LOWEST);

    token *next_token;
    if (NULL == (next_token = (token *) seekable_peek(tokenstream))) {
        fputs("Unexpected token type in token stream: EOF (Expected ')')", stderr);
        exit(1);
    }

    if (next_token->type != ')') {
        parser_error(next_token);
    }

    return expression;
}

/* This function parses an escaped character.
 *
 * Input:
 * - char escaped: The actual escaped char;
 * Output:
 * - regex_node *escaped: The resulting regex subtree;
 */
regex_node *parser_parse_escaped(char escaped) {
    switch (escaped) {
        case 'd':
            return parser_balanced_tree(0, 9);
        case 'x':
            return regex_union(parser_balanced_tree(0, 9), parser_get_hex());
        case 'w':
            return regex_union(regex_union(parser_balanced_tree(0, 9), parser_get_alphanum()), regex_symbol('_'));
        case 'n':
            return regex_symbol(10);
        default:
            fprintf(stderr, "Unexpected escape character: \\%c\n", escaped);
            exit(1);
    }
}

regex_node *parser_parse_quantifier(regex_node *restrict LHS, seek *restrict tokenstream) {

    token *number1, *number2, *comma;
    regex_node *expression;
    char i;

    parser_advance(tokenstream);

    if (NULL == (number1 = (token *) seekable_peek(tokenstream))) {
        fputs("Unexpected token type in token stream: EOF (Expected 'INT')", stderr);
        exit(1);
    }

    if (number1->type != INTEGER) {
        parser_error(number1);
    }

    parser_advance(tokenstream);
    if (NULL == (comma = (token *) seekable_peek(tokenstream))) {
        fputs("Unexpected token type in token stream: EOF (Expected ',')", stderr);
        exit(1);
    }

    if (comma->type == '}') {
        return parser_repeat_pattern(LHS, number1->symbol);
    }

    parser_advance(tokenstream);
    if (NULL == (number2 = (token *) seekable_peek(tokenstream))) {
        fputs("Unexpected token type in token stream: EOF (Expected 'INT')", stderr);
        exit(1);
    }

    if (number2->type == '}') {
        return regex_concat(parser_repeat_pattern(LHS, number1->symbol), regex_kleene(LHS));
    }

    if (number1->symbol == number2->symbol) {
        return parser_repeat_pattern(LHS, number1->symbol);
    } else if (number1->symbol > number2->symbol) {
        fputs("Invalid range detected", stderr);
        exit(1);
    }

    expression = regex_union(parser_repeat_pattern(LHS, number1->symbol), parser_repeat_pattern(LHS, number1->symbol+1));
    i = (char) (number1->symbol + 1);

    while (i < number2->symbol) {
        expression = regex_union(expression, parser_repeat_pattern(LHS, ++i));
    }

    parser_advance(tokenstream);

    return expression;
}

/* This function starts a union parse.
 *
 * Input:
 * - regex_node *LHS: The left hand side of the expression so far;
 * - seek *tokenstream: The tokenstream;
 * Output:
 * - regex_node *escaped: The resulting regex subtree;
 */
regex_node *parser_parse_union(regex_node *restrict LHS, seek *restrict tokenstream) {
    parser_advance(tokenstream);
    regex_node *RHS = parser_parse(tokenstream, PR_UNION);
    return regex_union(LHS, RHS);
}

/* This function starts a union parse for a set.
 *
 * Input:
 * - regex_node *LHS: The left hand side of the expression so far;
 * - seek *tokenstream: The tokenstream;
 * Output:
 * - regex_node *escaped: The resulting regex subtree;
 */
regex_node *parser_parse_union_set(regex_node *restrict LHS, seek *restrict tokenstream) {
    /* don't parser_advance since the set function advances instead */
    regex_node *RHS = parser_parse_set(tokenstream, PR_UNION);
    return regex_union(LHS, RHS);
}

/* This function starts a concat parse.
 *
 * Input:
 * - regex_node *LHS: The left hand side of the expression so far;
 * - seek *tokenstream: The tokenstream;
 * Output:
 * - regex_node *escaped: The resulting regex subtree;
 */
regex_node *parser_parse_concat(regex_node *restrict LHS, seek *restrict tokenstream) {
    parser_advance(tokenstream);
    regex_node *RHS = parser_parse(tokenstream, PR_CONCAT);
    return regex_concat(LHS, RHS);
}

/* This function starts a range parse from a set.
 *
 * Input:
 * - regex_node *LHS: The left hand side of the expression so far;
 * - seek *tokenstream: The tokenstream;
 * Output:
 * - regex_node *escaped: The resulting regex subtree;
 */
regex_node *parser_parse_range(regex_node *restrict LHS, seek *restrict tokenstream) {
    /* don't parser_advance since the set function advances instead */
    regex_node *RHS = parser_parse_set(tokenstream, PR_SETRANGE);

    unsigned char l_content = LHS->symbol;
    unsigned char r_content = RHS->symbol;

    free(LHS);
    free(RHS);

    if (l_content > r_content) {
        fputs("Invalid range detected", stderr);
        exit(1);
    }

    return parser_balanced_tree((char) l_content - 48, (char) r_content - 48);
}

regex_node *parser_repeat_pattern(regex_node *pattern, char count) {

    if (count == 1) {
        return pattern;
    }

    char limit;
    regex_node *expression = regex_concat(pattern, regex_copy_tree(pattern));

    for (limit = 2; limit < count; limit++) {
        expression = regex_concat(expression, regex_copy_tree(pattern));
    }
    return expression;
}

/* This function recursively produces a balanced regex tree (as balanced as possible) with the digits.
 * The tree is unions of unions. The chars have to be decreased by 48 of their ASCII value.
 *
 * Input:
 * - const char from: The start of the range;
 * - const char to: The end of the range;
 * Output:
 * - regex_node *tree: The balanced regex tree;
 */
regex_node *parser_balanced_tree(char from, char to) {
    if (from == to) {
        return regex_symbol((char) (from + 48));
    }

    char difference = (char) (to - from);
    char diff_half = (char) (difference / 2);

    if ((difference & 1) == 1) {
        return regex_union(parser_balanced_tree(from, (char) (from + diff_half)),
                           parser_balanced_tree((char) (to - diff_half), to));
    } else {
        return regex_union(parser_balanced_tree(from, (char) (from + diff_half - 1)),
                           parser_balanced_tree((char) (to - diff_half), to));
    }
}

/* This function recursively produces a balanced regex tree (as balanced as possible) of all the alphanumeric values.
 *
 * Output:
 * - regex_node *tree: The balanced regex tree;
 */
regex_node *parser_get_alphanum() {
	return regex_union(parser_balanced_tree(17, 42), parser_balanced_tree(49, 74));
}

/* This function recursively produces a balanced regex tree (as balanced as possible) of all the hex chars.
 * Equals [0-9a-fA-F].
 *
 * Output:
 * - regex_node *tree: The balanced regex tree;
 */
regex_node *parser_get_hex() {
    return regex_union(parser_balanced_tree(17, 22), parser_balanced_tree(49, 54));
}

/* This function checks the next token and if it is a NULL denominator, it inserts a CONCAT token.
 *
 * Input:
 * - seek *tokenstream: The token stream to check;
 */
void parser_lookahead_concat(seek *tokenstream) {
	token *lookahead;
	if (NULL != (lookahead = (token *) seekable_peek_right(tokenstream)) && lookahead->is_nud) {
        NEW(token, new, 1)

		new->type = CONCAT;
		new->precedence = PR_CONCAT;

        seekable_insert_node_right(tokenstream);
        seekable_set_right((void *) new, tokenstream->current);
	}
}

/* This function checks the next token and if it is the LAC, it inserts an EPSILON token.
 *
 * Input:
 * - seek *tokenstream: The token stream to check;
 * - char lookahead_control: The condition on wether to insert an EPSILON or not;
 */
void parser_lookahead_epsilon(seek *tokenstream, char lookahead_control) {
	token *lookahead;
	if (NULL != (lookahead = (token *) seekable_peek_right(tokenstream)) && lookahead->type == lookahead_control) {
		NEW(token, new, 1)

		new->type = SYMBOL;
		new->precedence = PR_LOWEST;
		new->is_nud = 1;
		new->symbol = EPSILON;

        seekable_insert_node_right(tokenstream);
        seekable_set_right((void *) new, tokenstream->current);
	}
}

/* This function checks the next token and if it is not a dash or ']', it inserts a UNION.
 *
 * Input:
 * - seek *tokenstream: The token stream to check;
 */
void parser_lookahead_union_set(seek *tokenstream) {

    token *lookahead = (token *) seekable_peek_right(tokenstream);
    if (NULL != lookahead && lookahead->type != '-' && lookahead->type != ']') {
        NEW(token, new, 1)

        new->type = UNION;
        new->precedence = PR_UNION;

        seekable_insert_node_right(tokenstream);
        seekable_set_right((void *) new, tokenstream->current);
    }
}

/* This function prints out the error in case an unknown token is encountered.
 *
 * Input:
 * - token *token: The erroneous token;
 */
void parser_error(token *token) {
    char err_type;
    switch (token->type) {
        case SYMBOL:
            err_type = token->symbol;
            break;
        case BACKSLASH:
            err_type = '\\';
            break;
        case CONCAT:
            err_type = '~';
            break;
        default:
            err_type = token->type;
    }

    fprintf(stderr, "Unexpected token in token stream: %c", err_type);
    exit(1);
}

/* This function advances the stream by one and checks the stream for exhaustion
 *
 * Input:
 * - seek *tokenstream: The tokenstream to advance;
 */
void parser_advance(seek *tokenstream) {
    if (-1 == seekable_seek_right(tokenstream)) {
        fputs("Unexpected exhaust of token stream", stderr);
        exit(1);
    }
}
