FLAGS=-g
NCURSES=-lncurses
PTHREAD=-pthread
HEADERS=-I.
RUN=-D NO_IDE
SERVER_DEPENDENCIES=server-utils.o server-users.o server-commands.o server-functions.o biblioteca.o
CLIENT_DEPENDENCIES=client-functions.o biblioteca.o

all : server client

server : server.o $(SERVER_DEPENDENCIES)
	gcc $(SERVER_DEPENDENCIES) server.o -o servidor $(FLAGS) $(PTHREAD)
	
server.o : server.c
	gcc server.c -c $(HEADERS) $(RUN)

server-utils.o : server-utils.c
	gcc server-utils.c -c

server-users.o : server-users.c
	gcc server-users.c -c $(HEADERS) $(RUN)

server-commands.o : server-commands.c
	gcc server-commands.c -c $(HEADERS) $(RUN)
	
server-functions.o : server-functions.c
	gcc server-functions.c -c $(HEADERS) $(RUN)

client : client.o $(CLIENT_DEPENDENCIES)
	gcc $(CLIENT_DEPENDENCIES) client.o -o cliente $(FLAGS) $(NCURSES)

client.o : client.c
	gcc client.c -c $(HEADERS) $(RUN)
	
client-functions.o : client-functions.c
	gcc client-functions.c -c $(HEADERS) $(RUN)

biblioteca.o : biblioteca.c
	gcc -c biblioteca.c biblioteca.h $(HEADERS) $(RUN)
	ar rcs biblioteca.a biblioteca.o
	
remake : limpa_objs limpa_ficheiros get_ficheiros all

limpa_objs :
	rm ./*.o

limpa_ficheiros :
	rm ./*.c
	rm ./*.h
	
get_ficheiros :
#Projeto cliente
	cp ../../Cliente/*.c .
	cp ../../Cliente/*.h .
#Projeto Servidor
	cp ../../Servidor/*.c .
	cp ../../Servidor/*.h .
	cp ../../Servidor/*.db .
#Projeto Biblioteca
	cp ../../Biblioteca/*.c .
	cp ../../Biblioteca/*.h .
#Buscar Header file comum
	cp ../../medit-defaults.h .
	cp ../../medit-defaults.h .
