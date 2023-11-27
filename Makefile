all:	common.o clients.o servermain.o clientmain.o
	gcc common.o clients.o servermain.o -o server
	gcc common.o clients.o clientmain.c -o client

common.o:	common.c
	gcc -c common.c

clientmain.o:	clientmain.c
	gcc -c clientmain.c	

clients.o:	clients.c
	gcc -c clients.c

servermain.o:	servermain.c
	gcc -c servermain.c

clean:
	rm -f *.o
	rm server
	rm client