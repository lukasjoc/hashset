#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <glib.h>
#include <gmodule.h>

int main() {

    GHashTable *table = g_hash_table_new(NULL, NULL);
    g_hash_table_insert(table, (gpointer)"a", (gpointer)100);
    g_hash_table_insert(table, (gpointer)"b", (gpointer)400);
    g_hash_table_insert(table, (gpointer)"c", (gpointer)300);

    gpointer key, value;
    GHashTableIter iter;

    g_hash_table_iter_init(&iter, table);

    while(g_hash_table_iter_next (&iter, &key, &value)) {
        if(value != NULL) {
            printf("%s : %d\n", (char *)(key), GPOINTER_TO_INT(value));
        }
    }


    g_hash_table_destroy(table);

    return EXIT_SUCCESS;
}

