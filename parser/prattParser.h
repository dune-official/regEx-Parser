#ifndef REGEX_PARSER_PRATTPARSER_H
#define REGEX_PARSER_PRATTPARSER_H

#include "../queue/queue.h"
#include "../stack/stack.h"
#include "../seekable/seekable.h"

#define BACKSLASH 0x6

typedef struct tokenStruct {
	char type, content;

	unsigned char precedence: 3;
	unsigned char isNud: 1;
} token;

enum Precedence {
	LOWEST_PR,
	UNION_PR,
	CONCAT_PR,
	KLEENE_PR
};

extern regexNode *parse(seek *tokenstream, char precedence);
extern void advance(seek *tokenstream);

extern regexNode *parseUnion(seek *tokenstream);
extern regexNode *parseGroup(seek *tokenstream);
extern regexNode *parseEscaped(seek *tokenstream, char escaped);
regexNode *parseConcat(regexNode *restrict LHS, seek *restrict node);

#endif
