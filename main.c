#include "regex/regex.h"
#include "parser/processPattern.h"
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
	char *testSuite[17] = {
			"mario",
			"a",
			"mario|luigi",
			"\\d",
			"\\|",
			"(ab)c",
			"<abc>",
			"@1",
			"a*v",
			"c+c",
			"[afd]",
			"[a-x1]",
			"[1a-x2]",
			"a{4, 7}",
			"(y){3, 5}c",
			"(ya){1,                }",
			"afd*{3}"
	};

	int i = 2;
	seek *tokenStream;
	tokenStream = tokenize(testSuite[i], strnlen(testSuite[i], 256));

	regexNode *tree = parse(tokenStream, PR_LOWEST);
	print_regExp(tree);

	return 0;
}
