/** @file
 *
 * Reinvented AVL-tree implementation
 */

#ifndef __REINVENTED__AVL_H__
#define __REINVENTED__AVL_H__ 1

#include <stddef.h>

struct reinvented_avl;

/** Comparator function
 *
 * Returns a negative integer, zero, or a positive integer as the search key
 * passed in first argument is less than, equal to, or greater than node
 * payload passed in second argument
 *
 * @param key search key
 * @param payload node payload to compare key with
 * @return comparison result
 */
typedef int (*reinvented_avl_cmp)(const void* key, const void* payload);

/** Node constructor function
 *
 * Initializes new object at placement. It is expected that placement is
 * properly aligned and points to the area large enough to contain payload_size
 * bytes.
 *
 * @param key search key
 * @param placement memory area which will hold initialized object
 * @return 1 on successful completion, 0 otherwise
 */
typedef _Bool (*reinvented_avl_ctor)(const void* key, void* placement);

/** Node desstructor function
 *
 * Deiitializes new object at placement. This callback is responsible for
 * deallocation of structures pointed by placement. The placement itself should
 * be freed.
 *
 * @param data pointer to additional data which can be used by destructor
 * @param placement memory area which holds object to deinitialize
 */
typedef void (*reinvented_avl_dtor)(void* data, void* placement);

/** Non-modifying search function
 *
 * @param root tree root
 * @param key search key
 * @param comparator comparator function
 * @return pointer to node payload or 0 if there is no such element
 */
void* reinvented_avl_find(
    struct reinvented_avl* root,
    const void* key,
    reinvented_avl_cmp comparator);

/** Insertion function
 *
 * If tree already contains node which is equal to search key,
 * then no allocations will be performed, otherwise memory for node payload
 * will be allocated and constructor will be called in order to initialize it
 *
 * @param root tree root, may be modified, should point to 0 on empty tree
 * @param key search key, will be passed to constructor as first argument
 * @param comparator comparator function
 * @param payload_size number of bytes required for node payload
 * @param constructor payload initializing function
 * @return pointer to found or constructed node payload, 0 on ENOMEM or
 *  constructor error
 */
void* reinvented_avl_insert(
    struct reinvented_avl** root,
    const void* key,
    reinvented_avl_cmp comparator,
    size_t payload_size,
    reinvented_avl_ctor constructor);

/** Insertion function returning found/inserted flag
 *
 * In addition to all actions performed in previous method, this one will
 * return flag indicating if insertion really occured or existing node was
 * found in tree
 *
 * @param root tree root, may be modified, should point to 0 on empty tree
 * @param key search key, will be passed to constructor as first argument
 * @param comparator comparator function
 * @param payload_size number of bytes required for node payload
 * @param constructor payload initializing function
 * @param inserted will be set to 0 if existing node was found, 1 if insertion
 *  has taken place
 * @return pointer to found or constructed node payload, 0 on ENOMEM or
 *  constructor error
 */
void* reinvented_avl_insert_flag(
    struct reinvented_avl** root,
    const void* key,
    reinvented_avl_cmp comparator,
    size_t payload_size,
    reinvented_avl_ctor constructor,
    _Bool* inserted);

/** Tree deallocation function
 *
 * This function will traverse all nodes in post-order applying destructor to
 * all payloads and freeing memory used by nodes
 *
 * @param root tree root
 * @param destructor payload deinitializing function
 * @param data additional data used by destructor
 */
void reinvented_avl_destroy(
    struct reinvented_avl* root,
    reinvented_avl_dtor destructor,
    void* data);

#endif

