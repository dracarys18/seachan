#include "channel/channel.h"
#include <stdio.h>

int main() {
  channel_t *chan = new_bounded(3);
  void *msg = "What's up";
  int s = send(chan, msg);

  void *data;
  int k = recv(chan, data);
  printf("%s\n", (char*)data);
}
