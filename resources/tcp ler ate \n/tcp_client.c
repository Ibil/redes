#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>

#define PORT 58000

int fd, nleft,nwritten,nread;
int nbytestoread = 35, nbytestowrite= 20;

char *ptr,buffer[128];
char* msg ;

struct hostent *hostptr;
struct sockaddr_in serveraddr;

int tcp_connect(int fd){
	fd=socket(AF_INET,SOCK_STREAM,0);
	hostptr = gethostbyname("localhost");

	memset((void*)&serveraddr, (int)'\0',sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
	serveraddr.sin_port=htons((short int)PORT);

	connect(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
   printf("connect aceite\n");
   return fd;
}

void tcp_write(int nbytestowrite){
	ptr =strcpy(buffer,"client: Ola server\n");
   nleft=nbytestowrite;
   printf("vou enviar mensagem: %s\n", ptr);
	while(nleft > 0)	{
		if( (nwritten=write(fd,ptr,nleft)) == -1){
			printf("erro no write\n");
			exit(1);
		}
		nleft-=nwritten;
		ptr+=nwritten;
	}
	printf("Mensagem Enviada\n");
	return;
}

void tcp_read(int nbytestoread){
	nleft=nbytestoread;
	/* vou escrever por cima que nao faz mal neste exemplo*/
	ptr=&buffer[0];
	printf("Vou ler a resposta\n");
	while(nleft>0){
		if((nread = read(fd,ptr,nleft)) == -1){
			printf("Erro a ler a resposta\n");
			exit(1);
		}
		nleft-=nread;
		ptr+=nread;
	}
	printf("A resposta lida foi : %s\n", buffer);
	return;
}

void tcp_close(int fd){
	close(fd);
}

int main(){

	fd = tcp_connect(fd);
	
	tcp_write(nbytestowrite);
	tcp_read(nbytestoread);
	
	tcp_close(fd);
	
 	return 0;
}
