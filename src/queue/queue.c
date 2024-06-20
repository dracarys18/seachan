#include "queue.h"
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

queue_t *init_queue(size_t capacity) {
  if (capacity > INT_MAX) {
    return NULL;
  }

  if (capacity == 0) {
    return NULL;
  }

  queue_t *q = (queue_t *)malloc(sizeof(queue_t));
  void **data = (void **)malloc(capacity * sizeof(void *));

  if (!data || !q) {
    free(data);
    free(q);
    return NULL;
  }

  q->data = data;
  q->size = 0;
  q->back = 0;
  q->front = 0;
  q->capacity = capacity;

  return q;
}

int enqueue(queue_t *q, void *data) {
  if (q->size == q->capacity) {
    return -1;
  }

  if (q->capacity == INT_MAX) {
    return -1;
  }

  if ((q->front != -1) && (q->back == q->capacity - 1)) {
    q->back = 0;
  }

  q->data[q->back] = data;

  q->back++;

  q->size++;

  return 0;
}

void *dequeue(queue_t *q) {
  void *data = NULL;

  if (q->size == 0) {
    return NULL;
  }

  if ((q->back < q->front) && (q->front == q->capacity - 1)) {
    q->front = 0;
  }

  data = q->data[q->front];

  q->size--;
  q->front++;

  return data;
}

void *peek(queue_t *q) {
  if (q->back == 0) {
    return NULL;
  }
  return q->data[q->front];
}
