#include <string.h>
#include <stdlib.h>

#include "queue.h"

Queue *CreateQueue(size_t elementSize)
{
    Queue *queue = malloc(sizeof(Queue));
    queue->length = 0;
    queue->elementSize = elementSize;
    queue->p_head = NULL;
    queue->p_tail = NULL;

    return queue;
}

void DestroyQueue(Queue **queue)
{
    if (queue == NULL) {
        return;
    }

    Queue *q = *queue;

    while(q->length > 0) {
        Node *node = q->p_head;
        q->p_head = q->p_head->p_next;
        free(node->p_data);
        free(node);
        q->length--;
    }

    free(*queue);
    *queue = NULL;
}

void Enqueue(Queue *queue, void *data)
{
    if (queue == NULL) {
        return;
    }

    Node *node = malloc(sizeof(Node*));
    node->p_data = malloc(queue->elementSize); 
    node->p_next = NULL;
    memcpy(node->p_data, data, queue->elementSize);
    
    if (queue->length == 0) {
        queue->p_head = node;
        queue->p_tail = node;
        queue->length = 1;
        
        return;
    }

    queue->p_tail->p_next = node;
    queue->p_tail = node;
    queue->length++;
}

void Dequeue(Queue *queue, void *p_data)
{
    if (queue == NULL || queue->length == 0) {
        
        return;
    }

    Node *node = queue->p_head;
    memcpy(p_data, node->p_data, queue->elementSize);

    if (queue->length == 1) {
        queue->length = 0;
        queue->p_head = NULL;
        queue->p_tail = NULL;
        
        free(node->p_data);
        free(node);

        return;
    }

    queue->p_head = queue->p_head->p_next;
    queue->length--;

    free(node->p_data);
    free(node);
}

