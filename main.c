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

    int c = 0;
    while(g_hash_table_iter_next (&iter, &key, &value)) {
        if(g_hash_table_contains(T->elements, key)) c++;
    }
    return c==S->length;
}

set void_set_remove_subset(set *S, set *T);
set void_set_remove_subset(set *S, set *T) {
    if(!void_set_is_subset(S, T)) {
        return *T;
    }

    gpointer key,value;
    GHashTableIter iter;

    g_hash_table_iter_init(&iter, S->elements);

    while(g_hash_table_iter_next(&iter, &key, &value)) {
        void_set_remove(T, key);
    }

    return *T;
}

set void_set_intersection(set *I, set *S, set *T);
set void_set_intersection(set *I, set *S, set *T) {
    gpointer key,value;
    GHashTableIter iter;

    g_hash_table_iter_init(&iter, S->elements);
    while(g_hash_table_iter_next (&iter, &key, &value)) {
        if(g_hash_table_contains(T->elements, key)) {
            void_set_add(I, key);
        }
    }

    return *I;
}

set void_set_union(set *U, set *S, set *T);
set void_set_union(set *U, set *S, set *T) {
    gpointer key_S, value_S;
    gpointer key_T, value_T;
    GHashTableIter iterS;
    GHashTableIter iterT;

    g_hash_table_iter_init(&iterS, S->elements);
    while(g_hash_table_iter_next(&iterS, &key_S, &value_S)) {
            void_set_add(U, key_S);
    }

    g_hash_table_iter_init(&iterT, T->elements);
    while(g_hash_table_iter_next(&iterT, &key_T, &value_T)) {
            void_set_add(U, key_T);
    }

    return *U;
}

set void_set_complement(set *AC, set *S, set *T);
set void_set_complement(set *AC, set *S, set *T) {
    gpointer key, value;
    GHashTableIter iter;

    g_hash_table_iter_init(&iter, S->elements);
    while(g_hash_table_iter_next(&iter, &key, &value)) {
            if(!void_set_contains(T, key)) {
                void_set_add(AC, key);
            }
    }

    return *AC;
}



int main() {
    set A, B, C, D, E;

    A = void_set_new(&A, 0);
    void_set_add(&A, (void *)1);
    void_set_add(&A, (void *)2);
    void_set_add(&A, (void *)3);
    void_set_add(&A, (void *)4);
    void_set_add(&A, (void *)5);

    B = void_set_new(&B, 0);
    void_set_add(&B, (void *)1);
    void_set_add(&B, (void *)2);
    void_set_add(&B, (void *)3);
    void_set_add(&B, (void *)10);

    C = void_set_new(&C, 0);
    C = void_set_intersection(&C, &A, &B);

    D = void_set_new(&D, 0);
    D = void_set_union(&D, &A, &B);

    E = void_set_new(&E, 0);
    E = void_set_complement(&E, &A, &B);

    printf("Set (A, Normal Set) Length: %llu\n", A.length);
    printf("Set (B, Normal Set) Length: %llu\n", B.length);
    printf("Set (C, Intersection) Length: %llu\n", C.length);
    printf("Set (D, Union) Length: %llu\n", D.length);
    printf("Set (E, Complement) Length: %llu\n", E.length);

    return EXIT_SUCCESS;
}

