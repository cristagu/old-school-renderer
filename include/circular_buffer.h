#include <stdio.h>

typedef struct CircularBuffer
{
    void **elements;
    int length;
    int size;
    int front;
    int rear;
} CircularBuffer;

CircularBuffer *CircularBufferCreate(int size)
{
    CircularBuffer *buffer = malloc(sizeof(CircularBuffer));
    buffer->elements = malloc(sizeof(void*) * size); 
    buffer->length = 0;
    buffer->front = 0;
    buffer->rear = 0;
    buffer->size = size + 1;

    return buffer;
}

void CircularBufferAdd(CircularBuffer *buffer, void *data)
{
    if ((buffer->rear + 1) % buffer->size == buffer->front) {
        printf("Circular buffer overflow! \n");
        return;
    }

    buffer->elements[buffer->rear % buffer->size] = data;
    buffer->rear++;
    buffer->length++;
}

void* CircularBufferRemove(CircularBuffer *buffer)
{
    if ((buffer->front % buffer->size) >= buffer->rear) {
        printf("Circular buffer underflow! \n");
        return NULL;
    }
    
    buffer->front++;
    buffer->length--;

    return buffer->elements[(buffer->front -1) % buffer->size];
}
