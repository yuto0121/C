/**
 * \author Yusuf Hussein
 */

#include "sbuffer.h"
#include <semaphore.h>
#include <pthread.h>

pthread_mutex_t buffer_lock;
pthread_cond_t buf_add_sig;

/**
 * basic node for the buffer, these nodes are linked together to create the buffer
 */
typedef struct sbuffer_node {
    struct sbuffer_node *next;  /**< a pointer to the next node*/
    sensor_data_t data;         /**< a structure containing the data */
    bool accessed[2];
} sbuffer_node_t;

/**
 * a structure to keep track of the buffer
 */
struct sbuffer {
    sbuffer_node_t *head;       /**< a pointer to the first node in the buffer */
    sbuffer_node_t *tail;       /**< a pointer to the last node in the buffer */
};

int sbuffer_init(sbuffer_t **buffer) {
    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL) return SBUFFER_FAILURE;
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    pthread_mutex_init(&buffer_lock, NULL);
    pthread_cond_init(&buf_add_sig, NULL);
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **buffer) {
    sbuffer_node_t *dummy;
    if ((buffer == NULL) || (*buffer == NULL)) {
        return SBUFFER_FAILURE;
    }
    while ((*buffer)->head) {
        dummy = (*buffer)->head;
        (*buffer)->head = (*buffer)->head->next;
        free(dummy);
    }
    free(*buffer);
    *buffer = NULL;
    pthread_mutex_destroy(&buffer_lock);
    pthread_cond_destroy(&buf_add_sig);
    return SBUFFER_SUCCESS;
}

sbuffer_node_t *get_first_unaccessed(sbuffer_t *buffer, int access_code) {
    if(buffer->head == NULL) return NULL;
    sbuffer_node_t *dummy;
    dummy = buffer->head;
    while(dummy->accessed[access_code]) {
        dummy = dummy->next;
        if(dummy == NULL) return NULL;
    }
    return dummy;
}

int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data, int access_code) {
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;

    pthread_mutex_lock(&buffer_lock);
    // get first node not previously accessed

    while ((dummy = get_first_unaccessed(buffer,  access_code)) == NULL) {
        // blocking wait
        pthread_cond_wait(&buf_add_sig, &buffer_lock);
    }

    *data = dummy->data;
    dummy->accessed[access_code] = true;

    // check for eof
    if(data->id == 0) {
        pthread_mutex_unlock(&buffer_lock);
        return SBUFFER_NO_DATA;
    }

    // if it hasn't been accessed by one of the threads, don't remove it
    else if(!dummy->accessed[0] || !dummy->accessed[1]) {
        pthread_mutex_unlock(&buffer_lock);
        return SBUFFER_SUCCESS;
    }

    // remove if accessed by both threads
    if (buffer->head == buffer->tail) // buffer has only one node
    {
        buffer->head = buffer->tail = NULL;
    } else  // buffer has many nodes empty
    {
        buffer->head = buffer->head->next;
    }
    free(dummy);

    pthread_mutex_unlock(&buffer_lock);
    return SBUFFER_SUCCESS;
}

int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data) {
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;

    dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL) return SBUFFER_FAILURE;

    pthread_mutex_lock(&buffer_lock);
    // initialize dummy
    dummy->data = *data;
    dummy->accessed[0] = false;
    dummy->accessed[1] = false;
    dummy->next = NULL;

    if (buffer->tail == NULL) // buffer empty (buffer->head should also be NULL
    {
        buffer->head = buffer->tail = dummy;
    } else // buffer not empty
    {
        buffer->tail->next = dummy;
        buffer->tail = buffer->tail->next;
    }

    pthread_cond_broadcast(&buf_add_sig);

    pthread_mutex_unlock(&buffer_lock);
    return SBUFFER_SUCCESS;
}
