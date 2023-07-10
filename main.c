#include "regex/regex.h"
#include "parser/prattParser.h"
#include <time.h>

// x#include <fcntl.h>

int main_() {

	clock_t begin, end;
	double time_spent;

	char *pattern = "(0*|1*)";
    char *string = "0";

	printf("Pattern: %s\n", pattern);
	// regexNode *tree = parse(pattern);
	regexNode *tree = NULL;
	printf("Resulting regular expression tree: ");
	print_regExp(tree);
	putchar('\n');

	DFA **dfa = patternToDFA(tree);
    printDelta(dfa, 3);

	begin = clock();
	_Bool hasMatched = matchDFA(dfa, string, strnlen(string, 255));
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("Execution time: %fs\n", time_spent);

    // int *matchRange = matchAnyDFA(dfa, string, strnlen(string, 255));
    // int i, j = 0;

	// printf("Input: %s\n", string);
	printf("Matched: %s\n", hasMatched ? "true" : "false");

	return 0;
}

int main() {
	char *pattern = "ab";

	seek *node = initialize_seekable();

	token *firstToken = (token *) calloc(1, sizeof(*firstToken));
	if (NULL == firstToken) {
		fputs("Failed to initialize buffer", stderr);
		exit(1);
	}

	firstToken->type = SYMBOL;
	firstToken->content = 'c';
	firstToken->precedence = LOWEST_PR;
	firstToken->isNud = 1;

	token *secondToken = (token *) calloc(1, sizeof(*secondToken));
	if (NULL == secondToken) {
		fputs("Failed to initialize buffer", stderr);
		exit(1);
	}

	secondToken->type = SYMBOL;
	secondToken->content = 'b';
	secondToken->precedence = LOWEST_PR;
	secondToken->isNud = 1;

	token *thirdToken = (token *) calloc(1, sizeof(*thirdToken));
	if (NULL == thirdToken) {
		fputs("Failed to initialize buffer", stderr);
		exit(1);
	}

	thirdToken->type = SYMBOL;
	thirdToken->content = 't';
	thirdToken->precedence = LOWEST_PR;
	thirdToken->isNud = 1;

	/* the first object inserted is special */
	set_current((void *) firstToken, node->current);

	insert_node_right(node);
	seek_right(node);
	set_current((void *) secondToken, node->current);

	insert_node_right(node);
	seek_right(node);
	set_current((void *) thirdToken, node->current);

	node->current = node->start;

	regexNode *tree = parse(node, LOWEST_PR);
	print_regExp(tree);

	return 0;
}
