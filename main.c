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

#define SIZE 7

int main() {
	char *testSuite[SIZE] = {
			"(a|b|c|d|e|f)",
			"(7*e*|d*)l",
			"I'm down bad ngl",
			"a*bc|d*",
			"ye(a*|v)",
			"yeah (boi)*",
			// "(abc",
			// "wrong)",
			"colo(u|)r",
	};

	char *testSuiteMatch[SIZE] = {
			"b",
			"3",
			"I'm down bad ngl",
			"aaaaaaaaaabc",
			"yeaaaaaaa",
			"yeah boiboiboi",
			"color"
	};

	clock_t begin, end;
	double time_spent;

	regexNode *tree;
	seek *tokenStream;

	DFA **dfa;

	int i;
	for (i = 0; i < SIZE; i++) {
		begin = clock();
		tokenStream = tokenize(testSuite[i], strnlen(testSuite[i], 256));
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

		printf("Tokenizing \"%s\": %f\n", testSuite[i], time_spent);

		begin = clock();
		tree = parse(tokenStream, PR_LOWEST);
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

		printf("parsing \"%s\": %f\n", testSuite[i], time_spent);

		print_regExp(tree);
		puts("------");

		dfa = patternToDFA(tree);
	}

	return 0;
}
