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
	char *pattern = "cbt";
    seek *tokenStream = tokenize(pattern, strnlen(pattern, 256));

	regexNode *tree = parse(tokenStream, PR_LOWEST);
	print_regExp(tree);

	return 0;
}
