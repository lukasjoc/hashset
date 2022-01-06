#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "hashset.h"

int main () {
    set A;
    A = void_set_new(&A, 0, &g_direct_hash, &g_direct_equal, NULL, NULL);
    for(uint64_t i = 0; i < 100; i=i+20) {
        void_set_add(&A, (void *)i);
    }
    void_set_iter_print(&A, "A");


    set B;
    B = void_set_new(&B, 0, &g_direct_hash, &g_direct_equal, NULL, NULL);

    for(uint64_t i = 0; i < 50; i=i+10) {
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

