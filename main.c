#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <glib.h>
#include <gmodule.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>

// |AxB| = |A| * |B|
#define CP_COUPLE_LENGTH(card_S, card_T) (card_S * card_T)

// the main definition of the mutable, unordered hashset
typedef struct {
    // All Set Elements
    GHashTable *elements;

    // GHashTable Config
    GHashFunc hash_func;
    GEqualFunc key_equal_func;
    GDestroyNotify key_destroy_func;
    GDestroyNotify value_destroy_func;
    GHashTableIter iter;
    gpointer key;
    gpointer value;

    // current size of elements
    uint64_t length;

    // max size in elements
    uint64_t length_max;
} set;

// A Couple represents an ordered pair of values within
// a set
typedef struct {
    void *a, *b;
} couple;

void void_set_destroy(set *S) {
    g_hash_table_destroy(S->elements);
    S->length = 0;
}

void void_set_hashmap_value_destroy(gpointer data) {
    g_hash_table_destroy(data);
}

uint64_t void_set_cardinality(set *S);
uint64_t void_set_cardinality(set *S) {
    return S->length;
}

bool set_full(set *S);
bool set_full(set *S) {
    return (bool)((S->length && S->length_max)
            && (S->length >= S->length_max));
}

bool void_set_contains(set *S, void *element);
bool void_set_contains(set *S, void *element) {
    return (bool)g_hash_table_contains(S->elements, (gconstpointer)element);
}

set void_set_new(set *S, uint64_t length_max, GHashFunc hash_func,
        GEqualFunc key_equal_func, GDestroyNotify key_destroy_func,
        GDestroyNotify value_destroy_func);

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

uint64_t void_set_add(set *S, void *element);
uint64_t void_set_add(set *S, void *element) {
    if(!set_full(S) && !void_set_contains(S, element)) {
        g_hash_table_insert(S->elements, (gpointer)element, (gpointer)NULL);
        S->length = (uint64_t)g_hash_table_size(S->elements);
    }

    return S->length;
}

uint64_t void_set_add_key_value(set *S, void *element, void *value);
uint64_t void_set_add_key_value(set *S, void *element, void *value) {
    if(!set_full(S) && !void_set_contains(S, element)) {
        g_hash_table_insert(S->elements, (gpointer)element, (gpointer)value);
        S->length = (uint64_t)g_hash_table_size(S->elements);
    }

    return S->length;
}

uint64_t void_set_remove(set *S, void *element);
uint64_t void_set_remove(set *S, void *element) {
    if(!set_full(S)
            && g_hash_table_contains(S->elements, (gconstpointer)element)) {
        g_hash_table_remove(S->elements, (gconstpointer)element);
        S->length = (uint64_t)g_hash_table_size(S->elements);
    }

    return S->length;
}

bool void_set_is_subset(set *S, set *T);
bool void_set_is_subset(set *S, set *T) {
    int c = 0;
    g_hash_table_iter_init(&S->iter, S->elements);
    while(g_hash_table_iter_next(&S->iter, &S->key, &S->value)) {
        if(g_hash_table_contains(T->elements, S->key)) c++;
    }
    return c==S->length;
}

set void_set_remove_subset(set *S, set *T);
set void_set_remove_subset(set *S, set *T) {
    if(!void_set_is_subset(S, T)) {
        return *T;
    }

    g_hash_table_iter_init(&S->iter, S->elements);
    while(g_hash_table_iter_next(&S->iter, &S->key, &S->value)) {
        void_set_remove(T, S->key);
    }

    return *T;
}

set void_set_intersection(set *I, set *S, set *T);
set void_set_intersection(set *I, set *S, set *T) {
    g_hash_table_iter_init(&S->iter, S->elements);
    while(g_hash_table_iter_next(&S->iter, &S->key, &S->value)) {
        if(g_hash_table_contains(T->elements, S->key)) {
            void_set_add(I, S->key);
        }
    }
    return *I;
}

set void_set_union(set *U, set *S, set *T);
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

set void_set_complement(set *Ac, set *S, set *T);
set void_set_complement(set *Ac, set *S, set *T) {
    g_hash_table_iter_init(&S->iter, S->elements);
    while(g_hash_table_iter_next(&S->iter, &S->key, &S->value)) {
        if(!void_set_contains(T, S->key)) {
            void_set_add(Ac, S->key);
        }
    }

    return *Ac;
}

set void_set_cartesian_product(set *SxT, set *S, set *T);
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

int main () {
    set A;
    A = void_set_new(&A, 0, &g_direct_hash, &g_direct_equal, NULL, NULL);
    for(uint64_t i = 0; i < 3000; i=i+20) {
        void_set_add(&A, (void *)i);
    }
    void_set_iter_print(&A, "A");


    set B;
    B = void_set_new(&B, 0, &g_direct_hash, &g_direct_equal, NULL, NULL);

    for(uint64_t i = 0; i < 500; i=i+10) {
        void_set_add(&B, (void *)i);
    }
    void_set_iter_print(&B, "B");


    set union_AB;
    union_AB = void_set_new(&union_AB, 0, &g_direct_hash, &g_direct_equal, NULL, NULL);
    union_AB = void_set_union(&union_AB, &A, &B);
    void_set_iter_print(&union_AB, "A∪B");


    set C;
    C = void_set_new(&C, 0, &g_direct_hash, &g_direct_equal,
            NULL, &void_set_hashmap_value_destroy);

    C = void_set_cartesian_product(&C, &A, &B);

    uint64_t couple_length = CP_COUPLE_LENGTH(void_set_cardinality(&A),
            void_set_cardinality(&B));

    couple couples[((int)couple_length)];
    couple_length = void_set_cartesian_product_couples(couples, couple_length, &C);

    void_set_iter_print_int_couples(couples, couple_length, "AxB");


    set complement_AB;
    complement_AB = void_set_new(&complement_AB, 0, &g_direct_hash,
                &g_direct_equal, NULL, NULL);
    complement_AB = void_set_complement(&complement_AB, &A, &B);
    void_set_iter_print(&complement_AB, "ACB");


    set intersection_AB;
    intersection_AB = void_set_new(&intersection_AB, 0, &g_direct_hash,
                &g_direct_equal, NULL, NULL);

    intersection_AB = void_set_intersection(&intersection_AB, &A, &B);
    void_set_iter_print(&intersection_AB, "A∩B");

    printf("A is subset of B %d\n", void_set_is_subset(&A, &B));
    printf("B is subset of A %d\n", void_set_is_subset(&B, &A));

    // ----
    void_set_destroy(&A);
    void_set_destroy(&B);
    void_set_destroy(&C);
    void_set_destroy(&union_AB);
    void_set_destroy(&complement_AB);
    void_set_destroy(&intersection_AB);

    return EXIT_SUCCESS;
}

