#include "prattParser.h"

regexNode *getDigit() {
	return NULL;
}

regexNode *getPrintable() {
	return getDigit();
}


/* Equals [0-9a-f] */
regexNode *getHex() {
	return getDigit();
}

void advance(seek *node) {
	if (-1 == seek_right(node)) {
		fputs("Unexpected exhaust of token stream", stderr);
		exit(1);
	}
}

// we supply a token stream
regexNode *parse(seek *node, char precedence) {
	token *lookahead, *new;
	regexNode *left;

	token *tkn = peek(node);
	if (!tkn->isNud) {
		fputs("Unexpected token in input stream: ", stderr);
		putc(tkn->type, stderr);
		exit(1);
	}

	/* at first, we check for the null denotations */

		/* but before that, we'll take the lookahead in the tokenstream and determine if it is also a null denotation,
		 * and if yes, we add a concat token */

		if (NULL != (lookahead = peek_right(node))) {
			if (lookahead->isNud) {
				new = (token *) calloc(1, sizeof(*new));
				if (NULL == new) {
					fputs("Failed to initialize buffer", stderr);
					exit(1);
				}
				new->type = CONCAT;
				new->precedence = CONCAT_PR;
				new->isNud = 1;

				insert_node_right(node);
				set_right((void *) new, node->current);
			}
		}

	switch (tkn->type) {
		case SYMBOL:
			left = symbol(tkn->content);
			advance(node);
			break;
		case '(':
			left = parseUnion(node);
			advance(node);
			break;
		case '<':
			left = parseGroup(node);
			advance(node);
			break;
		case '[':
			puts("Parsing set...");
			advance(node);
			break;
		case '@':
			puts("Pending implementation, skipping...");
			advance(node);
			parse(node, 1);
			break;
		case BACKSLASH:
			left = parseEscaped(node, tkn->content);
			advance(node);
			break;
		default:
			fputs("Unexpected token type in input stream: ", stderr);
			putc(tkn->type, stderr);
			exit(1);
	}

	token *next_token;
	if (NULL == (next_token = (token *) peek(node))) {
		return left;
	}

	while (precedence < next_token->precedence) {
		switch (next_token->type) {
			case '*':
				left = kleene(left);
				advance(node);
				break;
			case '+':
				left = concat(left, kleene(copyTree(left)));
			case '{':
				puts("Parsing quantifier");
				break;
			case CONCAT:
				left = parseConcat(left, node);
				break;
			default:
				fputs("Unexpected token type in input stream: ", stderr);
				putc(tkn->type, stderr);
				exit(1);
		}

		if (NULL == (next_token = (token *) peek(node))) {
			break;
		}
	}

	return left;
}

regexNode *parseUnion(seek *node) {
	advance(node);

	regexNode *LHS = parse(node, 1);
	token *next_token = peek(node);

	if (next_token->type != '|') {
		fprintf(stderr, "Expected '|', got %c\n", next_token->type);
		exit(1);
	}

	regexNode *RHS = parse(node, 1);
	next_token = peek(node);

	if (next_token->type != ')') {
		fprintf(stderr, "Expected ')', got %c\n", next_token->type);
		exit(1);
	}

	return union_re(LHS, RHS);
}

regexNode *parseGroup(seek *tokenstream) {
	return NULL;
}

regexNode *parseEscaped(seek *node, char escaped) {
	switch (escaped) {
		case 'd':
			return getDigit();
		case 'x':
			return getHex();
		case 'w':
			return getPrintable();
		case 'n':
			return symbol(10);
		default:
			fprintf(stderr, "Unexpected escape character: \\%c\n", escaped);
	}
	return NULL;
}

regexNode *parseConcat(regexNode *restrict LHS, seek *restrict node) {
	advance(node);
	regexNode *RHS = parse(node, CONCAT_PR);
	return concat(LHS, RHS);
}
