#include "prattParser.h"
#include "../seekable/seekable.h"

seek* tokenize(const char *inputString, char length) {
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

        switch (inputString[i]) {
            case '(':
            case '[':
                curToken->type = inputString[i];
                curToken->precedence = PR_UNION;
                curToken->isNud = 1;
                break;
            case '<':
            case ')':
            case '>':
            case ']':
            case '{':
            case '}':
            case '|':
            case '@':
                curToken->type = inputString[i];
                curToken->precedence = PR_LOWEST;
                break;
            case '*':
            case '+':
                curToken->type = inputString[i];
                curToken->precedence = PR_KLEENE;
                break;

            case '\\':

                if (i + 1 == length) {
                    fputs("Expected token, got: EOF", stderr);
                    exit(1);
                }

                switch (inputString[i + 1]) {
                    /* insert as many more special chars as you please */
                    case 'd':
                    case 'x':
                    case 'w':
                        curToken->type = BACKSLASH;
                        curToken->content = inputString[i + 1];
                        curToken->precedence = PR_UNION;
                        curToken->isNud = 1;
                        break;

                    /* this one is a bit more special... the backslash before a default character means,
                     * that the lexer is supposed to skip the current token in the input, which usually
                     * not in the capabilities of a normal DFA */
                    default:
                        i++;
                        /* fml... for the love of god, this is the default case of the outer switch case (!)
                         * it just means that we won't check for anything else anymore and just append the string */
                        goto ESCAPED;
                }
                break;

            default:
                ESCAPED:
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
