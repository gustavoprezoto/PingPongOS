#include <stdio.h>
#include "../include/queue.h"

void element_cleanup(queue_t *elem);

int queue_size (queue_t *queue) {
    if(queue == NULL)
        return 0;

    if(queue->next == queue)
        return 1;

    int queue_size = 1;
    queue_t *queue_iterator = queue;
    while(queue_iterator->next != queue) {
        queue_size++;
        queue_iterator = queue_iterator->next;
    }

    return queue_size;
}

void queue_print (char *name, queue_t *queue, void print_elem (void*) ) {
    queue_t* iterator = queue;

    printf("%s", name);
    printf("[");

    // Se a fila é nula, então aborta.
    if (queue == NULL) {
        printf("]\n");
        return;
    }

    // Caso externo (só para não printar o primeiro espaço a esquerda)
    print_elem(iterator);
    iterator = iterator->next;

    while (iterator != queue) {
        printf(" ");
        print_elem(iterator);
        iterator = iterator->next;
    }

    printf("]\n");
    return;
}

int queue_append (queue_t **queue, queue_t *elem) {
    if(elem->next != NULL || elem->prev != NULL) {
        return -1;
    }

    if(queue == NULL) {
        return -2;
    }

    if(*queue == NULL) {
        *queue = elem;
        (*queue)->next = elem;
        (*queue)->prev = elem;
        return 0;
    }

    queue_t *first_element = *queue;
    queue_t *last_element = (*queue)->prev;

    elem->prev = last_element;
    elem->next = first_element;
    first_element->prev = elem;
    last_element->next = elem;

    return 0;
}

int queue_remove (queue_t **queue, queue_t *elem) {
    if(elem == NULL) {
        return -1;
    }

    if(queue == NULL) {
        return -2;
    }

    if(*queue == NULL) {
        return -3;
    }

    if(elem == *queue) {
        if((*queue)->next == *queue && (*queue)->prev == *queue) {
            element_cleanup(*queue);
            *queue = NULL;
            return 0;
        }
    }

    int element_found_flag = 0;
    queue_t *first_element = *queue;
    queue_t *queue_iterator = *queue;
    while (1) {
        if(elem == queue_iterator) {
            (queue_iterator->prev)->next = queue_iterator->next;
            (queue_iterator->next)->prev = queue_iterator->prev;
            element_found_flag = 1;
            break;
        }

        queue_iterator = queue_iterator->next;
        if(queue_iterator == first_element) {
            break;
        }
    }

    if(!element_found_flag) {
        return -4;
    }

    if(elem == first_element) {
        *queue = first_element->next;
    }

    element_cleanup(elem);
    return 0;
}

void element_cleanup(queue_t *elem) {
    elem->next = NULL;
    elem->prev = NULL;
}
