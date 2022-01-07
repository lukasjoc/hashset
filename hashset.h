#ifndef HASHSET_H

#define _XOPEN_SOURCE 600

#define HASHSET_H

#include <glib.h>
#include <inttypes.h>
#include <stdbool.h>

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


void void_set_destroy(set *S);

void void_set_hashmap_value_destroy(gpointer data);

bool set_full(set *S);

set void_set_new(set *S, uint64_t length_max, GHashFunc hash_func,
                 GEqualFunc key_equal_func, GDestroyNotify key_destroy_func,
                 GDestroyNotify value_destroy_func);

uint64_t void_set_add(set *S, void *element);

uint64_t void_set_add_key_value(set *S, void *element, void *value);

uint64_t void_set_remove(set *S, void *element);

set void_set_remove_subset(set *S, set *T);

// Common Set Operations
bool void_set_contains(set *S, void *element);

uint64_t void_set_cardinality(set *S);

bool void_set_is_subset(set *S, set *T, bool strict);

set void_set_intersection(set *I, set *S, set *T);

set void_set_union(set *U, set *S, set *T);

set void_set_complement(set *Ac, set *S, set *T);

// ----

// Special Set Operations
set void_set_cartesian_product(set *SxT, set *S, set *T);
uint64_t void_set_cartesian_product_couples(couple *couples,
        uint64_t couple_length, set *SxT);

// ----


// Printing a set with set notation
void void_set_iter_print(set *S, char *set_name);

// Printing a Set with Couuples like produced by
// the cartesian product
void void_set_iter_print_int_couples(couple *couples,
                                     uint64_t couple_length, char *set_name);

#endif

