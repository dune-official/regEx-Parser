#include "../../header/parser.h"

/* recursively produces a balanced tree (as balanced as possible) with the digits */
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

regex_node *parser_get_alphanum() {
	return regex_union(parser_balanced_tree(17, 42), parser_balanced_tree(49, 74));
}


/* Equals [0-9a-fA-F] */
regex_node *parser_get_hex() {
    return regex_union(parser_balanced_tree(17, 22), parser_balanced_tree(49, 54));
}

void parser_advance(seek *tokenstream) {
	if (-1 == seekable_seek_right(tokenstream)) {
		fputs("Unexpected exhaust of token stream", stderr);
		exit(1);
	}
}

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

/* lac = "LookAhead Control" */
void parser_lookahead_epsilon(seek *tokenstream, char lac) {
	token *lookahead;
	if (NULL != (lookahead = (token *) seekable_peek_right(tokenstream)) && lookahead->type == lac) {
		NEW(token, new, 1)

		new->type = SYMBOL;
		new->precedence = PR_LOWEST;
		new->is_nud = 1;
		new->symbol = EPSILON;

        seekable_insert_node_right(tokenstream);
        seekable_set_right((void *) new, tokenstream->current);
	}
}

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

/* parses the supplied tokenstream with the pratt parser method. it is supposed to be initialized with the lowest
 * existing precedence, usually 0 */
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
            parser_advance(tokenstream);
			break;
		case '(':
			/* if the following token is a "|", we can assume that there has to be an EPSILON */
            parser_lookahead_epsilon(tokenstream, '|');
			left = parser_parse_group(tokenstream);
            parser_advance(tokenstream);
			break;
		case '[':
			left = parser_parse_set(tokenstream, PR_LOWEST);
            parser_lookahead_concat(tokenstream);
            parser_advance(tokenstream);
			break;
		case BACKSLASH:
			left = parser_parse_escaped(tkn->symbol);
            parser_advance(tokenstream);
			break;
		default:
            parser_error(tkn);
	}

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
				puts("Parsing quantifier");
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

regex_node *parser_parse_group(seek *tokenstream) {
    parser_advance(tokenstream);
	regex_node *expression = parser_parse(tokenstream, PR_LOWEST);

	token *next_token;
	if (NULL == (next_token = (token *) seekable_peek(tokenstream))) {
		fputs("Unexpected token type in token stream: EOF (Expected ')')", stderr);
		exit(1);
	}

	if (next_token->type != ')') {
		fprintf(stderr, "Unexpected token type in token stream: %c (Expected ')') ", next_token->type);
		exit(1);
	}

    parser_lookahead_concat(tokenstream);
	return expression;
}

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

regex_node *parser_parse_union(regex_node *restrict LHS, seek *restrict tokenstream) {
    parser_advance(tokenstream);
	regex_node *RHS = parser_parse(tokenstream, PR_UNION);
	return regex_union(LHS, RHS);
}

regex_node *parser_parse_concat(regex_node *restrict LHS, seek *restrict tokenstream) {
    parser_advance(tokenstream);
	regex_node *RHS = parser_parse(tokenstream, PR_CONCAT);
	return regex_concat(LHS, RHS);
}

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

regex_node *parser_parse_union_set(regex_node *restrict LHS, seek *restrict tokenstream) {
    /* don't parser_advance since the set function advances instead */
    regex_node *RHS = parser_parse_set(tokenstream, PR_UNION);
    return regex_union(LHS, RHS);
}

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
