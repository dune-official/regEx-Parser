#ifndef REGEX_PARSER_ALLOC_S_H
#define REGEX_PARSER_ALLOC_S_H

#include "parser.h"

#define VALNUL(x) if (NULL == (x)) {fprintf(stderr, #x " in function %s is NULL, exiting!\n", __func__);exit(1);}

#define NEW(type, name, amount) \
    type *name = (type *) calloc(amount, sizeof(*name)); \
    VALNUL(name)

#define NEWSTRUCT(type, name) \
    name = (type *) calloc(1, sizeof(*name)); \
    VALNUL(name)

#endif
