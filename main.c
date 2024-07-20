#include "header/parser.h"
#include <time.h>

#define SIZE 11

int main() {

    char *input = "this ain't no thing but a summer dance";
    char *pattern = "[ u']t";

    seek *tokenstream = lexer_tokenize(pattern, (char) strnlen(pattern, 256));

    regex_node *tree = parser_parse(tokenstream, PR_LOWEST);

    putchar('>');
    regex_print_regexp(tree);
    putchar('\n');

    dfa *dfa = regex_pattern_to_dfa(tree);
    matcher *all = matcher_match_all(dfa, input, (int) strnlen(input, 256));

    matcher_print_matches(all, input, false);

    return 0;
}

int main_() {
	char *test_suite[SIZE] = {
			"(a|b|c|d|e|f)",
			"(7*e*|d*)l",
			"I+",
			"a*bc|d*",
			"ye(a*|v)",
			"yeah (boi)*",
			"colo(u|)r",
            "\\x:\\x",
            "d[abcd]n",
            "d[a-f]t",
            "this"
	};

	char *test_suite_match[SIZE] = {
			"b",
			"3",
			"I'm down bad ngl",
			"aaaaaaaaaabc",
			"yeaaaaaaa",
			"yeah boiboiboi",
			"color",
            "7",
            "don",
            "dat",
            "this"
	};

	clock_t begin, end;
	double time_spent;

	regex_node *tree;
	seek *tokenstream;

	dfa *dfa;

	int i;
	for (i = 0; i < SIZE; i++) {
		begin = clock();
        tokenstream = lexer_tokenize(test_suite[i], (char) strnlen(test_suite[i], 256));
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

		printf("Tokenizing \"%s\": \x1b[32m%f\x1b[0m\n", test_suite[i], time_spent);

		begin = clock();
		tree = parser_parse(tokenstream, PR_LOWEST);
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

		printf("parsing \"%s\": \x1b[32m%f\x1b[0m\n", test_suite[i], time_spent);

        regex_print_regexp(tree);
        putchar('\n');

        begin = clock();
        dfa = regex_pattern_to_dfa(tree);
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        printf("pattern 2 dfa \"%s\": \x1b[32m%f\x1b[0m\n", test_suite[i], time_spent);

        begin = clock();
        _Bool matched = matcher_match_full(dfa, test_suite_match[i], (int) strnlen(test_suite_match[i], 256));
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        printf("regex_match \"%s\" with \"%s\" preemptively: \x1b[32m%f\x1b[0m\n", test_suite[i], test_suite_match[i], time_spent);

		puts("\x1b[31m------\x1b[0m");
	}

	return 0;
}
