#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#include "hashset.h"

void void_set_destroy(set *S) {
    g_hash_table_destroy(S->elements);
    S->length = 0;
}

void void_set_hashmap_value_destroy(gpointer data) {
    g_hash_table_destroy(data);
}

uint64_t void_set_cardinality(set *S) {
    return S->length;
}

bool set_full(set *S) {
    return (bool)((S->length && S->length_max)
                  && (S->length >= S->length_max));
}

bool void_set_contains(set *S, void *element) {
    return (bool)g_hash_table_contains(S->elements, (gconstpointer)element);
}

set void_set_new(set *S, uint64_t length_max, GHashFunc hash_func,
                 GEqualFunc key_equal_func, GDestroyNotify key_destroy_func,
                 GDestroyNotify value_destroy_func) {

    S->elements = g_hash_table_new_full(hash_func, key_equal_func,
                                        key_destroy_func, value_destroy_func);

    S->length_max = length_max;

    g_hash_table_iter_init(&S->iter, S->elements);

    S->length = 0;

    return *S;
}

uint64_t void_set_add(set *S, void *element) {
    if(!set_full(S) && !void_set_contains(S, element)) {
        g_hash_table_insert(S->elements, (gpointer)element, (gpointer)NULL);
        S->length = (uint64_t)g_hash_table_size(S->elements);
    }

    return S->length;
}

uint64_t void_set_add_key_value(set *S, void *element, void *value) {
    if(!set_full(S) && !void_set_contains(S, element)) {
        g_hash_table_insert(S->elements, (gpointer)element, (gpointer)value);
        S->length = (uint64_t)g_hash_table_size(S->elements);
    }

    return S->length;
}

uint64_t void_set_remove(set *S, void *element) {
    if(!set_full(S)
            && g_hash_table_contains(S->elements, (gconstpointer)element)) {
        g_hash_table_remove(S->elements, (gconstpointer)element);
        S->length = (uint64_t)g_hash_table_size(S->elements);
    }

    return S->length;
}

bool void_set_is_subset(set *S, set *T, bool strict) {
    int c = 0;
    g_hash_table_iter_init(&S->iter, S->elements);
    while(g_hash_table_iter_next(&S->iter, &S->key, &S->value)) {
        if(g_hash_table_contains(T->elements, S->key)) c++;
    }
    return strict ? c==S->length && c==T->length : c==S->length;
}

set void_set_remove_subset(set *S, set *T) {
    if(!void_set_is_subset(S, T, false)) {
        return *T;
    }

    g_hash_table_iter_init(&S->iter, S->elements);
    while(g_hash_table_iter_next(&S->iter, &S->key, &S->value)) {
        void_set_remove(T, S->key);
    }

    return *T;
}

set void_set_intersection(set *I, set *S, set *T) {
    g_hash_table_iter_init(&S->iter, S->elements);
    while(g_hash_table_iter_next(&S->iter, &S->key, &S->value)) {
        if(g_hash_table_contains(T->elements, S->key)) {
            void_set_add(I, S->key);
        }
    }
    return *I;
}

set void_set_union(set *U, set *S, set *T) {

    g_hash_table_iter_init(&S->iter, S->elements);
    while(g_hash_table_iter_next(&S->iter, &S->key, &S->value)) {
        void_set_add(U, S->key);
    }

    g_hash_table_iter_init(&T->iter, T->elements);
    while(g_hash_table_iter_next(&T->iter, &T->key, &T->value)) {
        void_set_add(U, T->key);
    }

    return *U;
}

set void_set_complement(set *Ac, set *S, set *T) {
    g_hash_table_iter_init(&S->iter, S->elements);
    while(g_hash_table_iter_next(&S->iter, &S->key, &S->value)) {
        if(!void_set_contains(T, S->key)) {
            void_set_add(Ac, S->key);
        }
    }

    return *Ac;
}

set void_set_cartesian_product(set *SxT, set *S, set *T) {
    g_hash_table_iter_init(&S->iter, S->elements);

    while(g_hash_table_iter_next(&S->iter, &S->key, &S->value)) {
        GHashTable *products = g_hash_table_new(NULL, NULL);
        g_hash_table_iter_init(&T->iter, T->elements);

        while(g_hash_table_iter_next(&T->iter, &T->key, &T->value)) {
            g_hash_table_insert(products, T->key, T->key);
        }

        void_set_add_key_value(SxT, S->key, (void *)products);
    }

    return *SxT;
}

uint64_t void_set_cartesian_product_couples(couple *couples,
        uint64_t couple_length, set *SxT) {

    GHashTableIter cartesian_product_iter;
    gpointer cp_key, cp_value;
    g_hash_table_iter_init(&cartesian_product_iter, SxT->elements);

    uint64_t l = 0;
    while(g_hash_table_iter_next(&cartesian_product_iter, &cp_key, &cp_value)
            && l <= couple_length) {
        GHashTableIter cartesian_product_values_iter;
        gpointer cpv_key, cpv_value;
        g_hash_table_iter_init(&cartesian_product_values_iter, cp_value);

        while(g_hash_table_iter_next(
                    &cartesian_product_values_iter, &cpv_key, &cpv_value)) {
            couple c = {cp_key, cpv_key};
            couples[l++] = c;
        }
    }

    return l;
}

void void_set_iter_print(set *S, char *set_name) {
    g_hash_table_iter_init(&S->iter, S->elements);
    printf("%s = { ", set_name);
    while(g_hash_table_iter_next(&S->iter, &S->key, &S->value)) {
        printf("%d, ", GPOINTER_TO_INT(S->key));
    }
    printf("}\n");
}

void void_set_iter_print_int_couples(couple *couples, uint64_t couple_length,
                                     char *set_name) {

    printf("AxB = { ");
    for(uint64_t i = 0 ; i < couple_length; i++) {
        printf("(%d, %d), ",
               GPOINTER_TO_INT(couples[i].a),
               GPOINTER_TO_INT(couples[i].b)
              );
    }
    printf("}\n");
}

