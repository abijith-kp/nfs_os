#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "avl.h"

int cmp_str(void *a, void *b)
{
    return strcmp((char *)a, (char *)b);
}

int cmp_int(void *a, void *b)
{
    int p = *(int *)a;
    int q = *(int *)b;
    return p - q;
}

//NODE *create_node(void *data, void *key, int (*cmp)(void *a, void *b), void (*delete_funct)(void *node))
NODE *create_node(void *data, int key, int (*cmp)(void *a, void *b), void (*delete_funct)(void *node))
{
    NODE *node = calloc(1, sizeof(NODE));
    node->data = data;
    node->key = key;
    node->right = NULL;
    node->left = NULL;
    node->parent = NULL;
    node->height = 1;
    node->delete_funct = delete_funct;
    node->cmp = cmp;
    return node;
}

void free_tree(NODE *root)
{
    if (!root)
        return;

    free_tree(root->left);
    free_tree(root->right);
    
    if (root->delete_funct)
        root->delete_funct(root->data);
    // free(root->key);
    free(root);
}

void inorder(NODE *root)
{
    if (!root)
        return;

    inorder(root->left);
    // printf("(%d %d %p) >> %p %p %p\n", *((int *)root->key), get_height(root), root, root->left, root->parent, root->right);
    printf("(%d %d %p) >> %p %p %p\n", root->key, get_height(root), root, root->left, root->parent, root->right);
    inorder(root->right);
}

int get_node_height(NODE *node)
{
    int h = 0;
    if (!node)
        return 0;
    
    if (node->left)
        h = node->left->height;
    if ((node->right) && (h < node->right->height))
        h = node->right->height;

    return h + 1;
}

NODE *left_rotate(NODE *node)
{
    if ((!node) || (!(node->right)))
        return node;

    NODE *tmp_head = node->right;
    tmp_head->parent = node->parent;
    node->parent = tmp_head;
    node->right = tmp_head->left;
    if (node->right)
        node->right->parent = node;
    tmp_head->left = node;

    node->height = get_height(node);
    tmp_head->height = get_height(tmp_head);

    return tmp_head;
}

NODE *right_rotate(NODE *node)
{
    if ((!node) || (!(node->left)))
        return node;

    NODE *tmp_head = node->left;
    tmp_head->parent = node->parent;
    node->left = tmp_head->right;
    if (node->left)
        node->left->parent = node;
    tmp_head->right = node;
    node->parent = tmp_head;

    node->height = get_height(node);
    tmp_head->height = get_height(tmp_head);

    return tmp_head;
}

int get_height(NODE *root)
{
    if (!root)
        return 0;

    int l = get_height(root->left);
    int r = get_height(root->right);
    return MAX(l, r) + 1;
}

// NODE *__find_node_equal(NODE *root, void *key)
NODE *__find_node_equal(NODE *root, int key)
{
    if (!root)
        return NULL;
    // else if (root->cmp(root->key, key) == 0)
    else if (root->key == key)
        return root;
    // else if (root->cmp(root->key, key) > 0)
    else if (root->key > key)
        return __find_node_equal(root->left, key);
    return __find_node_equal(root->right, key);
}

NODE *__find_node_smallest(NODE *root)
{
    if (!root)
        return NULL;

    while (root->left)
        root = root->left;
   return root; 
}

NODE *__find_node_largest(NODE *root)
{
    if (!root)
        return NULL;

    while (root->right)
        root = root->right;
    return root;
}

// void *lookup_avl(NODE *root, void *key)
void *lookup_avl(NODE *root, int key)
{
    NODE *node = __find_node_equal(root, key);
    if (node)
        return node->data;
    return NULL;
}

