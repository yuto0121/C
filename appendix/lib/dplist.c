/**
 * \author Yusuf Hussein
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"




/*
 * The real definition of struct list / struct node
 */

struct dplist_node {
    dplist_node_t *prev, *next;
    void *element;
};

struct dplist {
    dplist_node_t *head;

    void *(*element_copy)(void *src_element);

    void (*element_free)(void **element);

    int (*element_compare)(void *x, void *y);
};


dplist_t *dpl_create(// callback functions
        void *(*element_copy)(void *src_element),
        void (*element_free)(void **element),
        int (*element_compare)(void *x, void *y)
) {
    // allocate memory for list
    dplist_t *list;
    list = malloc(sizeof(struct dplist));

    // initialize empty list
    list->head = NULL;

    // callbacks
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;
    return list;
}

void dpl_free(dplist_t **list, bool free_element) {
    // if list is NULL
    if((*list) == NULL) return;

    // if list is empty
    int size = dpl_size(*list);
    if(!size)
    {
        free(*list);
        *list = NULL;
        return;
    }


    // go over each node/element and free its memory
    for(int i = size-1; i >= 0; i--) {
        dplist_node_t *cur = dpl_get_reference_at_index(*list, i);
        if(cur->prev != NULL) cur->prev->next = NULL;

        if(free_element) {
            (*list)->element_free(&cur->element);
        }
        free(cur);
    }

    // free list and point to NULL
    free(*list);
    *list = NULL;

}

dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {
    // if list is null
    if(list == NULL) return NULL;

    // generate new node
    dplist_node_t *new = malloc(sizeof(dplist_node_t));
    if(insert_copy) {
        new->element = list->element_copy(element);
    }
    else {
        new->element = element;
    }

    // get size of list
    int size = dpl_size(list);

    // if empty list
    if(size == 0) {
        list->head = new;
        new->next = NULL;
        new->prev = NULL;
        return list;
    }

    // start of list
    else if(index <= 0) {
        dplist_node_t *old = list->head;
        list->head = new;
        old->prev = new;
        new->next = old;
        new->prev = NULL;

        return list;
    }

    // end of list
    else if(index >= size) {
        // get last node
        dplist_node_t *cur = dpl_get_reference_at_index(list, size-1);

        cur->next = new;
        new->prev = cur;
        new->next = NULL;

        return list;
    }
    // middle of list
    else {
        // get node at index
        dplist_node_t *cur = dpl_get_reference_at_index(list, index);

        // rearrange nodes
        new->prev = cur->prev;
        new->prev->next = new;
        cur->prev = new;
        new->next = cur;

        return list;
    }
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) {

    // if null list return null
    if(list == NULL) return NULL;

    // get size and return unmodified if empty
    int size = dpl_size(list);
    if(!size) return list;

    // clamp index
    index = index < 0 ? 0 : index;
    index = index > size-1 ? size-1 : index;


    // if remove first
    if(index == 0) {
        dplist_node_t *cur = list->head;
        list->head = cur->next;
        if(cur->next != NULL) {
            cur->next->prev = NULL;
        }

        // free element* and node*
        if(free_element) {
            list->element_free(&cur->element);
        }
        free(cur);

        return list;
    }

    // get node at index
    dplist_node_t *cur = dpl_get_reference_at_index(list, index);

    // rearrange list nodes
    cur->prev->next = cur->next;
    if(cur->next != NULL) cur->next->prev = cur->prev;

    // free element*
    if(free_element) {

        list->element_free(&cur->element);
    }

    // free node*
    free(cur);
    return list;

}

int dpl_size(dplist_t *list) {
    // if list is null
    if(list == NULL) return -1;

    // if list is empty
    if(list->head == NULL)
        return 0;

    // start node
    dplist_node_t *cur = list->head;

    // initial size, then increment while iterating over full list
    int size = 1;
    while(cur->next != NULL) {
        size++;
        cur = cur->next;
    }

    return size;
}

void *dpl_get_element_at_index(dplist_t *list, int index) {

    dplist_node_t *ref = dpl_get_reference_at_index(list, index);
    if(ref == NULL) return NULL;

    return ref->element;

}

int dpl_get_index_of_element(dplist_t *list, void *element) {
    // if list is NULL
    if(list == NULL) return -1;

    int count = 0;

    // start node
    dplist_node_t *cur = list->head;

    // iterate over full list, when element is found return counter
    while(cur != NULL) {
        if(!list->element_compare(cur->element, element)) {
            return count;
        }

        count++;
        cur = cur->next;
    }


    return -1;

}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {

    // list is null
    if(list == NULL) return NULL;

    // get size
    int size = dpl_size(list);
    if(!size) return NULL;

    // clamp index
    index = index < 0 ? 0 : index;
    index = index > size-1 ? size-1 : index;

    dplist_node_t *cur = list->head;

    // set cur to correct index
    for(int i = 0; i < index; i++) {
        cur = cur->next;
    }

    return cur;

}

void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference) {
    // if list is null
    if(list == NULL || reference == NULL) return NULL;

    // get size and return NULL if empty list
    int size = dpl_size(list);

    if(!size) return NULL;

    // start node
    dplist_node_t *cur = list->head;
    dplist_node_t *sol = NULL;

    // find node that matches reference
    while(cur->next != NULL && cur != reference) {
        cur = cur->next;
    }

    if(cur == reference) {
        sol = cur;
    }

    // if there was a solution return it else return NULL
    return sol != NULL ? sol->element : NULL;

}


