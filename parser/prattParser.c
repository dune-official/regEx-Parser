#include "processPattern.h"

/* recursively produces a balanced tree (as balanced as possible) with the digits */
regexNode *balancedTree(char from, char to) {
    if (from == to) {
        return symbol((char) (from + 48));
    }

    char difference = (char) (to - from);
    char diffHalf = (char) (difference / 2);

    if ((difference & 1) == 1) {
        return union_re(balancedTree(from, (char) (from+diffHalf)), balancedTree((char) (to-diffHalf), to));
    } else {
        return union_re(balancedTree(from, (char) (from+diffHalf-1)), balancedTree((char) (to-diffHalf), to));
    }
}

regexNode *getAlphanum() {
	return union_re(balancedTree(17, 42), balancedTree(49, 74));
}


/* Equals [0-9a-f] */
regexNode *getHex() {
    return union_re(balancedTree(17, 22), balancedTree(49, 54));
}

void advance(seek *node) {
	if (-1 == seek_right(node)) {
		fputs("Unexpected exhaust of token stream", stderr);
		exit(1);
	}
}

void lookahead_concat(seek *tokenstream) {
	token *lookahead, *new;
	if (NULL != (lookahead = (token *) peek_right(tokenstream)) && lookahead->isNud) {
		new = (token *) calloc(1, sizeof(*new));
		if (NULL == new) {
			fputs("Failed to initialize buffer", stderr);
			exit(1);
		}
		new->type = CONCAT;
		new->precedence = PR_CONCAT;

		insert_node_right(tokenstream);
		set_right((void *) new, tokenstream->current);
	}
}

/* lac = "LookAhead control" */
void lookahead_epsilon(seek *tokenstream, char lac) {
	token *lookahead, *new;
	if (NULL != (lookahead = (token *) peek_right(tokenstream)) && lookahead->type == lac) {
		new = (token *) calloc(1, sizeof(*new));
		if (NULL == new) {
			fputs("Failed to initialize buffer", stderr);
			exit(1);
		}
		new->type = SYMBOL;
		new->precedence = PR_LOWEST;
		new->isNud = 1;
		new->content = EPSILON;

		insert_node_right(tokenstream);
		set_right((void *) new, tokenstream->current);
	}
}

/* parses the supplied tokenstream with the pratt parser method. it is supposed to be initialized with the lowest
 * existing precedence */
regexNode *parse(seek *tokenstream, char precedence) {
	regexNode *left;
	token *tkn = peek(tokenstream);

    char errType;
	if (!tkn->isNud) {

        switch (tkn->type) {
            case SYMBOL:
                errType = tkn->content;
                break;
            case BACKSLASH:
                errType = '\\';
                break;
            case CONCAT:
                errType = '~';
                break;
            default:
                errType = tkn->type;
        }

		fprintf(stderr, "Unexpected token in input stream: %c", errType);
		exit(1);
	}

	/* at first, we check for the null denotations */
	if (tkn->type != '(' && tkn->type != '[') {
		lookahead_concat(tokenstream);
	}

	/* null denominators */
	switch (tkn->type) {
		case SYMBOL:
			left = symbol(tkn->content);
			advance(tokenstream);
			break;
		case '(':
			/* if the following token is a "|", we can assume that there has to be an EPSILON */
			lookahead_epsilon(tokenstream, '|');
			left = parseGroup(tokenstream);
			advance(tokenstream);
			break;
		case '[':
			puts("Parsing set...");
			advance(tokenstream);
			break;
		case BACKSLASH:
			left = parseEscaped(tkn->content);
            advance(tokenstream);
			break;
		default:
            switch (tkn->type) {
                case SYMBOL:
                    errType = tkn->content;
                    break;
                case BACKSLASH:
                    errType = '\\';
                    break;
                case CONCAT:
                    errType = '~';
                    break;
                default:
                    errType = tkn->type;
            }

            fprintf(stderr, "Unexpected token in input stream: %c", errType);
            exit(1);
	}

	/* left denominators */
	token *next_token;
	if (NULL == (next_token = (token *) peek(tokenstream))) {
		return left;
	}

	while (precedence < next_token->precedence) {
		switch (next_token->type) {
			case '*':
				lookahead_concat(tokenstream);
				left = kleene(left);
				advance(tokenstream);
				break;
			case '+':
				lookahead_concat(tokenstream);
				left = concat(left, kleene(copyTree(left)));
				advance(tokenstream);
				break;
			case '{':
				puts("Parsing quantifier");
				break;
			case '|':
				/* if the following token is a ")" or EOF, we can assume that there has to be an EPSILON */
				lookahead_epsilon(tokenstream, ')');
				left = parseUnion(left, tokenstream);
				break;
			case CONCAT:
				left = parseConcat(left, tokenstream);
				break;
            default:
                switch (tkn->type) {
                    case SYMBOL:
                        errType = tkn->content;
                        break;
                    case BACKSLASH:
                        errType = '\\';
                        break;
                    case CONCAT:
                        errType = '~';
                        break;
                    default:
                        errType = tkn->type;
                }

                fprintf(stderr, "Unexpected token in input stream: %c", errType);
				exit(1);
		}

		if (NULL == (next_token = (token *) peek(tokenstream))) {
			break;
		}
	}

	return left;
}

regexNode *parseGroup(seek *tokenstream) {
	advance(tokenstream);
	regexNode *expression = parse(tokenstream, PR_LOWEST);

	token *next_token;
	if (NULL == (next_token = (token *) peek(tokenstream))) {
		fputs("Unexpected token type in input stream: EOF (Expected ')')", stderr);
		exit(1);
	}

	if (next_token->type != ')') {
		fprintf(stderr, "Unexpected token type in input stream: %c (Expected ')') ", next_token->type);
		exit(1);
	}

	lookahead_concat(tokenstream);
	return expression;
}

regexNode *parseEscaped(char escaped) {
	switch (escaped) {
		case 'd':
			return balancedTree(0, 9);
		case 'x':
			return union_re(balancedTree(0, 9), getHex());
		case 'w':
			return union_re(union_re(balancedTree(0, 9), getAlphanum()), symbol('_'));
		case 'n':
			return symbol(10);
		default:
			fprintf(stderr, "Unexpected escape character: \\%c\n", escaped);
            exit(1);
	}
}

regexNode *parseUnion(regexNode *restrict LHS, seek *restrict tokenstream) {
	advance(tokenstream);
	regexNode *RHS = parse(tokenstream, PR_UNION);
	return union_re(LHS, RHS);
}

regexNode *parseConcat(regexNode *restrict LHS, seek *restrict tokenstream) {
	advance(tokenstream);
	regexNode *RHS = parse(tokenstream, PR_CONCAT);
	return concat(LHS, RHS);
}
