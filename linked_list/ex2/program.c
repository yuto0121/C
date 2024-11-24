#define _GNU_SOURCE
/**
 * \author Bert Lagaisse
 *
 * main method that executes some test functions (without check framework)
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "dplist.h"

void ck_assert_msg(bool result, char *msg)
{
    if (!result)
        printf("%s\n", msg);
}

int main(void)
{
    dplist_t *strings = NULL;
    strings = dpl_create();

    ck_assert_msg(strings != NULL, "strings = NULL, List not created");
    ck_assert_msg(dpl_size(strings) == 0, "Strings may not contain elements.");

    dpl_insert_at_index(strings, "Hello", 0);
    ck_assert_msg(dpl_size(strings) == 1, "Strings must contain 1 element.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(strings, 0), "Hello") == 0, "First element must be 'Hello'.");

    dpl_insert_at_index(strings, "World", 1); // 修正: インデックスを1に変更
    ck_assert_msg(dpl_size(strings) == 2, "Strings must contain 2 elements.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(strings, 1), "World") == 0, "Second element must be 'World'.");

    dpl_insert_at_index(strings, "!", 100);
    ck_assert_msg(dpl_size(strings) == 3, "Strings must contain 3 elements.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(strings, 2), "!") == 0, "Third element must be '!'.");

    char *first = dpl_get_element_at_index(strings, 0);
    char *second = dpl_get_element_at_index(strings, 1);
    char *third = dpl_get_element_at_index(strings, 2);

    ck_assert_msg(strcmp(first, "Hello") == 0, "First element must be 'Hello'.");
    ck_assert_msg(strcmp(second, "World") == 0, "Second element must be 'World'.");
    ck_assert_msg(strcmp(third, "!") == 0, "Third element must be '!'.");

    strings = dpl_remove_at_index(strings, 1); // "World" を削除
    ck_assert_msg(dpl_size(strings) == 2, "Strings must contain 2 elements after removal.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(strings, 1), "!") == 0, "Second element must be '!' after removal.");

    dpl_free(&strings);
    ck_assert_msg(strings == NULL, "List should be NULL after free.");

    return 0;
}
