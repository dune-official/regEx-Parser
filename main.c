#include "regex/regex.h"
#include "parser/processPattern.h"
#include <time.h>

// #include <fcntl.h>

#define SIZE 7

int main() {
	char *testSuite[SIZE] = {
			"(a|b|c|d|e|f)",
			"(7*e*|d*)l",
			"I'm down bad ngl",
			"a*bc|d*",
			"ye(a*|v)",
			"yeah (boi)*",
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

	dfa *dfa;

	int i;
	for (i = 0; i < SIZE; i++) {
		begin = clock();
		tokenStream = tokenize(testSuite[i], (char) strnlen(testSuite[i], 256));
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

		printf("Tokenizing \"%s\": %f\n", testSuite[i], time_spent);

		begin = clock();
		tree = parse(tokenStream, PR_LOWEST);
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

		printf("parsing \"%s\": %f\n", testSuite[i], time_spent);

        begin = clock();
        dfa = patternToDFA(tree);
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        printf("pattern 2 dfa \"%s\": %f\n", testSuite[i], time_spent);

        begin = clock();
        // _Bool matched = matchDFAPreemptive(dfa, testSuiteMatch[i], strnlen(testSuiteMatch[i],  256));
        _Bool matched = matchDFA(dfa, testSuiteMatch[i], (int) strnlen(testSuiteMatch[i],  256));
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        printf("match \"%s\" with \"%s\" preemptively: %f\n", testSuite[i], testSuiteMatch[i], time_spent);

		puts("------");
	}

	return 0;
}
