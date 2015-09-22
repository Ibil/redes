
all: 
	gcc -o user Client/client.c
	gcc -o CS Server/CS.c
	gcc -o SS storage/SS.c
