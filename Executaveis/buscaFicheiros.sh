#Script para ir buscar código fonte e header files

#Projeto cliente
cp ../Cliente/*.c ./Files
cp ../Cliente/*.h ./Files

#Projeto Servidor

cp ../Servidor/*.c ./Files
cp ../Servidor/*.h ./Files

#Buscar Header file comum

cp ../medit-defaults.h ./Files
cp ../medit-defaults.h ./Files

echo "Efetuado com sucesso!"
