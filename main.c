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
    if(!set_full(S)) {
        g_hash_table_insert(S->elements, (gpointer)element, (gpointer)NULL);
        S->length = (uint64_t)g_hash_table_size(S->elements);
    }
    return S->length;
}

int main() {
    set A;
    A = void_set_new(&A, 0);

    void_set_add(&A, (void *)100);
    void_set_add(&A, (void *)200);

    printf("Length(MAX): %llu,\n", A.length_max);
    printf("Length(Actual): %llu,\n", A.length);

    return EXIT_SUCCESS;
}

