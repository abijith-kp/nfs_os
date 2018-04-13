#ifndef __AVL_LIB_H
 #define __AVL_LIB_H

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define NEW_NODE(data, key) create_node(data, key, cmp_int, NULL)

struct _node
{
    // void *data, *key;
    void *data;
    int key;
    int height;
    struct _node *right, *left, *parent;
    void (*delete_funct)(void *node);
    int (*cmp)(void *a, void *b);
};

typedef struct _node NODE;

// NODE *create_node(void *data, void *key, int (*cmp)(void *a, void *b), void (*delete_funct)(void *node));
NODE *create_node(void *data, int key, int (*cmp)(void *a, void *b), void (*delete_funct)(void *node));
void free_tree(NODE *root);
void inorder(NODE *root);
int get_node_height(NODE *node);
NODE *left_rotate(NODE *node);
NODE *right_rotate(NODE *node);
int get_height(NODE *root);
// NODE *__find_node_equal(NODE *root, void *key);
NODE *__find_node_equal(NODE *root, int key);
NODE *__find_node_smallest(NODE *root);
NODE *__find_node_largest(NODE *root);
// void *delete_avl(NODE **root_ptr, void *key);
void *delete_avl(NODE **root_ptr, int key);
// void insert_avl(NODE **root_ptr, void *data, void *key);
void insert_avl(NODE **root_ptr, void *data, int key);
// void *lookup_avl(NODE *root, void *key);
void *lookup_avl(NODE *root, int key);

#endif
