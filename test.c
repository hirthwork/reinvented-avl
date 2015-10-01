#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "avl.c"

int cmp(const void* key, const void* payload) {
    return ((int) key) - (*(int*) payload);
}

_Bool ctor(const void* key, void* placement) {
    *(int*) placement = (int) key;
    return 1;
}

_Bool bad_ctor(const void* key, void* placement) {
    (void) placement;
    fprintf(stderr, "ctor should never be called for %p\n", key);
    assert(0);
    return 0;
}

_Bool failed_ctor(const void* key, void* placement) {
    (void) key;
    (void) placement;
    return 0;
}

void counter(void* data, void* placement) {
    (void) placement;
    ++*(int*) data;
}

void print_tree(const struct reinvented_avl* root, size_t indent) {
    for (size_t i = 0; i < indent; ++i) {
        putc(' ', stderr);
    }
    if (root) {
        fprintf(
            stderr,
            "%zu %d %p %p %p\n",
            root->height,
            *(int*) root->payload,
            root,
            root->left,
            root->right);
        if (root->left || root->right) {
            print_tree(root->left, indent + 2);
            print_tree(root->right, indent + 2);
        }
    } else {
        puts("0 (nil)");
    }
}

size_t validate_tree_balance(const struct reinvented_avl* root) {
    if (root) {
        size_t left = validate_tree_balance(root->left);
        size_t right = validate_tree_balance(root->right);
        if (left > right + 1 || right > left + 1) {
            fprintf(
                stderr,
                "Tree is not balanced: left is %zu, right is %zu\n",
                left,
                right);
            print_tree(root, 0);
            assert(0);
        }
        return (left < right ? right : left) + 1;
    }
    return 0;
}

#define insert(elem, ctor)\
    reinvented_avl_insert(&t, elem, cmp, sizeof(int), ctor)
#define insert_flag(elem, ctor)\
    reinvented_avl_insert_flag(&t, elem, cmp, sizeof(int), ctor, &inserted)

int main() {
    struct reinvented_avl* t = 0;
    void* minus1 = (void*) -1;
    void* one = (void*) 1;
    void* two = (void*) 2;
    void* three = (void*) 3;
    void* four = (void*) 4;
    void* five = (void*) 5;
    void* six = (void*) 6;
    void* eight = (void*) 8;
    void* nine = (void*) 9;
    void* ten = (void*) 10;
    void* eleven = (void*) 11;
    void* twelve = (void*) 12;
    void* p13 = (void*) 13;
    void* p14 = (void*) 14;
    void* p15 = (void*) 15;
    void* p16 = (void*) 16;
    void* p17 = (void*) 17;
    void* p20 = (void*) 20;
    void* p21 = (void*) 21;
    void* p22 = (void*) 22;
    assert(*(int*) insert(five, ctor) == 5);
    assert(*(int*) insert(five, bad_ctor) == 5);
    assert(*(int*) insert(three, ctor) == 3);
    assert(*(int*) insert(four, ctor) == 4);
    assert(validate_tree_balance(t) == 2);
    assert(*(int*) reinvented_avl_find(t, five, cmp) == 5);
    assert(reinvented_avl_find(t, six, cmp) == 0);
    _Bool inserted = 0;
    assert(*(int*) insert_flag(eight, ctor) == 8);
    assert(inserted);
    assert(*(int*) insert_flag(eight, bad_ctor) == 8);
    assert(!inserted);
    assert(*(int*) insert(six, ctor) == 6);
    assert(validate_tree_balance(t) == 3);
    assert(reinvented_avl_find(t, 0, cmp) == 0);
    assert(*(int*) insert(one, ctor) == 1);
    assert(*(int*) insert(two, ctor) == 2);
    assert(validate_tree_balance(t) == 3);
    assert(*(int*) insert(0, ctor) == 0);
    assert(*(int*) insert(minus1, ctor) == -1);
    assert(validate_tree_balance(t) == 4);
    assert(*(int*) insert(nine, ctor) == 9);
    assert(*(int*) insert(ten, ctor) == 10);
    assert(validate_tree_balance(t) == 4);
    assert(*(int*) insert(eleven, ctor) == 11);
    assert(*(int*) insert(twelve, ctor) == 12);
    assert(validate_tree_balance(t) == 4);
    assert(*(int*) insert(p20, ctor) == 20);
    assert(*(int*) insert(p21, ctor) == 21);
    assert(*(int*) insert(p22, ctor) == 22);
    assert(validate_tree_balance(t) == 5);
    assert(*(int*) insert(p13, ctor) == 13);
    assert(*(int*) insert(p14, ctor) == 14);
    assert(*(int*) insert(p15, ctor) == 15);
    assert(*(int*) insert(p16, ctor) == 16);
    assert(*(int*) insert(p17, ctor) == 17);
    inserted = 1;
    assert(*(int*) insert_flag(p17, bad_ctor) == 17);
    assert(!inserted);
    assert(validate_tree_balance(t) == 5);
    print_tree(t, 0);
    int deletions = 0;
    reinvented_avl_destroy(t, counter, &deletions);
    assert(deletions == 21);

    t = 0;
    assert(*(int*) insert(five, ctor) == 5);
    assert(*(int*) insert(two, ctor) == 2);
    assert(*(int*) insert(six, ctor) == 6);
    assert(*(int*) insert(one, ctor) == 1);
    assert(*(int*) insert(three, ctor) == 3);
    assert(*(int*) insert(four, ctor) == 4);
    assert(validate_tree_balance(t) == 3);
    deletions = 0;
    reinvented_avl_destroy(t, counter, &deletions);
    assert(deletions == 6);

    t = 0;
    assert(*(int*) insert(0, ctor) == 0);
    assert(*(int*) insert(one, ctor) == 1);
    assert(*(int*) insert(four, ctor) == 4);
    assert(*(int*) insert(five, ctor) == 5);
    assert(*(int*) insert(three, ctor) == 3);
    assert(*(int*) insert(two, ctor) == 2);
    assert(validate_tree_balance(t) == 3);
    deletions = 0;
    reinvented_avl_destroy(t, counter, &deletions);
    assert(deletions == 6);

    t = 0;
    assert(insert(0, failed_ctor) == 0);
    assert(t == 0);

    size_t bad_size = SIZE_MAX - sizeof(struct reinvented_avl);
    assert(reinvented_avl_insert(&t, one, cmp, bad_size, bad_ctor) == 0);
    assert(t == 0);
}

