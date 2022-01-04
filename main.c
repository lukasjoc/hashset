#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <glib.h>
#include <gmodule.h>
#include <inttypes.h>
#include <stdbool.h>

// the main definition of the mutable, unordered hashset
typedef struct {
    // Key is the set value and the value
    // on the Hashtable is jsut a dummy
    // this way the lookup is fast and
    // still easy to use
    // Keys: To Values
    // (Elements) : 0
    GHashTable *elements;

    // current size of elements
    uint64_t length;

    // max size in elements
    uint64_t length_max;
}set;

set void_set_new(set *S, uint64_t length_max);
set void_set_new(set *S, uint64_t length_max) {
    /*
     * TODO: pass over options for table
     * GHashFunc hash_func = !S->hash_func ? &g_direct_hash : S->hash_func;
      GEqualFunc equal_func = !S->key_equal_func ? &g_direct_equal : S->key_equal_func;
      GDestroyNotify key_destroy_func = !S->key_destroy_func
      ? NULL : S->key_destroy_func;
      GDestroyNotify value_destroy_func = !S->value_destroy_func
      ? NULL : S->value_destroy_func; */

    S->elements = g_hash_table_new(NULL, NULL);

    S->length_max = length_max;
    S->length = 0;

    return *S;
}

bool set_full(set *S);
bool set_full(set *S) {

    return ((S->length && S->length_max) && (S->length >= S->length_max))
        ? true 
        : false;
}

uint64_t void_set_add(set *S, void *element);
uint64_t void_set_add(set *S, void *element) {
    // printf("DEBUG: %d\n", GPOINTER_TO_INT((gpointer)element));
    if(!set_full(S)
            && !g_hash_table_contains(S->elements, (gconstpointer)element)) {
        g_hash_table_insert(S->elements, (gpointer)element, (gpointer)NULL);
        S->length = (uint64_t)g_hash_table_size(S->elements);
    }

    return S->length;
}

uint64_t void_set_remove(set *S, void *element);
uint64_t void_set_remove(set *S, void *element) {
    // printf("DEBUG: %d\n", GPOINTER_TO_INT((gpointer)element));
    if(!set_full(S)
            && g_hash_table_contains(S->elements, (gconstpointer)element)) {
        g_hash_table_remove(S->elements, (gconstpointer)element);
        S->length = (uint64_t)g_hash_table_size(S->elements);
    }

    return S->length;
}

bool void_set_contains(set *S, void *element);
bool void_set_contains(set *S, void *element) {

    return (bool)g_hash_table_contains(S->elements, (gconstpointer)element);
}

bool void_set_is_subset(set *S, set *T);
bool void_set_is_subset(set *S, set *T) {
    gpointer key,value;
    GHashTableIter iter;
    g_hash_table_iter_init(&iter, S->elements);

    bool is_subset = false;
    while(g_hash_table_iter_next (&iter, &key, &value)) {
        if(g_hash_table_contains(T->elements, key)){
            is_subset = true;
        }else {
            is_subset = false;
        }
    }

    return is_subset;
}

set void_set_remove_subset(set *S, set *T);
set void_set_remove_subset(set *S, set *T) {
    if(!void_set_is_subset(S, T)) {
        return *T;
    }

    gpointer key,value;
    GHashTableIter iter;
    g_hash_table_iter_init(&iter, S->elements);
    while(g_hash_table_iter_next (&iter, &key, &value)) {
        void_set_remove(T, key);
    }

    return *T;
}



int main() {
    set A;
    A = void_set_new(&A, 0);

    void_set_add(&A, (void *)100);
    void_set_add(&A, (void *)200);
    void_set_add(&A, (void *)300);

    set B;
    B = void_set_new(&B, 0);

    void_set_add(&B, (void *)100);
    void_set_add(&B, (void *)200);
    void_set_add(&B, (void *)300);
    void_set_add(&B, (void *)700);
    void_set_add(&B, (void *)2000);


    printf("A contains 200: %d\n", void_set_contains(&A, (void *)200));
    printf("A contains 1000: %d\n", void_set_contains(&A, (void *)1000));

    printf("B contains 700: %d\n", void_set_contains(&B, (void *)700));
    printf("B contains 1: %d\n", void_set_contains(&B, (void *)1));

    printf("A is subset of B: %d\n", void_set_is_subset(&A, &B));
    printf("B is subset of A: %d\n", void_set_is_subset(&B, &A));

    B = void_set_remove_subset(&A, &B);
    printf("B Set length after removing subset A: %llu\n", B.length);

    return EXIT_SUCCESS;
}

