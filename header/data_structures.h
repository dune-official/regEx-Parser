#ifndef REGEX_PARSER_DATA_STRUCTURES_H
#define REGEX_PARSER_DATA_STRUCTURES_H

typedef struct matcher {
    unsigned int from;
    unsigned int to;

    struct matcher *next;
} matcher;

typedef struct DFA_state {
    bool is_final, is_dead;

    struct DFA_state *alphabet[94];
} dfa_state;

typedef struct DFA_wrapper {
    struct DFA_state *start;
    int state_count;
} dfa;

/* RegEx tree structure */
typedef struct regexNodeStruct {
    struct regexNodeStruct *LHS, *RHS;
    unsigned char type, symbol;
    unsigned long long hash;
} regex_node;

struct SeekableNode {
    void *object;
    struct SeekableNode *next, *previous;
};

typedef struct SeekableWrapper {
    struct SeekableNode *start, *current;
} seek;

typedef struct AuxiliaryNodeQ {
    struct QueueElement *queue_start, *queue_end;
} queue;

typedef struct tokenStruct {
    char type, symbol;

    unsigned char precedence: 3;
    unsigned char is_nud: 1;
} token;

typedef struct hash_root {
    unsigned int (*hashfktn)(unsigned int, unsigned int);
    unsigned int bucket_size;
    struct hash_node **bucket;
} hashmap;

struct hash_node {
    unsigned long long key;

    regex_node *tree;
    dfa_state *dfa_state;

    struct hash_node *next;
};

typedef dfa pattern;

#endif
