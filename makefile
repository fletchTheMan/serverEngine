CC =gcc
CFLAGS =-Wall -pthread -g
LIBS =-lc

server: src/server.c obj/client.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

obj/client.o: src/client.c
	$(CC) -c -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm obj/*.o server
