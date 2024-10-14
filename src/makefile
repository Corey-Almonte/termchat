# targets
all: main client


# termchat
main: main.o server.o
	rm -f main
	gcc -o main main.o server.o
	mv *.o bin/

main.o:  main.c server.h client.h common.h
	gcc -g -D_GLIBCXX_DEBUG -c main.c

server.o: server.c server.h common.h
	gcc -g -D_GLIBCXX_DEBUG -c server.c

# client
client: client.o
	rm -f client
	gcc -o client client.o
	mv client.o bin/

client.o: client.c client.h common.h
	gcc -g -D_GLIBCXX_DEBUG -c client.c

clean:
	rm bin/*.o
	rm client
	rm termchat
