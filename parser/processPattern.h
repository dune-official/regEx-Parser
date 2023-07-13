#ifndef REGEX_PARSER_PROCESSPATTERN_H
#define REGEX_PARSER_PROCESSPATTERN_H

#include "../queue/queue.h"
#include "../stack/stack.h"
#include "../seekable/seekable.h"
#include "../seekable/seekable.h"

#define BACKSLASH 0x6

typedef struct tokenStruct {
	char type, content;

	unsigned char precedence: 3;
	unsigned char isNud: 1;
} token;

enum Precedence {
	PR_LOWEST,
	PR_UNION,
	PR_CONCAT,
	PR_QUANT,
	PR_KLEENE
};



extern seek* tokenize(const char *inputString, char length);

extern regexNode *parse(seek *tokenstream, char precedence);
extern void advance(seek *tokenstream);

extern regexNode *parseGroup(seek *tokenstream);
extern regexNode *parseEscaped(seek *tokenstream, char escaped);
regexNode *parseConcat(regexNode *restrict LHS, seek *restrict tokenstream);
regexNode *parseUnion(regexNode *restrict LHS, seek *restrict tokenstream);

#endif
