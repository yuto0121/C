/**
 * \author Jeroen Van Aken, Bert Lagaisse, Ludo Bruynseels
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "dplist.h"

/*
 * The real definition of struct list / struct node
 */
struct dplist_node
{
    dplist_node_t *prev, *next;
    element_t element;
};

struct dplist
{
    dplist_node_t *head;
    // more fields will be added later
};

dplist_t *dpl_create()
{
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    if (list == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for list.\n");
        return NULL;
    }
    list->head = NULL;
    return list;
}

void dpl_free(dplist_t **list)
{
    if (list == NULL || *list == NULL)
        return;

    dplist_node_t *current = (*list)->head;
    dplist_node_t *temp;

    while (current != NULL)
    {
        temp = current;
        current = current->next;
        free(temp->element);
        free(temp);
    }

    free(*list);
    *list = NULL;
}

/* Important note: to implement any list manipulation operator (insert, append, delete, sort, ...), always be aware of the following cases:
 * 1. empty list ==> avoid errors
 * 2. do operation at the start of the list ==> typically requires some special pointer manipulation
 * 3. do operation at the end of the list ==> typically requires some special pointer manipulation
 * 4. do operation in the middle of the list ==> default case with default pointer manipulation
 * ALWAYS check that you implementation works correctly in all these cases (check this on paper with list representation drawings!)
 **/

dplist_t *dpl_insert_at_index(dplist_t *list, element_t element, int index)
{
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL)
        return NULL;

    list_node = malloc(sizeof(dplist_node_t));

    list_node->element = strdup(element);
    // pointer drawing breakpoint
    if (list->head == NULL)
    { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    }
    else if (index <= 0)
    { // covers case 2
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
        // pointer drawing breakpoint
    }
    else
    {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);
        // pointer drawing breakpoint
        if (index < dpl_size(list))
        { // covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        }
        else
        { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            // pointer drawing breakpoint
        }
    }
    return list;
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index)
{
    if (list == NULL || list->head == NULL)
        return list;

    dplist_node_t *to_remove = dpl_get_reference_at_index(list, index);

    if (to_remove == NULL)
        return list; // ノードが存在しない場合

    if (to_remove->prev != NULL)
    {
        to_remove->prev->next = to_remove->next;
    }
    else
    {
        // 削除するノードが先頭の場合
        list->head = to_remove->next;
    }

    if (to_remove->next != NULL)
    {
        to_remove->next->prev = to_remove->prev;
    }

    free(to_remove->element); // 文字列のメモリを解放
    free(to_remove);

    return list;
}

int dpl_size(dplist_t *list)
{
    if (list == NULL)
        return -1;

    int count = 0;
    dplist_node_t *current = list->head;

    while (current != NULL)
    {
        count++;
        current = current->next;
    }

    return count;
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index)
{
    if (list == NULL || list->head == NULL)
        return NULL;

    dplist_node_t *current = list->head;
    int i = 0;

    if (index <= 0)
    {
        return list->head;
    }

    while (current->next != NULL && i < index)
    {
        current = current->next;
        i++;
    }

    return current;
}

element_t dpl_get_element_at_index(dplist_t *list, int index)
{

    // TODO: add your code here
    dplist_node_t *ref_node = dpl_get_reference_at_index(list, index);
    if (ref_node == NULL)
        return NULL;
    return ref_node->element;
}

int dpl_get_index_of_element(dplist_t *list, element_t element)
{

    // TODO: add your code here
    if (list == NULL || list->head == NULL)
        return -1;

    dplist_node_t *current = list->head;
    int index = 0;

    while (current != NULL)
    {
        if (current->element == element)
        {
            return index;
        }
        current = current->next;
        index++;
    }
    return -1;
}
