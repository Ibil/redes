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

char udp_hostname[] = "localhost";
#define DEFAULT_PORT 58018

char ECSname[50];
int ECSport;
int SID;
char *terminal_input;

int fd;
struct hostent *hostptr;
struct sockaddr_in serveraddr;
int addrlen;

extern int errno;
/* 192.168.128.1 */

char *buffer_tn;
#define MAXBUFFSIZE 2580


int udp_open(int fd){
	fd = socket(AF_INET, SOCK_DGRAM,0);
	
	/* meter "localhost" para testes*/
	hostptr=gethostbyname("localhost");
	
	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=((struct in_addr*)(hostptr->h_addr_list[0]))->s_addr;
	serveraddr.sin_port=htons((int)DEFAULT_PORT);
	return fd;
}

void udp_send(int nbytestosend, char* mensagem){
	printf("VOu enviar a mensagem: %s", mensagem);
	sendto(fd,mensagem, nbytestosend*sizeof(char), 0, (struct sockaddr*)&serveraddr, addrlen);
	printf("Mensagem enviada\n");
	return;
}

void udp_receive(int nbytestoread){
	printf("Pronto a receber\n");
	recvfrom(fd, buffer_tn, nbytestoread*sizeof(char),0, (struct sockaddr*) &serveraddr, &addrlen);
	printf("Recebi resposta: %s", buffer_tn);
	return;
}

void udp_close(int fd){
	close(fd);
	return;
}

void limpa_buffer(char* buffer, int tamanho){
	int i;
	for(i=0;i<tamanho;i++){
		buffer[i] = '\0';
	}
	return;
}

int udp_list(){

	fd = udp_open(fd);
	addrlen = sizeof(serveraddr);
	udp_send(4, "TQR\n\0");
	
	buffer_tn = (char*)malloc(MAXBUFFSIZE*sizeof(char));
	limpa_buffer(buffer_tn, MAXBUFFSIZE);
	udp_receive(MAXBUFFSIZE);	

	udp_close(fd);
	
	printf("%s", buffer_tn); /* o /n ja vem dentro do buffer*/
	
	/* falta detectar erros*/
	/*
	if(buffer_tn[5]==' '){
		nt_max = atoi(buffer_tn[4]);
	}else{
		nt_max = atoi(buffer_tn[4])*10 + atoi(buffer_tn[5]);
	}*/
	
	
	
	/*usar string token*/
	
	free (buffer_tn);
	return 0;
}

void udp_request(char* input){
	int Tn;
	char *msg;
	
	/* trata input */
	scanf("%d", &Tn);
	if(Tn<10){
		msg=(char*)malloc(7*sizeof(char));
		strcpy(msg, "TER ");
		msg[4] = Tn + '0';
		msg[5] = '\n';
		msg[6] = '\0';
	}
	else{
		msg=(char*)malloc(8*sizeof(char));
		strcpy(msg, "TER ");
		msg[4] = (Tn/10) + '0';
		msg[5] = (Tn%10) + '0';
		msg[6] = '\n';
		msg[7] = '\0';
	}
	
	/* envia TER */
	fd = udp_open(fd);
	addrlen = sizeof(serveraddr);
	udp_send(strlen(msg), msg);
	
	/* recebe AWTES ip port */
	buffer_tn = (char*)malloc(MAXBUFFSIZE*sizeof(char));
	limpa_buffer(buffer_tn, MAXBUFFSIZE);
	udp_receive(MAXBUFFSIZE);	

	printf("Recebi mensagem AWTES: %s\n", buffer_tn);
	
	printf("falta guardar ip e porta em variavel global\n");
	
	free (buffer_tn);
	udp_close(fd);
	
	return;
}


void trata_args(char* arg1, char* arg2){
	if(!strcmp(arg1,"-n")){
		strcpy(ECSname,arg2);
		ECSport = DEFAULT_PORT;
		return;
	}
	if(!strcmp(arg1,"-p")){
		strcpy(ECSname,"localhost");
		ECSport= atoi(arg2);
		return;
	}
	else{
		exit(-1);
	}
	return;
}

int main(int argc, char **argv){
	if(argc == 1){
		printf("Falta o SID\n");
		exit(1);
	}
	SID = atoi(argv[1]);
	switch(argc){
		case 2 : strcpy(ECSname,"localhost");ECSport = DEFAULT_PORT;
			      break;
		case 4 : trata_args(argv[2],argv[3]);
			      break;
		case 6 : !strcmp(argv[2],"-n") ? strcpy(ECSname,argv[3]) : exit(-1);
		         !strcmp(argv[4],"-p") ? ECSport= atoi(argv[5]) : exit(-1);
		         break;
		default:printf("Erro no número de argumentos");
	}
	terminal_input = (char*)malloc(50*sizeof(char));
	while(1){
		limpa_buffer(terminal_input, 50);
		printf("Escolha uma das opções possiveis:\n\t list\n\t request\n\t submit\n\t exit\n");
		scanf("%s", terminal_input);
		if(!strcmp(terminal_input, "list")){
			udp_list();
		}
		if(!strcmp(terminal_input, "request")){
			udp_request(terminal_input);
			/*tcp_RQT();*/
			printf("Falta fazer o tcp_RQT a ligar ao TES\n");
		}
		if(!strcmp(terminal_input, "exit")){
			break;
		}
	}
	return 0;
}