// void *delete_avl(NODE **root_ptr, void *key)
void *delete_avl(NODE **root_ptr, int key)
{
    NODE *root = *root_ptr;

    NODE *node = __find_node_equal(root, key);
    if (!node)
        return NULL;

    NODE *parent = node->parent;
    NODE *replacement_node = NULL;
    if ((!(node->left)) && (!(node->right)))
    {
        replacement_node = NULL;
        if (parent)
        {
            if (parent->left == node)
                parent->left = NULL;
            else
                parent->right = NULL;
            parent->height = get_height(parent);
            replacement_node = parent;
        }
        else
            *root_ptr = NULL;
    }
    else if ((!(node->left)) && (node->right))
    {
        if (parent)
        {
            if (parent->left == node)
                parent->left = node->right;
            else
                parent->right = node->right;
            parent->height = get_height(parent);
        }
        else
            *root_ptr = node->right;
        node->right->parent = parent;
        // replacement_node = node->right;
        replacement_node = parent;
    }
    else if ((node->left) && (!(node->right)))
    {
        if (parent)
        {
            if (parent->left == node)
                parent->left = node->left;
            else
                parent->right = node->left;
            parent->height = get_height(parent);
        }
        else
            *root_ptr = node->left;
        node->left->parent = parent;
        replacement_node = node->left;
    }
    else
    {
        replacement_node = __find_node_smallest(node->right);
        if (!replacement_node)
            replacement_node = __find_node_largest(node->left);

        if (replacement_node->left)
        {
            if (replacement_node->parent->left == replacement_node)
                replacement_node->parent->left = replacement_node->left;
            else
                replacement_node->parent->right = replacement_node->left;
        }
        else if (replacement_node->right)
        {
            if (replacement_node->parent->left == replacement_node)
                replacement_node->parent->left = replacement_node->right;
            else
                replacement_node->parent->right = replacement_node->right;
        }
        else
        {
            if (replacement_node->parent->left == replacement_node)
                replacement_node->parent->left = NULL;
            else
                replacement_node->parent->right = NULL;
        }

        if (replacement_node->parent)
            replacement_node->parent->height = get_height(replacement_node->parent);

        replacement_node->left = node->left;
        replacement_node->right = node->right;
        replacement_node->parent = node->parent;
        replacement_node->height = get_height(replacement_node);
        if (node->left)
            node->left->parent = replacement_node;
        if (node->right)
            node->right->parent = replacement_node;

        if (!(node->parent))
            *root_ptr = replacement_node;
        else
        {
            if (node->parent->left == node)
                node->parent->left = replacement_node;
            else
                node->parent->right = replacement_node;
        }
    }

    while (replacement_node)
    {
        int l = get_height(replacement_node->left);
        int r = get_height(replacement_node->right);
        replacement_node->height = get_height(replacement_node);

        if (l - r == -2)
        {
            int rl = get_height(replacement_node->right->left);
            int rr = get_height(replacement_node->right->right);

            if (rr < rl)
                replacement_node->right = right_rotate(replacement_node->right);
            if (replacement_node->parent)
            {
                if (replacement_node->parent->left == replacement_node)
                    replacement_node->parent->left = left_rotate(replacement_node);
                else
                    replacement_node->parent->right = left_rotate(replacement_node);
            }
            else
                *root_ptr = left_rotate(replacement_node);
        }
        else if (l - r == 2)
        {
            int ll = get_height(replacement_node->left->left);
            int lr = get_height(replacement_node->left->right);

            /* weight on right */
            if (ll < lr)
                replacement_node->left = left_rotate(replacement_node->left);
            if (replacement_node->parent)
            {
                if (replacement_node->parent->left == replacement_node)
                    replacement_node->parent->left = right_rotate(replacement_node);
                else
                    replacement_node->parent->right = right_rotate(replacement_node);
            }
            else
                *root_ptr = right_rotate(replacement_node);
        }

        replacement_node->height = get_height(replacement_node);
        replacement_node = replacement_node->parent;
    }

    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->height = 1;
    node->delete_funct = NULL;

    void *data = node->data;
    free(node);
    // free(node->key);
    return data;
}

// void insert_avl(NODE **root_ptr, void *data, void *key)
void insert_avl(NODE **root_ptr, void *data, int key)
{
    NODE *node = NEW_NODE(data, key);
    NODE *root = *root_ptr;
    NODE **place_holder = NULL;
    NODE *parent = NULL;

    if (!root)
    {
        *root_ptr = node;
        return;
    }

    while (root)
    {
        parent = root;
        // if (root->cmp(root->key, key) > 0)
        if (root->key > key)
            root = root->left;
        else
            root = root->right;
    }

    node->parent = parent;
    // if (parent->cmp(parent->key, key) > 0)
    if (parent->key > key)
        parent->left = node;
    else
        parent->right = node;

    node = parent;
    while (node)
    {
        parent = node->parent;
        node->height = get_height(node);

        int diff = get_height(node->left) - get_height(node->right);

        if (diff == -2)
        {
            int rl = get_height(node->right->left);
            int rr = get_height(node->right->right);

            if (rr < rl)
                node->right = right_rotate(node->right);

            if (parent)
            {
                if (parent->left == node)
                    place_holder = &(parent->left);
                else
                    place_holder = &(parent->right);
            }
            else
                place_holder = root_ptr;

            *place_holder = left_rotate(node);
        }
        else if (diff == 2)
        {
            int ll = get_height(node->left->left);
            int lr = get_height(node->left->right);

            if (ll < lr)
                node->left = left_rotate(node->left);

            if (parent)
            {
                if (parent->left == node)
                    place_holder = &(parent->left);
                else
                    place_holder = &(parent->right);
            }
            else
                place_holder = root_ptr;

            *place_holder = right_rotate(node);
        }
        node = parent;
    }
}

void test(int argc, char *argv[])
{
    NODE *root = NULL;

    for (int i=1; i<argc; i++)
    {
        int *t = calloc(1, sizeof(int));
        *t = atoi(argv[i]);
        insert_avl(&root, NULL, *t);
        #ifdef DEBUG
        inorder(root);
        printf("height: %d\n", get_height(root));
        printf("------------------\n");
        #endif
    }

    inorder(root);
    printf("------------------\n");

    for (int i=1; i<argc; i++)
    {
        printf("deleting... %s \n", argv[i]);
        int *t = calloc(1, sizeof(int));
        *t = atoi(argv[i]);
        delete_avl(&root, *t);
        free(t);
        #ifdef DEBUG
        inorder(root);
        printf("height: %d\n", get_height(root));
        printf("------------------\n");
        #endif
    }

    free_tree(root);
}

/*
int main(int argc, char *argv[])
{
    test(argc, argv);
    return 0;
}
*/
