#include "processPattern.h"

seek *tokenize(const char *inputString, char length) {
	seek *tokenStream = initialize_seekable();
	token *curToken;

	int i;
	for (i = 0; i < length; i++) {

		/* initialize token stream */
		curToken = (token *) calloc(1, sizeof(*curToken));
		if (NULL == curToken) {
			fputs("Failed to initialize buffer", stderr);
			exit(1);
		}

		if (inputString[i] == '\\') {

			if (i + 1 == length) {
				fputs("Expected token, got: EOF", stderr);
				exit(1);
			}

			i++;
			switch (inputString[i]) {
				/* insert as many more special chars as you please */
				case 'd':
				case 'D':
				case 'l':
				case 'u':
				case 'v':
				case 'h':
				case 'n':
				case 'N':
				case 'r':
				case 't':
				case 'a':
				case 'b':
				case 'e':
				case 'f':
				case 'k':
				case 'x':  // my own ( matches hexadecimal chars 😁 )
				case 'X':
				case 'w':
				case 'W':
				case 's':
				case 'S':
				case 'R':
				case '0':
				case 'E':
				case 'Q':
					curToken->type = BACKSLASH;
					curToken->content = inputString[i];
					curToken->precedence = PR_UNION;
					curToken->isNud = 1;
					break;

					/* this one is a bit more special... the backslash before a default character means,
					 * that the lexer is supposed to skip the current token in the input, which is not in the capabilities
					 * of a normal DFA */
				default:
					curToken->type = SYMBOL;
					curToken->content = inputString[i];
					curToken->precedence = PR_LOWEST;
					curToken->isNud = 1;
			}
			/* insert into seeker */
			if (i > 1) {
				insert_node_right(tokenStream);
				seek_right(tokenStream);
			}

			set_current((void *) curToken, tokenStream->current);
			continue;
		}

		curToken->type = inputString[i];
		switch (inputString[i]) {
			case '[':
				curToken->precedence = PR_UNION;
				curToken->isNud = 1;
				break;
			case '^':
			case '$':
			case '(':
				curToken->precedence = PR_LOWEST;
				curToken->isNud = 1;
				break;
			case '{':
			case ')':
			case '>':
			case ']':
			case '}':
			case '-':
			case ',':
			case '<':
				curToken->precedence = PR_LOWEST;
				break;
			case '|':
			case '?':
				curToken->precedence = PR_UNION;
				break;
			case '*':
			case '+':
				curToken->precedence = PR_KLEENE;
				break;

			default:
				curToken->type = SYMBOL;
				curToken->content = inputString[i];
				curToken->precedence = PR_LOWEST;
				curToken->isNud = 1;
				break;
		}

		/* insert into seeker */
		if (i > 0) {
			insert_node_right(tokenStream);
			seek_right(tokenStream);
		}

		set_current((void *) curToken, tokenStream->current);
	}

	tokenStream->current = tokenStream->start;
	return tokenStream;
}