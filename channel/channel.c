#include "channel.h"
#include <pthread.h>
#include <stdlib.h>

channel_t *new_bounded(size_t capacity) {
  queue_t *q = init_queue(capacity);
  channel_t *chan = (channel_t *)malloc(sizeof(channel_t));

  if (!q || !chan) {
    free(q);
    free(chan);

    return NULL;
  }

  if (pthread_mutex_init(&chan->mu, NULL) != 0) {
    free(q);
    free(chan);

    return NULL;
  }

  chan->que = q;
  return chan;
}

int send(channel_t *chan, void *data) {
  pthread_mutex_lock(&chan->mu);
  while (chan->que->size == chan->que->capacity) {
    chan->sender->waiting = true;
    pthread_cond_wait(&chan->sender->cond, &chan->mu);
    chan->sender->waiting = false;
  }

  if (enqueue(chan->que, data) == 0) {
    if (chan->recv->waiting) {
      pthread_cond_signal(&chan->recv->cond);
    }

    pthread_mutex_unlock(&chan->mu);
    return 0;
  }

  pthread_mutex_unlock(&chan->mu);
  return -1;
}

int recv(channel_t *chan, void *data) {
  pthread_mutex_lock(&chan->mu);

  while (chan->que->size == 0) {
    chan->recv->waiting = true;
    pthread_cond_wait(&chan->recv->cond, &chan->mu);

    chan->recv->waiting = false;
  }

  void *message = dequeue(chan->que);

  if (message) {
    data = message;

    if (chan->sender->waiting) {
      pthread_cond_signal(&chan->sender->cond);
    }
    pthread_mutex_unlock(&chan->mu);
    return 0;
  }

  pthread_mutex_unlock(&chan->mu);
  return -1;
}
