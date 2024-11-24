#define _GNU_SOURCE

#include "dplist.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct
{
    int id;
    char *name;
} my_element_t;

void *element_copy(void *element);
void element_free(void **element);
int element_compare(void *x, void *y);

void *element_copy(void *element)
{
    my_element_t *copy = malloc(sizeof(my_element_t));
    char *new_name;
    asprintf(&new_name, "%s", ((my_element_t *)element)->name); // asprintf requires _GNU_SOURCE
    assert(copy != NULL);
    copy->id = ((my_element_t *)element)->id;
    copy->name = new_name;
    return (void *)copy;
}

void element_free(void **element)
{
    free((((my_element_t *)*element))->name);
    free(*element);
    *element = NULL;
}

int element_compare(void *x, void *y)
{
    return ((((my_element_t *)x)->id < ((my_element_t *)y)->id) ? -1 : (((my_element_t *)x)->id == ((my_element_t *)y)->id) ? 0
                                                                                                                            : 1);
}

void ck_assert_msg(bool result, char *msg)
{
    if (!result)
        printf("%s\n", msg);
}

void yourtest1()
{
    // Test free NULL, don't use callback
    dplist_t *list = NULL;
    dpl_free(&list, false);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // Test free NULL, use callback
    list = NULL;
    dpl_free(&list, true);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // Test free empty list, don't use callback
    list = dpl_create(element_copy, element_free, element_compare);
    dpl_free(&list, false);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // Test free empty list, use callback
    list = dpl_create(element_copy, element_free, element_compare);
    dpl_free(&list, true);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // TODO : your test scenarios
    my_element_t *elem_deep = malloc(sizeof(my_element_t));
    elem_deep->id = 100;
    elem_deep->name = strdup("Deep Copy Element");

    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, elem_deep, 0, true); // insert_copy = true

    elem_deep->id = 200;
    strcpy(elem_deep->name, "Modified Original");

    my_element_t *retrieved_deep = dpl_get_element_at_index(list, 0);

    ck_assert_msg(retrieved_deep->id == 100, "Failure: Deep copy should not reflect changes in original element ID");
    ck_assert_msg(strcmp(retrieved_deep->name, "Deep Copy Element") == 0, "Failure: Deep copy should not reflect changes in original element name");

    my_element_t *elem_shallow = malloc(sizeof(my_element_t));
    elem_shallow->id = 300;
    elem_shallow->name = strdup("Shallow Copy Element");

    list = dpl_insert_at_index(list, elem_shallow, 1, false); // insert_copy = false

    elem_shallow->id = 400;
    strcpy(elem_shallow->name, "Modified Shallow");

    my_element_t *retrieved_shallow = dpl_get_element_at_index(list, 1);

    ck_assert_msg(retrieved_shallow->id == 400, "Failure: Shallow copy should reflect changes in original element ID");
    ck_assert_msg(strcmp(retrieved_shallow->name, "Modified Shallow") == 0, "Failure: Shallow copy should reflect changes in original element name");

    dpl_free(&list, true);
    ck_assert_msg(list == NULL, "Failure: List should be NULL after freeing");

    free(elem_deep->name);
    free(elem_deep);
    free(elem_shallow->name);
    free(elem_shallow);
}

int main(void)
{

    yourtest1();
    return 0;
}
