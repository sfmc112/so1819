#Script para ir buscar código fonte e header files

#Projeto cliente
cp ../Cliente/*.c ./Cliente
cp ../Cliente/*.h ./Cliente

#Projeto Servidor

cp ../Servidor/*.c ./Servidor
cp ../Servidor/*.h ./Servidor

#Buscar Header file comum

cp ../medit-defaults.h ./Servidor
cp ../medit-defaults.h ./Cliente

echo "Efetuado com sucesso!"
