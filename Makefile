CC =clang
CFLAGS = -flto=full -O2 -rdynamic -arch arm64 -I src/queue/ -I src/channel/
SRC ?=src

default: build

build:
	$(CC) $(CFLAGS) $(SRC)/queue/queue.c $(SRC)/channel/channel.c $(SRC)/main.c -o chan

run: build
	./chan

clean:
	rm chan
