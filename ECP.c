#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#define DEFAULT_PORT 58011

char ECSname[50];
int ECSport;
char terminal_input[50];

int fd;
struct hostent *hostptr;
struct sockaddr_in serveraddr;
int addrlen;

extern int errno;
/* 192.168.128.1 */

char buffer_tn[2580];


int udp_open(int fd){
	fd = socket(AF_INET, SOCK_DGRAM,0);
	
	/* meter "localhost" para testes*/
	hostptr=gethostbyname("tejo");
	
	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=((struct in_addr*)(hostptr->h_addr_list[0]))->s_addr;
	serveraddr.sin_port=htons((int)DEFAULT_PORT);
	return fd;
}

void udp_send(int nbytestosend){
	printf("VOu enviar a mensagem: \n");
	sendto(fd,"TQR\n", nbytestosend*sizeof(char), 0, (struct sockaddr*)&serveraddr, addrlen);
	printf("Mensagem enviada\n");
	return;
}

void udp_receive(int nbytestoread){
	printf("Pronto a receber\n");
	recvfrom(fd, buffer_tn, nbytestoread*sizeof(char),0, (struct sockaddr*) &serveraddr, &addrlen);
	printf("Recebi resposta: \n");
	return;
}

void udp_close(int fd){
	close(fd);
	return;
}


int main(int argc, char **argv){
	switch(argc){
		case 1 : ECSport = DEFAULT_PORT;
			     break;
		case 3 : !strcmp(argv[1],"-p") ? ECSport= atoi(argv[2]) : exit(-1);
		         break;
		default:printf("Erro no número de argumentos");
	}
	while(1){
		printf("QQ coisa\n");
		break;
	}
	return 0;
}
