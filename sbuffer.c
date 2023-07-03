/**
 * \author Chrysovalantis Psilos
 */

#include <stdlib.h>
#include <stdio.h>
#include "sbuffer.h"
#include <pthread.h>

/**
 * basic node for the buffer, these nodes are linked together to create the buffer
 */
typedef struct sbuffer_node {
    struct sbuffer_node *next;  /**< a pointer to the next node*/
    sensor_data_t data;         /**< a structure containing the data */
} sbuffer_node_t;

/**
 * a structure to keep track of the buffer
 */
struct sbuffer {
    sbuffer_node_t *head;       /**< a pointer to the first node in the buffer */
    sbuffer_node_t *tail;       /**< a pointer to the last node in the buffer */
    pthread_mutex_t *mutex;
};

int sbuffer_init(sbuffer_t **buffer) {
	
    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL) return SBUFFER_FAILURE;
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    (*buffer)->mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init((*buffer)->mutex,NULL);
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **buffer) {
	pthread_mutex_t *m;
	pthread_mutex_lock((*buffer)->mutex);
    sbuffer_node_t *dummy;
    if ((buffer == NULL) || (*buffer == NULL)) {
    	pthread_mutex_unlock((*buffer)->mutex);
        return SBUFFER_FAILURE;
    }
    while ((*buffer)->head) {
        dummy = (*buffer)->head;
        (*buffer)->head = (*buffer)->head->next;
        free(dummy);
    }
    m = (*buffer)->mutex;
    free(*buffer);
    *buffer = NULL;
    pthread_mutex_unlock(m);
    pthread_mutex_destroy(m);
    free(m);
    return SBUFFER_SUCCESS;
}

int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data) {
	pthread_mutex_lock(buffer->mutex);
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;
    if (buffer->head == NULL) return SBUFFER_NO_DATA;
    *data = buffer->head->data;
    dummy = buffer->head;
    if (buffer->head == buffer->tail) // buffer has only one node
    {
        buffer->head = buffer->tail = NULL;
    } else  // buffer has many nodes empty
    {
        buffer->head = buffer->head->next;
    }
    free(dummy);
    pthread_mutex_unlock(buffer->mutex);
    return SBUFFER_SUCCESS;
}

int sbuffer_getData(sbuffer_t *buffer, sensor_data_t *data){
	pthread_mutex_lock(buffer->mutex);
	if(buffer==NULL) return SBUFFER_FAILURE;
	if(buffer->head==NULL) return SBUFFER_NO_DATA;
	*data = buffer->head->data;
	buffer->head = buffer->head->next;
	pthread_mutex_unlock(buffer->mutex);
	return SBUFFER_SUCCESS;
}

int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data) {
	pthread_mutex_lock(buffer->mutex);
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;
    dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL) return SBUFFER_FAILURE;
    dummy->data = *data;
    dummy->next = NULL;
    if (buffer->tail == NULL) // buffer empty (buffer->head) should also be NULL
    {
        buffer->head = buffer->tail = dummy;
    } else // buffer not empty
    {
        buffer->tail->next = dummy;
        buffer->tail = buffer->tail->next;
    }
    pthread_mutex_unlock(buffer->mutex);
    return SBUFFER_SUCCESS;
}


