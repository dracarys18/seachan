#include "channel/channel.h"
#include <stdio.h>

int main() {
  channel_t *chan = new_bounded(3);
  if (!chan) {
    return 0;
  }
  char *msg = "What's up";
  int s = send(chan, msg);

  char *data;
  int k = recv(chan, (void *)&data);
  printf("Message %s\n", (char *)data);
}
