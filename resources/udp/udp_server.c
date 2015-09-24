#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 59000

int fd;
struct hostent *hostptr;
struct sockaddr_in serveraddr, clientaddr;
int addrlen;

extern int errno;
/* 192.168.128.1 */

char *buffer;
char msg[6] = "minho";

int udp_open_socket(int fd){
	fd = socket(AF_INET, SOCK_DGRAM,0);
	
	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
	serveraddr.sin_port=htons((int)PORT);
	
	bind(fd, (struct sockaddr*) &serveraddr, sizeof(serveraddr));
	return fd;
}

void udp_receive(int nbytestoreceive){
	printf("Pronto a receber\n");
	recvfrom(fd, buffer, nbytestoreceive*sizeof(char),0, (struct sockaddr*) &clientaddr, &addrlen);
	printf("Mensagem recebida: %s\n", buffer);
	return;
}

void udp_send(int nbytestosend){
	printf("VOu enviar resposta: %s\n", msg);
	sendto(fd,msg, nbytestosend*sizeof(char), 0, (struct sockaddr*)&clientaddr, addrlen);
	printf("Resposta enviada \n");
	return;
}

void udp_close(int fd){
	close(fd);
	return;
}

int main(){

	buffer = (char*)malloc(sizeof(char) *20);
	
	fd = udp_open_socket(fd);
		
	addrlen = sizeof(clientaddr);
	
	udp_receive(6);
	udp_send(6);
	
	udp_close(fd);
	return 0;
}
