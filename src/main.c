#include "channel/channel.h"
#include <stdio.h>

int main() {
  channel_t *chan = new_bounded(3);
  if (!chan) {
    return 0;
  }
  char *msg = "What's up";
  int s = send(chan, msg);
  char *msg2 = "I am great";
  int s2 = send(chan, msg2);

  char *data;
  int k = recv(chan, (void *)&data);

  char *data2;
  int k2 = recv(chan, (void *)&data2);

  int l = close(chan);
  printf("Message1 %s\n", (char *)data);
  printf("Message2 %s\n", (char *)data2);
}
