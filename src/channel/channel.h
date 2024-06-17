#ifndef CHANNEL_H
#define CHANNEL_H

#include "../queue/queue.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct sender {
  bool waiting;
  pthread_cond_t cond;
} sender_t;

typedef struct receiver {
  bool waiting;
  pthread_cond_t cond;
} receiver_t;

typedef struct chan {
  receiver_t *receiver;
  sender_t *sender;
  queue_t *que;
  pthread_mutex_t mu;
  bool is_closed;
} channel_t;

channel_t *new_bounded(size_t capacity);
int send(channel_t *chan, void *data);
int recv(channel_t *chan, void **data);
int close(channel_t *chan);

#endif
