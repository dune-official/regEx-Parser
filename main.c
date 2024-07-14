#include "header/parser.h"
#include <time.h>

// #include <fcntl.h>

#define SIZE 10

int main() {
    regex_node *tree;
    seek *tokenstream;
    dfa *dfa;

    bool matched;

    char *input = "d[0-9a-f]m";
    char *input_match = "d9m";

    tokenstream = lexer_tokenize(input, (char) strnlen(input, 256));
    tree = parser_parse(tokenstream, PR_LOWEST);

    regex_print_regexp(tree);
    putchar('\n');

    dfa = regex_pattern_to_dfa(tree);

    matched = match_dfa(dfa, input_match, (int) strnlen(input_match, 256));
    printf("%s", matched == true ? "Matched" : "Not matched");

    return 0;
}

int main_() {
	char *testSuite[SIZE] = {
			"(a|b|c|d|e|f)",
			"(7*e*|d*)l",
			"I+",
			"a*bc|d*",
			"ye(a*|v)",
			"yeah (boi)*",
			"colo(u|)r",
            "\\x:\\x",
            "d[abcdefgh]n",
            "d[a-f]t"
	};

	char *testSuiteMatch[SIZE] = {
			"b",
			"3",
			"I'm down bad ngl",
			"aaaaaaaaaabc",
			"yeaaaaaaa",
			"yeah boiboiboi",
			"color",
            "7",
            "don",
            "dat"
	};

	clock_t begin, end;
	double time_spent;

	regex_node *tree;
	seek *tokenStream;

	dfa *dfa;

	int i;
	for (i = 0; i < SIZE; i++) {
		begin = clock();
		tokenStream = lexer_tokenize(testSuite[i], (char) strnlen(testSuite[i], 256));
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

		printf("Tokenizing \"%s\": \x1b[32m%f\x1b[0m\n", testSuite[i], time_spent);

		begin = clock();
		tree = parser_parse(tokenStream, PR_LOWEST);
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

		printf("parsing \"%s\": \x1b[32m%f\x1b[0m\n", testSuite[i], time_spent);

        regex_print_regexp(tree);
        putchar('\n');

        begin = clock();
        dfa = regex_pattern_to_dfa(tree);
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        printf("pattern 2 dfa \"%s\": \x1b[32m%f\x1b[0m\n", testSuite[i], time_spent);

        begin = clock();
        _Bool matched = match_dfa(dfa, testSuiteMatch[i], (int) strnlen(testSuiteMatch[i], 256));
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        printf("regex_match \"%s\" with \"%s\" preemptively: \x1b[32m%f\x1b[0m\n", testSuite[i], testSuiteMatch[i], time_spent);

		puts("\x1b[31m------\x1b[0m");
	}

	return 0;
}
