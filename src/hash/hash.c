#include "../../header/parser.h"


/* This function applies a hash function to the key according to the bucket size.
 *
 * Input:
 * - struct hash_root *restrict hashmap: The hashmap to get the function from;
 * - unsigned int key: The unique key to hash;
 * Output:
 * - unsigned int hashed: The key hashed according to this hash function;
 */
unsigned int hash(struct hash_root *restrict hashmap, unsigned int key) {
    if (NULL != hashmap->hashfktn) {
        return (*hashmap->hashfktn)(hashmap->bucket_size, key);
    }
    return key % hashmap->bucket_size;
}

/* This function creates a hashmap. The bucket size is determined dynamically.
 *
 * Input:
 * - unsigned int bucket_size: The future bucket size of the hashmap;
 * Output:
 * - struct hash_root *root: The new hashmap;
 */
struct hash_root *hash_initialize(unsigned int bucket_size) {
    NEW(struct hash_root, root, 1)
    root->bucket_size = bucket_size;
    root->bucket = (struct hash_node **) calloc(bucket_size, sizeof(**root->bucket));
    VALNUL(root->bucket)

    unsigned int i;
    for (i = 0; i < bucket_size; i++) {
        root->bucket[i] = (struct hash_node *) calloc(1, sizeof(**root->bucket));
        VALNUL(root->bucket[i])
    }

    return root;
}


/* This function sets the hash function of the hashmap. If it is NULL, the default function will be used instead.
 *
 * Input:
 * - struct hash_root *restrict hashmap: The hashmap to add the function to;
 * - unsigned int (*hashfktn)(unsigned int, unsigned int): A function pointer with (unsigned int, unsigned int) arguments;
 */
void hash_set_hashfunction(struct hash_root *restrict hashmap, unsigned int (*hashfktn)(unsigned int, unsigned int)) {
    hashmap->hashfktn = hashfktn;
}

/* This function gets a node before the given key.
 *
 * Input:
 * - struct hash_root *restrict hashmap: The hashmap to get the node from;
 * - unsigned int key: The key of the value;
 * Output:
 * - struct hash_node *node: The node before the node with the key;
 */
struct hash_node *internal_hash_get_node_before(struct hash_root *restrict hashmap, unsigned int key) {
    unsigned int h = hash(hashmap, key);

    struct hash_node *node = hashmap->bucket[h];

    while (!(NULL == node->next || node->next->key == key)) {
        node = node->next;
    }

    return node;
}

/* This function gets a node with the given key.
 *
 * Input:
 * - struct hash_root *restrict hashmap: The hashmap to get the node from;
 * - unsigned int key: The key of the value;
 * Output:
 * - struct hash_node *node: The node with the key;
 */
struct hash_node *internal_hash_get_node(struct hash_root *restrict hashmap, unsigned int key) {
    return internal_hash_get_node_before(hashmap, key)->next;
}

/* This function inserts a value under a given key and informs the user about the insertion status.
 *
 * Input:
 * - struct hash_root *restrict hashmap: The hashmap to insert into;
 * - unsigned int key: The key of the value;
 * - void *restrict value: The value itself;
 * Output:
 * - bool status: true if the key has been inserted successfully;
 */
bool hash_insert(struct hash_root *restrict hashmap, unsigned int key, regex_node *restrict tree, dfa_state *restrict state) {

    struct hash_node *node_ptr = internal_hash_get_node_before(hashmap, key);

    if (NULL != node_ptr->next && node_ptr->next->key == key) {
        return false;
    }

    NEWSTRUCT(struct hash_node, node_ptr->next)

    node_ptr->next->key = key;
    node_ptr->next->tree = tree;
    node_ptr->next->dfa_state = state;

    return true;
}

/* This function gets a value with a given key and returns it.
 * Value is NULL if the key does not exist (the sentinel was selected).
 *
 * Input:
 * - struct hash_root *restrict hashmap: The hashmap to get the value from;
 * - unsigned int key: The key of the value;
 * Output:
 * - void *value: The value of the key (might be NULL);
 * */
regex_node *hash_get_tree(struct hash_root *restrict hashmap, unsigned int key) {
    struct hash_node *node = internal_hash_get_node(hashmap, key);
    if (NULL == node) {
        return NULL;
    }
    return node->tree;
}

dfa_state *hash_get_dfa(struct hash_root *restrict hashmap, unsigned int key) {
    struct hash_node *node = internal_hash_get_node(hashmap, key);
    if (NULL == node) {
        return NULL;
    }
    return node->dfa_state;
}

/* This function simply checks the existence of a key in the hashmap.
 *
 * Input:
 * - struct hash_root *restrict hashmap: The hashmap to insert into;
 * - unsigned int key: The key of the value;
 * Output:
 * - bool exists: true if the value exists;
 */
bool hash_exists(struct hash_root *restrict hashmap, unsigned int key) {
    struct hash_node *node = internal_hash_get_node(hashmap, key);
    if (NULL == node) {
        return NULL;
    }
    return NULL != node;
}

/* This internal function deletes the given hash node and restores the linked list.
 *
 * Input:
 * - struct hash_node *restrict node_before: The node BEFORE the node to delete;
 */
void internal_hash_delete(struct hash_node *restrict node_before) {
    struct hash_node *node_delete = node_before->next;
    if (NULL == node_delete) {
        return;
    }

    struct hash_node *node_after = node_delete->next;
    node_before->next = node_after;
    free(node_delete);
}

/* This function deletes an entry in the hashmap with the given key, but returns it as well.
 *
 * Input:
 * - struct hash_root *restrict hashmap: The hashmap to delete from;
 * - unsigned int key: The key of the value;
 * Output:
 * - void *value: The value of the key;
 */
void *hash_delete(struct hash_root *restrict hashmap, unsigned int key) {
    struct hash_node *delete_before = internal_hash_get_node_before(hashmap, key);
    if (NULL == delete_before->next) {
        return NULL;
    }

    void *value = delete_before->next->tree;
    internal_hash_delete(delete_before);
    return value;
}