#ifndef QUEUE_H
#define QUEUE_H

#include <string.h>
#include <stdlib.h>

typedef struct Node Node;
typedef struct Queue Queue;

typedef struct Node {
    void  *p_data;
    Node  *p_next; 
} Node;

typedef struct Queue {
    size_t elementSize;
    Node   *p_head;
    Node   *p_tail;
    int    length;
} Queue;

Queue *CreateQueue(size_t elementSize);
void DestroyQueue(Queue **queue);
void Enqueue(Queue *queue, void *data);
void Dequeue(Queue *queue, void *p_data);

#endif