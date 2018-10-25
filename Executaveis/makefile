FLAGS=-g

ex : server.o client.o
	gcc ./Files/server.o -o servidor $(FLAGS)
	gcc ./Files/client.o -o cliente $(FLAGS)

server.o : server.c ......util.h
	gcc ex1.c -c $(FLAGS)

client.o : util.c
	gcc util.c -c $(FLAGS)

#por acabar......

limpa_objs :
	rm ./Files/*.o

limpa_ficheiros :
	rm ./Files/*.c
	rm ./Files/*.h
	
get_ficheiros :
#Projeto cliente
	cp ../Cliente/*.c ./Files
	cp ../Cliente/*.h ./Files
#Projeto Servidor
	cp ../Servidor/*.c ./Files
	cp ../Servidor/*.h ./Files
#Buscar Header file comum
	cp ../medit-defaults.h ./Files
	cp ../medit-defaults.h ./Files
