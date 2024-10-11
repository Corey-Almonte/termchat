termchat: main.o server.o client.o
	gcc -o termchat main.o server.o client.o
	rm *.o
main.o:  main.c server.h client.h
	gcc -g -D_GLIBCXX_DEBUG  -c main.c

server.o: server.c server.h common.h
	gcc -g -D_GLIBCXX_DEBUG -c server.c

client.o: client.c client.h common.h
	gcc -g -D_GLIBCXX_DEBUG -c client.c

clean:
	rm termchat
