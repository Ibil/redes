#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 59000
int fd;
struct hostent *hostptr;
struct sockaddr_in serveraddr;
int addrlen;

extern int errno;
/* 192.168.128.1 */

char *buffer;
char msg[6] = "olasg";


int udp_open(int fd){
	fd = socket(AF_INET, SOCK_DGRAM,0);
	
	/* meter "localhost" para testes*/
	hostptr=gethostbyname("localhost");
	
	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=((struct in_addr*)(hostptr->h_addr_list[0]))->s_addr;
	serveraddr.sin_port=htons((int)PORT);
	return fd;
}

void udp_send(int nbytestosend){
	printf("VOu enviar a mensagem: %s\n", msg);
	sendto(fd,msg, nbytestosend*sizeof(char), 0, (struct sockaddr*)&serveraddr, addrlen);
	printf("Mensagem enviada\n");
	return;
}

void udp_receive(int nbytestoread){
	printf("Pronto a receber\n");
	recvfrom(fd, buffer, nbytestoread*sizeof(char),0, (struct sockaddr*) &serveraddr, &addrlen);
	printf("Recebi resposta: %s\n", buffer);
	return;
}

void udp_close(int fd){
	close(fd);
	return;
}

int main(){

	buffer = (char*)malloc(sizeof(char) *20);
	
	fd = udp_open(fd);
	addrlen = sizeof(serveraddr);
	udp_send(6);
	udp_receive(6);
	udp_close(fd);
	
	return 0;
}
