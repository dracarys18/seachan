#include "channel.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/// Free all the allocated variables if something goes wrong during init
void chan_free(channel_t *chan, bool mutex, bool s_cond) {
  free(chan->que);
  free(chan->sender);
  free(chan->receiver);

  if (mutex) {
    pthread_mutex_destroy(&chan->mu);
  }
  if (s_cond) {
    pthread_cond_destroy(&chan->sender->cond);
  }

  free(chan);
}

/// Initialises the channel and bounds it to 3 messages at one time in the queue
channel_t *new_bounded(size_t capacity) {
  queue_t *q = init_queue(capacity);

  sender_t *sender = (sender_t *)malloc(sizeof(sender_t));
  receiver_t *receiver = (receiver_t *)malloc(sizeof(receiver_t));

  channel_t *chan = (channel_t *)malloc(sizeof(channel_t));

  // Explicitly free all the allocated variables if any one of them fails
  if (!q || !chan || !sender || !receiver) {
    free(q);
    free(sender);
    free(receiver);
    free(chan);

    return NULL;
  }

  // Initialise all the channel elements
  chan->que = q;
  chan->sender = sender;
  chan->receiver = receiver;
  chan->sender->waiting = false;
  chan->receiver->waiting = false;

  if (pthread_mutex_init(&chan->mu, NULL) != 0) {
    chan_free(chan, false, false);
    return NULL;
  }

  if (pthread_cond_init(&chan->sender->cond, NULL) != 0) {
    chan_free(chan, true, false);
    return NULL;
  }

  if (pthread_cond_init(&chan->receiver->cond, NULL) != 0) {
    chan_free(chan, true, true);
    return NULL;
  }

  return chan;
}

/// Send a message in the channel
int send(channel_t *chan, void *data) {
  pthread_mutex_lock(&chan->mu);
  while (chan->que->size == chan->que->capacity) {
    chan->sender->waiting = true;
    pthread_cond_wait(&chan->sender->cond, &chan->mu);
    chan->sender->waiting = false;
  }

  if (enqueue(chan->que, data) == 0) {
    if (chan->receiver->waiting) {
      pthread_cond_signal(&chan->receiver->cond);
    }

    pthread_mutex_unlock(&chan->mu);
    return 0;
  }

  pthread_mutex_unlock(&chan->mu);
  return -1;
}

/// Receive a message in the channel
int recv(channel_t *chan, void **data) {
  pthread_mutex_lock(&chan->mu);
  while (chan->que->size == 0) {
    chan->receiver->waiting = true;
    pthread_cond_wait(&chan->receiver->cond, &chan->mu);

    chan->receiver->waiting = false;
  }

  void *message = dequeue(chan->que);

  if (message) {
    *data = message;
    if (chan->sender->waiting) {
      pthread_cond_signal(&chan->sender->cond);
    }
    pthread_mutex_unlock(&chan->mu);
    return 0;
  }

  pthread_mutex_unlock(&chan->mu);
  return -1;
}
