#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>

#define PORT 58000
                                      
int fd, newfd, clientlen, nleft, nread, nwritten;
int nbytestoread = 20, nbytestowrite = 35;

char *ptr, buffer[128]; 
char *msg = "server: Ola para ti tambem client\n";

extern int errno;

struct sockaddr_in serveraddr, clientaddr;


void tcp_connect(){
	if ((fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
		printf("Erro no socket : \n");
		exit(1);
	}
	memset((void*) &serveraddr, (int)'\0',sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
	serveraddr.sin_port=htons((short int)PORT);
	if (bind(fd,(struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1){
		printf("Erro no bind : \n");
		exit(1);
	}
	if (listen (fd,5) == -1){
		printf("Erro no listen : \n");
		exit(1);
	}
	printf("Estou a escuta!\n");
	clientlen = sizeof(clientaddr);
	if((newfd = accept(fd,(struct sockaddr*)&clientaddr,&clientlen)) == -1){
		printf("Erro no accept\n");
		exit(1);
	}
	printf("Aceitei a ligacao\n");
	return;
}

void tcp_read(int nbytestoread){
	printf("Vou ler a mensagem\n");
	ptr=&buffer[0];
	nleft = nbytestoread;
	while(nleft>0){
		if((nread = read(newfd,ptr,nleft)) == -1){
			printf("Erro a ler a mensagem\n");
			exit(1);
		}
		nleft-=nread;
		ptr+=nread;
	}
	printf("recebi a mensagem : %s\n", buffer);
	return;
}

void tcp_write(int nbytestowrite){
	printf("vou enviar a resposta: %s\n", msg);
	ptr =strcpy(buffer,msg);
	nleft = nbytestowrite;
	while(nleft > 0){
		if( (nwritten=write(newfd,ptr,nleft)) == -1){
			printf("erro no write\n");
			exit(1);
		}
	nleft-=nwritten;
	ptr+=nwritten;
	}
	printf("Resposta enviada\n");
	return;
}

void tcp_close(){	
	close(newfd);
	close(fd);
	return;
}

int main(){

	tcp_connect();
	
	tcp_read(nbytestoread);
	tcp_write(nbytestowrite);
	
	tcp_close();
	return 0 ;
}
