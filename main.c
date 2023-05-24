#include "regex.h"
#include <time.h>

int main() {

	clock_t begin, end;
	double time_spent;

	char *pattern = "(a|b)*";
	char *string = "abbbabacfewahqfeorwpnf4uio3pqnuv94qpnu4upqbabbbae";

	regexNode *tree = parse(pattern);
	print_regExp(tree);
	putchar('\n');

	begin = clock();
	_Bool hasMatched = match(tree, string);
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("%fs\n", time_spent);
	printf("%s\n", hasMatched ? "true" : "false");

	return 0;
}