#include <stdlib.h>

#include "avl.h"

struct reinvented_avl {
    size_t height;
    struct reinvented_avl* left;
    struct reinvented_avl* right;
    char payload[];
};

void* reinvented_avl_find(
    struct reinvented_avl* root,
    const void* key,
    reinvented_avl_cmp comparator)
{
    while (root) {
        int cmp = comparator(key, root->payload);
        if (cmp == 0) {
            return root->payload;
        } else if (cmp < 0) {
            root = root->left;
        } else {
            root = root->right;
        }
    }
    return 0;
}

static inline size_t height(struct reinvented_avl* node) {
    if (node) {
        return node->height;
    }
    return 0;
}

static inline size_t max(size_t lhs, size_t rhs) {
    return lhs < rhs ? rhs : lhs;
}

static inline void update_node_height(struct reinvented_avl* node) {
    if (node->left) {
        if (node->right) {
            node->height = max(node->left->height, node->right->height) + 1;
        } else {
            node->height = 2;
        }
    } else if (node->right) {
        node->height = 2;
    } else {
        node->height = 1;
    }
}

static void* insert(
    struct reinvented_avl** root,
    const void* key,
    reinvented_avl_cmp comparator,
    size_t payload_size,
    reinvented_avl_ctor constructor,
    _Bool* inserted);

/*
 * Two variants possible
 *       5          5          4
 *      / \        / \        / \
 *     4   D      3   D      3   5
 *    / \    or  / \    =>  /|   |\
 *   3   C      A   4      A B   C D
 *  / \            / \
 * A   B          B   C
 */
static inline void* insert_left(
    struct reinvented_avl** tree,
    const void* key,
    reinvented_avl_cmp comparator,
    size_t payload_size,
    reinvented_avl_ctor constructor,
    _Bool* inserted)
{
    struct reinvented_avl* root = *tree;
    size_t prev_height = height(root->left);
    void* payload = insert(
        &root->left,
        key,
        comparator,
        payload_size,
        constructor,
        inserted);
    size_t heightL = root->left->height;
    if (*inserted && heightL != prev_height) {
        size_t heightD = height(root->right);
        if (heightL - heightD > 1) {
            struct reinvented_avl* new_root;
            size_t heightLR = height(root->left->right);
            if (heightLR <= heightD) {
                /* left left case */
                /* new_root now points to 4 */
                new_root = root->left;
                /* 5 left now points to C */
                root->left = new_root->right;
                /* 4 right now points to 5 */
                new_root->right = root;
                /* heightLR is heightC */
                root->height = heightD + 1;
                new_root->height = root->height + 1;
            } else {
                /* left right case */
                /* new_root now points to 4 */
                new_root = root->left->right;
                /* 3 right now points to B */
                root->left->right = new_root->left;
                /* 4 left now points to 3 */
                new_root->left = root->left;
                /* 5 left now points to C */
                root->left = new_root->right;
                /* 4 right now points to 5 */
                new_root->right = root;
                root->height = max(heightD, height(root->left)) + 1;
                struct reinvented_avl* left = new_root->left;
                update_node_height(left);
                new_root->height = max(root->height, left->height) + 1;
            }
            /* 4 becomes root */
            *tree = new_root;
        } else {
            root->height = max(heightL, heightD) + 1;
        }
    }
    return payload;
}

/*
 * Two variants possible
 *   3          3              4
 *  / \        / \            / \
 * A   5      A   4          3   5
 *    / \  or    / \    =>  /|   |\
 *   4   D      B   5      A B   C D
 *  / \            / \
 * B   C          C   D
 */
static inline void* insert_right(
    struct reinvented_avl** tree,
    const void* key,
    reinvented_avl_cmp comparator,
    size_t payload_size,
    reinvented_avl_ctor constructor,
    _Bool* inserted)
{
    struct reinvented_avl* root = *tree;
    size_t prev_height = height(root->right);
    void* payload = insert(
        &root->right,
        key,
        comparator,
        payload_size,
        constructor,
        inserted);
    size_t heightR = root->right->height;
    if (*inserted && heightR != prev_height) {
        size_t heightA = height(root->left);
        if (heightR - heightA > 1) {
            struct reinvented_avl* new_root;
            size_t heightRL = height(root->right->left);
            if (heightRL <= heightA) {
                /* right right case */
                new_root = root->right;
                root->right = new_root->left;
                new_root->left = root;
                root->height = heightA + 1;
                new_root->height = root->height + 1;
            } else {
                /* rigth left case */
                new_root = root->right->left;
                root->right->left = new_root->right;
                new_root->right = root->right;
                root->right = new_root->left;
                new_root->left = root;
                root->height = max(heightA, height(root->right)) + 1;
                struct reinvented_avl* right = new_root->right;
                update_node_height(right);
                new_root->height = max(root->height, right->height) + 1;
            }
            *tree = new_root;
        } else {
            root->height = max(heightA, heightR) + 1;
        }
    }
    return payload;
}

static void* insert(
    struct reinvented_avl** root,
    const void* key,
    reinvented_avl_cmp comparator,
    size_t payload_size,
    reinvented_avl_ctor constructor,
    _Bool* inserted)
{
    if (!*root) {
        struct reinvented_avl* avl =
            malloc(sizeof(struct reinvented_avl) + payload_size);
        if (avl) {
            avl->height = 1;
            avl->left = 0;
            avl->right = 0;
            if (constructor(key, avl->payload)) {
                *root = avl;
                *inserted = 1;
                return avl->payload;
            }
        }
        free(avl);
        return 0;
    }
    int cmp = comparator(key, (*root)->payload);
    if (cmp == 0) {
        return (*root)->payload;
    } else if (cmp < 0) {
        return insert_left(
            root,
            key,
            comparator,
            payload_size,
            constructor,
            inserted);
    } else {
        return insert_right(
            root,
            key,
            comparator,
            payload_size,
            constructor,
            inserted);
    }
}

void* reinvented_avl_insert(
    struct reinvented_avl** root,
    const void* key,
    reinvented_avl_cmp comparator,
    size_t payload_size,
    reinvented_avl_ctor constructor)
{
    _Bool inserted = 0;
    return insert(root, key, comparator, payload_size, constructor, &inserted);
}

void* reinvented_avl_insert_flag(
    struct reinvented_avl** root,
    const void* key,
    reinvented_avl_cmp comparator,
    size_t payload_size,
    reinvented_avl_ctor constructor,
    _Bool* inserted)
{
    *inserted = 0;
    return insert(root, key, comparator, payload_size, constructor, inserted);
}

void reinvented_avl_destroy(
    struct reinvented_avl* root,
    reinvented_avl_dtor destructor,
    void* data)
{
    if (root) {
        reinvented_avl_destroy(root->left, destructor, data);
        reinvented_avl_destroy(root->right, destructor, data);
        destructor(data, root->payload);
        free(root);
    }
}

