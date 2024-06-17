#include "channel.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

channel_t *new_bounded(size_t capacity) {
  queue_t *q = init_queue(capacity);

  sender_t *sender = (sender_t *)malloc(sizeof(sender_t));
  receiver_t *receiver = (receiver_t *)malloc(sizeof(receiver_t));

  channel_t *chan = (channel_t *)malloc(sizeof(channel_t));
  if (!q || !chan || !sender || !receiver) {
    printf("Here in malloc of stuff");
    free(q);
    free(sender);
    free(receiver);
    free(chan);
    printf("I am in goto now");
    return NULL;
  }

  if (pthread_mutex_init(&chan->mu, NULL) != 0) {
    printf("Here in mutex");
    free(q);
    free(sender);
    free(receiver);
    free(chan);
    printf("I am in goto now");
    return NULL;
  }

  chan->sender = sender;
  chan->receiver = receiver;

  if (pthread_cond_init(&chan->sender->cond, NULL) != 0) {
    printf("Here in cond sender");
    pthread_mutex_destroy(&chan->mu);
    free(q);
    free(sender);
    free(receiver);
    free(chan);
    printf("I am in goto now");
    return NULL;
  }

  if (pthread_cond_init(&chan->receiver->cond, NULL) != 0) {
    printf("Here in cond receiver");
    pthread_mutex_destroy(&chan->mu);
    free(q);
    free(sender);
    free(receiver);
    free(chan);
    printf("I am in goto now");
    return NULL;
  }

  chan->que = q;
  chan->sender->waiting = false;
  chan->receiver->waiting = false;

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
    if (chan->receiver->waiting) {
      pthread_cond_signal(&chan->receiver->cond);
    }

    pthread_mutex_unlock(&chan->mu);
    return 0;
  }

  pthread_mutex_unlock(&chan->mu);
  return -1;
}

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
