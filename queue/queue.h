#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

typedef struct queue {
  void **data;
  size_t size;
  size_t capacity;
  int front;
  int back;
} queue_t;

queue_t *init_queue(size_t capacity);
int enqueue(queue_t *q, void *data);
void *dequeue(queue_t *q);
void *peek(queue_t *q);

#endif
