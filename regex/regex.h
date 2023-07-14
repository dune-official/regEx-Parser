#ifndef REGEX_PARSER_REGEX_H
#define REGEX_PARSER_REGEX_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../dfa/DFA.h"
// #include "../queue/queue.h"

/* RegEx types */
#define SYMBOL 0x1
#define UNION 0x2
#define KLEENE 0x3
#define CONCAT 0x4
#define POSITIVE 0x5

/* RegEx special char */
#define EMPTY 0x1e
#define EPSILON 0x1f

#define TENMEG 10485760
#define true 1
#define false 0

/* it is what it is sometimes */
#define reutner return
#define retutn return
#define reutnr return
#define retrun return

/* Szudzik-Functions */
#define S(x, y) ((x) >= (y) ? ((x) * (x)) + (x) + (y) : (y) * (y) + (x))
#define SMOD(x, y) ((x) >= (y) ? ((x) * (x)) + (x) + (y) : (y) * (y) + (x))

/* RegEx tree structure */
typedef struct regexNodeStruct {
	struct regexNodeStruct *LHS, *RHS;
	unsigned char type, symbol;
	unsigned long long hash;
} regexNode;

// typedef struct regexMatchResultStruct {
// 	struct regexMatchResultStruct *next;
// 	unsigned int from, to;
// 	char *string;
// } regexMatchResult;

/* regexNode constructors */
extern regexNode *getNode(unsigned char regexType);

extern regexNode *symbol(char symbol);

extern regexNode *union_re(regexNode *restrict LHS, regexNode *restrict RHS);

extern regexNode *concat(regexNode *restrict LHS, regexNode *restrict RHS);

extern regexNode *kleene(regexNode *child);

/* derivation functions */
extern _Bool isNullable(regexNode *rN);

extern regexNode *derive(regexNode *rN, char a);

/* match functions */
extern _Bool match(regexNode *restrict pattern, char *restrict string, int matchLen);
// extern regexMatchResult *matchAny(regexNode *pattern, char *string);

/* tree functions */
extern void print_regExp(const regexNode *root);

extern regexNode *copyTree(regexNode *child);
// extern regexNode *parse(char *string);

extern DFA **patternToDFA(regexNode *pattern);

#endif
