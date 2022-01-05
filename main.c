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

    // current size of elements
    uint64_t length;

    // max size in elements
    uint64_t length_max;

} set;

// A Couple represents an ordered pair of values within
// a set
typedef struct {
    void *a;
    void *b;
} couple;

void void_set_destroy(set *S) {
    g_hash_table_destroy(S->elements);
    S->length = (uint64_t)g_hash_table_size(S->elements);
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

set void_set_new(set *S, uint64_t length_max);
set void_set_new(set *S, uint64_t length_max) {
    S->elements = g_hash_table_new(NULL, NULL);
    S->length_max = length_max;
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

set void_set_complement(set *Ac, set *S, set *T);
set void_set_complement(set *Ac, set *S, set *T) {
    gpointer key, value;
    GHashTableIter iter;

    g_hash_table_iter_init(&iter, S->elements);
    while(g_hash_table_iter_next(&iter, &key, &value)) {
        if(!void_set_contains(T, key)) {
            void_set_add(Ac, key);
        }
    }

    return *Ac;
}

set void_set_cartesian_product(set *SxT, set *S, set *T);
set void_set_cartesian_product(set *SxT, set *S, set *T) {
    gpointer key_S, value_S;
    gpointer key_T, value_T;

    GHashTableIter iterS;
    GHashTableIter iterT;

    g_hash_table_iter_init(&iterS, S->elements);

    while(g_hash_table_iter_next(&iterS, &key_S, &value_S)) {
        GHashTable *products = g_hash_table_new(NULL, NULL);
        g_hash_table_iter_init(&iterT, T->elements);

        while(g_hash_table_iter_next(&iterT, &key_T, &value_T)) {
            g_hash_table_insert(products, (gpointer)key_T, (gpointer)NULL);
        }

        void_set_add_key_value(SxT, key_S, (void *)products);
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

int main() {
    set A, B, C, D, E, F, G;

    A = void_set_new(&A, 0);
    void_set_add(&A, (void *)1);
    void_set_add(&A, (void *)2);
    void_set_add(&A, (void *)6);

    B = void_set_new(&B, 0);
    void_set_add(&B, (void *)1);
    void_set_add(&B, (void *)2);
    void_set_add(&B, (void *)4);


    C = void_set_new(&C, 0);
    C = void_set_intersection(&C, &A, &B);

    D = void_set_new(&D, 0);
    D = void_set_union(&D, &A, &B);

    E = void_set_new(&E, 0);
    E = void_set_complement(&E, &A, &B);

    F = void_set_new(&F, 0);
    F = void_set_cartesian_product(&F, &A, &B);

    printf("Set (A, Normal Set) Length: %" PRIu64 "\n", void_set_cardinality(&A));
    printf("Set (B, Normal Set) Length: %" PRIu64 "\n", void_set_cardinality(&B));
    printf("Set (C, Intersection) Length: %" PRIu64 "\n", void_set_cardinality(&C));
    printf("Set (D, Union) Length: %" PRIu64 "\n", void_set_cardinality(&D));
    printf("Set (E, Complement) Length: %" PRIu64 "\n", void_set_cardinality(&E));

    uint64_t couple_length = CP_COUPLE_LENGTH(void_set_cardinality(&A),
                             void_set_cardinality(&A));

    couple couples[((int)couple_length)];

    couple_length = void_set_cartesian_product_couples(couples,
                    couple_length, &F);

    printf("AxB = { ");
    for(uint64_t i = 0 ; i < couple_length; i++) {
        couple c = couples[i];
        printf("(%d, %d), ", GPOINTER_TO_INT(c.a), GPOINTER_TO_INT(c.b));
    }
    printf("}\n");
    // ---

    // Destroy Phase
    void_set_destroy(&A);
    void_set_destroy(&B);
    void_set_destroy(&C);
    void_set_destroy(&D);
    void_set_destroy(&E);
    void_set_destroy(&F);
    // ----

    return EXIT_SUCCESS;
}

