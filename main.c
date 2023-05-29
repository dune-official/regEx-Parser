#include "regex/regex.h"
#include <time.h>

#include <fcntl.h>

int main1() {

	int fh;

	if (-1 == (fh = open("test.txt", O_RDONLY))) {
		fputs("Failed to open file, please try again", stderr);
		exit(1);
	}

	char *string = (char *) calloc(TENMEG, sizeof(string));
	if (NULL == string) {
		fputs("Failed to init buffer", stderr);
		exit(1);
	}

	if (-1 == (read(fh, string, TENMEG))) {
		fputs("Failed to read file, please try again", stderr);
		exit(1);
	}

	clock_t begin, end;
	double time_spent;

	char *pattern = "(0|1)+";
	// char *string = "0800";

	printf("Pattern: %s\n", pattern);
	regexNode *tree = parse(pattern);
	printf("Resulting regular expression tree: ");
	print_regExp(tree);
	putchar('\n');

	begin = clock();
	_Bool hasMatched = match(tree, string, strnlen(string, 255));
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("Execution time: %fs\n", time_spent);

	// printf("Input: %s\n", string);
	printf("Matched: %s\n", hasMatched ? "true" : "false");

	return 0;
}

int main() {

	int fh;

	if (-1 == (fh = open("test.txt", O_RDONLY))) {
		fputs("Failed to open file, please try again", stderr);
		exit(1);
	}

	char *string = (char *) calloc(TENMEG, sizeof(string));
	if (NULL == string) {
		fputs("Failed to init buffer", stderr);
		exit(1);
	}

	if (-1 == (read(fh, string, TENMEG))) {
		fputs("Failed to read file, please try again", stderr);
		exit(1);
	}

	clock_t begin, end;
	double time_spent;

	char *pattern = "\\d+";

	printf("Pattern: %s\n", pattern);
	regexNode *tree = parse(pattern);
	printf("Resulting regular expression tree: ");
	print_regExp(tree);
	putchar('\n');

	begin = clock();
	DFA **dfa = patternToDFA(tree);
	_Bool hasMatched = matchDFA(dfa, string, TENMEG);

	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("Execution time: %fs\n", time_spent);

	// printf("Input: %s\n", string);
	printf("Matched: %s\n", hasMatched ? "true" : "false");

	return 0;
}