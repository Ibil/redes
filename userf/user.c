#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
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
int SID, questID;
char *terminal_input;

int fd, fd2;
struct hostent *hostptr;
struct hostent *hostptr2;
struct sockaddr_in serveraddr;
struct sockaddr_in serveraddr2;
int addrlen, addrlen2, nleft, nwritten, nread;

extern int errno;
/* 192.168.128.1 */

char *buffer_tn;
char *buffer_rqt;
char *ip_tes, *port_tes;
char *tes_rqt;
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

int tcp_connect(int fd_param){
	printf("ENtrei no connect\n");
	fd_param = socket(AF_INET,SOCK_STREAM,0);
	printf("fiz socket\n");
	fflush(stdout);
	/*printf("ip_tes: %s", ip_tes);*/
	printf("antes do gethost\n");
	hostptr2 = gethostbyname(ip_tes);
	
	printf("fiz gethost\n");
	
	memset((void*)&serveraddr2, (int)'\0',sizeof(serveraddr2));
	serveraddr2.sin_family = AF_INET;
	serveraddr2.sin_addr.s_addr = ((struct in_addr *)(hostptr2->h_addr_list[0]))->s_addr;
	printf("atoi(port_tes): %d\n", atoi(port_tes));
	serveraddr2.sin_port = htons((short int)atoi(port_tes));

	connect(fd_param,(struct sockaddr*)&serveraddr2,sizeof(serveraddr2));
   printf("connect aceite\n");
   return fd_param;
}

void tcp_write(char* msg, int nbytestowrite){
	char *ptr;
	ptr = msg;   
	nleft=nbytestowrite;
   printf("vou enviar mensagem: %s\n", ptr);
	while(nleft > 0)	{
		if( (nwritten=write(fd,msg,nleft)) == -1){
			printf("erro no write\n");
			exit(1);
		}
		nleft-=nwritten;
		ptr+=nwritten;
	}
	printf("Mensagem Enviada\n");
	return;
}

void tcp_read(char *msg, int nbytestoread){
	char *ptr;
	nleft=nbytestoread;
	/* vou escrever por cima que nao faz mal neste exemplo*/
	ptr=msg;
	printf("Vou ler a resposta\n");
	while(nleft>0){
		if((nread = read(fd,ptr,nleft)) == -1){
			printf("Erro a ler a resposta\n");
			exit(1);
		}
		nleft-=nread;
		ptr+=nread;
	}
	printf("A resposta lida foi : %s\n", buffer_rqt);
	return;
}

void tcp_read_alt(char *msg){
	char *ptr_temp;
	
	printf("Vou ler a mensagem\n");
	ptr_temp=&msg[0];
	
	
	do{
		if((nread = read(fd2,ptr_temp,1)) == -1){
			printf("Erro a ler a mensagem\n");
			exit(1);
		}
		printf("O char : %c\n", (*ptr_temp));
		ptr_temp+=nread;
	}
	while( (*(ptr_temp-1))!=' ' );
	
	puts("SAI!");
	printf("recebi a mensagem : %s\n", msg);
	return;
}

void tcp_close(int fd){
	close(fd);
}


void limpa_buffer(char* buffer, int tamanho){
	int i;
	for(i=0;i<tamanho;i++){
		buffer[i] = '\0';
	}
	return;
}

int udp_list(){

	int contador, n_top;
	char *palavra;

	fd = udp_open(fd);
	addrlen = sizeof(serveraddr);
	udp_send(4, "TQR\n\0");
	
	buffer_tn = (char*)malloc(MAXBUFFSIZE*sizeof(char));
	limpa_buffer(buffer_tn, MAXBUFFSIZE);
	udp_receive(MAXBUFFSIZE);	

	udp_close(fd);
	
	palavra = strtok(buffer_tn, " ");
	palavra = strtok(NULL, " ");

	contador = 1;	
	
	for(n_top=atoi(palavra); n_top>0; n_top--,contador++){
		palavra = strtok(NULL, " ");
		printf("%d- %s\n",contador,palavra);
	}
	
	free (buffer_tn);
	return 0;
}

void udp_request(char* input){
	int Tn;
	char *msg;
	char *ptr;

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

	strtok(buffer_tn, " ");
	
	ptr = strtok(NULL, " ");
	ip_tes = (char*)malloc( strlen(ptr) * sizeof(char) );
	strcpy(ip_tes,ptr);
	
	ptr = strtok(NULL, " ");
	port_tes = (char*)malloc( strlen(ptr) * sizeof(char) );
	strcpy(port_tes,ptr);

	/*printf("Recebi mensagem AWTES: %s\n", buffer_tn);*/

	printf("Ip:\t %s Port:\t %s", ip_tes, port_tes);
	
	
	
	
	free (buffer_tn);
	udp_close(fd);
	
	return;
}

void tcp_RQT(){
	char tes_rqt[255];
	char tes_aqt[255];
	char *s_quest_ID;
	char *deadline;
	char s_file_size[255];
	long int file_size;
	char *data;
	fd2 = tcp_connect(fd2);

	limpa_buffer(tes_aqt, 255);
	
	printf("antes do sprintf\n");
	sprintf(tes_rqt, "RQT %d\n", SID);

	printf("antes do write\n");
	tcp_write(tes_rqt, 10);

	tcp_read(tes_aqt, 4);
	if( !strcmp("AQT ", tes_aqt)){
		/*mensagem AQT QID time size data*/
		/* Le o QID e grava */
		tcp_read(s_quest_ID, 5);
		questID = atoi(s_quest_ID);
		tcp_read(s_quest_ID, 1);
		/*Le a data*/
		tcp_read(deadline, 18);
		
		/*Le o tamanho do ficheiro*/
		tcp_read_alt(s_file_size);
		file_size = atoi(s_file_size);

		/*Aloca memória para o ficheiro e le-o*/
		data = (char*)malloc(file_size*sizeof(char));
		tcp_read(data, file_size);
		
		/*Adeus \n*/
		tcp_read(s_quest_ID, 1);
	}
	else printf("houve merda\n");

	tcp_close(fd2);

	printf("Questionario %d.pdf recebido.\nTem ate %s para responder.", questID, deadline);
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
			tcp_RQT();
			printf("Falta fazer o tcp_RQT a ligar ao TES\n");
		}
		if(!strcmp(terminal_input, "exit")){
			break;
		}
	}
	return 0;
}
