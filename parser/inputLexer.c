#include "processPattern.h"

void tokenizeEscaped(seek *restrict tokenStream,
                     token *restrict curToken, const char *restrict inputString, int *restrict pos, char len) {
    int i = pos[0];
    i++;
    switch (inputString[i]) {
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
        case 'x':  // my own ( matches hexadecimal chars )
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
            curToken->symbol = inputString[i];
            curToken->precedence = PR_UNION;
            curToken->isNud = 1;
            break;

        /* this one is a bit more special... the backslash before a default character means,
         * that the lexer is supposed to skip the current token in the input, which is not in the capabilities
         * of a normal DFA */
        default:
            curToken->type = SYMBOL;
            curToken->symbol = inputString[i];
            curToken->precedence = PR_LOWEST;
            curToken->isNud = 1;
    }

    /* insert into seeker */
    if (i > 1) {
        insert_node_right(tokenStream);
        seek_right(tokenStream);
    }

    set_current((void *) curToken, tokenStream->current);

    pos[0] = i;
}

void tokenizeSet(seek *restrict tokenStream,
                 token *restrict curToken, const char *restrict inputString, int *restrict pos, char len) {
    int i = pos[0];
    i++;

    if (i > 0) {
        insert_node_right(tokenStream);
        seek_right(tokenStream);
    }

    set_current((void *) curToken, tokenStream->current);

    for (; i < len; i++) {

        if (']' == inputString[i]) {
            pos[0] = i - 1;
            return;
        }

        /* initialize token */
        curToken = (token *) calloc(1, sizeof(*curToken));
        if (NULL == curToken) {
            fputs("Failed to initialize buffer", stderr);
            exit(1);
        }

        if (inputString[i] == '\\') {

            if (i + 1 == len) {
                fputs("Expected token, got: EOF", stderr);
                exit(1);
            }

            tokenizeEscaped(tokenStream, curToken, inputString, &i, len);
            continue;
        }

        curToken->type = inputString[i];
        curToken->precedence = PR_LOWEST;

        switch (inputString[i]) {
            case '-':
                break;
            case '^':
                curToken->isNud = 1;
                break;
            default:
                curToken->type = SYMBOL;
                curToken->symbol = inputString[i];
                curToken->isNud = 1;
        }

        /* insert into seeker */
        if (i > 0) {
            insert_node_right(tokenStream);
            seek_right(tokenStream);
        }

        set_current((void *) curToken, tokenStream->current);
    }

    fputs("Expected token ']', got: EOF", stderr);
    exit(1);
}

seek *tokenize(const char *inputString, char length) {
    /* initialize token stream */
    seek *tokenStream = initialize_seekable();
	token *curToken;

	int i;
	for (i = 0; i < length; i++) {

		curToken = (token *) calloc(1, sizeof(*curToken));
		if (NULL == curToken) {
			fputs("Failed to initialize buffer", stderr);
			exit(1);
		}

        /* Escaped character */
		if (inputString[i] == '\\') {

			if (i + 1 == length) {
				fputs("Expected token, got: <EOF>", stderr);
				exit(1);
			}

            tokenizeEscaped(tokenStream, curToken, inputString, &i, length);
            continue;
		}

		curToken->type = inputString[i];
		switch (inputString[i]) {
			case '[':
				curToken->precedence = PR_UNION;
				curToken->isNud = 1;
                tokenizeSet(tokenStream, curToken, inputString, &i, length);
                continue;
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
				curToken->symbol = inputString[i];
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