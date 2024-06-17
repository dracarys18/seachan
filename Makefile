CC =clang
CFLAGS = -arch arm64 -I queue/ -I channel/

default: build

build:
	$(CC) $(CFLAGS) queue/queue.c main.c -o chan

run: build
	./chan

clean:
	rm chan
