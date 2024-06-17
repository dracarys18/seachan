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
  receiver_t *recv;
  sender_t *sender;
  queue_t *que;
  pthread_mutex_t mu;
} channel_t;

#endif
