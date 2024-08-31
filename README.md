# About

This is a potent and fast RegEx parser written in pure C requiring nothing but the standard library. 
The speeds are comparable to those of PCRE and the code is being frequently updated to add its functionality.

This program incooperates a range of computer science principles such as parsing, recursion and the Szudjik function.

As of now, the functions ```pattern *matcher_get_pattern(char *string, char pattern_string_len)``` and the ```char *matcher_get_match(matcher *restrict match_object, char *restrict string)``` are the center of the programs functionality.
