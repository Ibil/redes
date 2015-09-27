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

#define DEFAULT_PORT 59000

#define QID_SIZE 5

int TESport = DEFAULT_PORT;

int fd, newfd;
int nread, nwritten,nleft;
struct sockaddr_in serveraddr, clientaddr;
int addrlen, clientlen;

char* tcp_input_buffer;
char *ptr;

extern int errno;

static int QID_index = 0;

/* EStrutura do TES com resultados*/

typedef struct{
	int SID;
	int QID;
	int Score;
}quest_form;

quest_form db_quest_form[1000];

void cria_instancia_QID(int sid){
	db_quest_form[QID_index].SID = sid;
	db_quest_form[QID_index].QID = QID_index;
	db_quest_form[QID_index].Score = 101;    /* 101 simboliza -1*/
	QID_index++;
}

/* fim de estrutura*/


/* ################   FUncoes UDP ################*/
/*
int udp_open_socket(int fd){
	fd = socket(AF_INET, SOCK_DGRAM,0);
	
	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
	serveraddr.sin_port=htons((int)TESport);
	
	if (bind(fd,(struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1){
		printf("Erro no bind : \n");
		exit(1);
	}
	return fd;
}

void udp_receive(int nbytestoreceive){
	printf("Pronto a receber\n");
	recvfrom(fd, udp_buffer, nbytestoreceive*sizeof(char),0, (struct sockaddr*) &clientaddr, &addrlen);
	printf("Mensagem recebida: %s\n", udp_buffer);
	return;
}

void udp_send(int nbytestosend){
	printf("VOu enviar resposta: %s\n", udp_buffer);
	sendto(fd,udp_buffer, nbytestosend*sizeof(char), 0, (struct sockaddr*)&clientaddr, addrlen);
	printf("Resposta enviada \n");
	return;
}

void udp_close(int fd){
	close(fd);
	return;
}
*/
/* ################   fim FUncoes UDP ################*/

/* ################   FUncoes tcp ################*/

void tcp_connect(){
	if ((fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
		printf("Erro no socket : \n");
		exit(1);
	}
	memset((void*) &serveraddr, (int)'\0',sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
	serveraddr.sin_port=htons((short int)TESport);
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

/*void tcp_read(char* buffer, int nbytestoread){
	printf("Vou ler a mensagem\n");
	ptr=buffer;
	do{
		if((nread = read(newfd,ptr,1)) == -1){
			printf("Erro a ler a mensagem\n");
			exit(1);
		}
		printf("O char : %c\n", (*ptr));
		ptr+=nread;
	}
	while( (*(ptr-1))!='\n' );
	
	printf("recebi a mensagem : %s\n", buffer);
	return;
}*/

void tcp_read(char *buffer, int nbytestoread){
	char *ptr;
	nleft=nbytestoread;
	/* vou escrever por cima que nao faz mal neste exemplo*/
	ptr=buffer;
	printf("Vou ler a resposta\n");
	while(nleft>0){
		if((nread = read(newfd,ptr,nleft)) == -1){
			printf("Erro a ler a resposta\n");
			exit(1);
		}
		nleft-=nread;
		ptr+=nread;
	}
	printf("A resposta lida foi : %s\n", buffer);
	return;
}

void tcp_write(char* buffer, int nbytestowrite){
	printf("vou enviar a resposta: %s\n", buffer);
	ptr = buffer;
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

/* ################   fim FUncoes tcp ################*/

void limpa_buffer(char* buffer, int tamanho){
	int i;
	for(i=0;i<tamanho;i++){
		buffer[i] = '\0';
	}
	return;
}



int conta_digitos_int(int numero){
	int n_digitos;
	for( n_digitos = 1; numero > 10; n_digitos++){
		numero = numero/10;
	}
	return n_digitos;
}
int conta_digitos_long_int(long int numero){
	int n_digitos;
	for( n_digitos = 1; numero > 10; n_digitos++){
		numero = numero/10;
	}
	return n_digitos;
}


long int get_file_size(){
	FILE *fp_pdf;

	long int new_pos;
	
	fp_pdf = fopen("aula2.pdf", "r+");
	fseek(fp_pdf, 0,SEEK_END);
	new_pos = ftell(fp_pdf);

	fclose(fp_pdf);
	return new_pos;
}

char* get_dados(char* dados){
	FILE *fp_pdf;
	
	fp_pdf = fopen("aula2.pdf", "r+");
	while(0 < fscanf(fp_pdf,"%s",dados)){
		printf("Fazer: mandar linha a linha\n");
		
	}

	fclose(fp_pdf);
	return dados;
}


void tcp_envia_AQT(){
	long int file_size;
	char s_QID[5];
	char *dados;
	int n_digitos;
	char *s_size_of_data;
	
	int contador = 0;
	
	tcp_write("AQT ", 4);
	
	sprintf(s_QID,"%d", QID_index);
	tcp_write(s_QID, conta_digitos_int(QID_index));
	tcp_write(" ", 1);
	
	
	/* fazer set_time
	time: DDMMMYYYY_HH:MM:SS
		  09JAN2016_20:00:00
	*/
	printf("que raio e set_time???\n");
	tcp_write("09JAN2016_20:00:00", 18);
	tcp_write(" ", 1);
	
	
	file_size = get_file_size();
	n_digitos = conta_digitos_long_int(file_size);

	s_size_of_data = (char*)malloc(n_digitos * sizeof(char));
	sprintf(s_size_of_data, "%ld", file_size);
	
	tcp_write( s_size_of_data, n_digitos);
	tcp_write(" ", 1);
	
	
	/*dados=(char*) malloc(file_size*sizeof(char));
	limpa_buffer(dados,file_size);
	printf("TIrar o comment do get dados\n");
	dados = get_dados(dados);
	tcp_write(dados,file_size);
	free(dados);
	*/
	
	
	tcp_write("\n", 1);
	return;	
}


void tcp_trata_mensagem(){
	/* 	
	-(INPUT)->  RQT SID\n  
	<-(OUTPUT)- AQT QID time size data\n 
				time: DDMMMYYYY_HH:MM:SS
					  09JAN2015_20:00:00
	
	-(INPUT)->  RQS SID V1 V2 V3 V4 V5\n
	<-(OUTPUT)- AQS QID score\n	
	*/
	
	int studID;
	char s_stud_ID[5];
	
	tcp_input_buffer = (char*)malloc(sizeof(char)* 10+ QID_SIZE + 12);
	limpa_buffer(tcp_input_buffer, 27);
	
	tcp_read(tcp_input_buffer, 4);
	printf("buffer: %s", tcp_input_buffer);
	if( !strcmp("RQT ", tcp_input_buffer)){
		/* Le o SID e grava */
		printf("entrei no RQT\n");
		tcp_read(s_stud_ID, 5);
		studID =atoi(s_stud_ID);
		/* come o '\n'*/
		tcp_read(tcp_input_buffer, 1);
		
		cria_instancia_QID(studID);
		printf("tirar comment do tcp_envia_aqt\n");
		tcp_envia_AQT();
	}
	else{
		printf("Fazer o RQS!!\n");
	}
	
	free(tcp_input_buffer);
	return;
}


void tcp_TES(){
	while(1){
		tcp_connect();

		tcp_trata_mensagem();
	
		tcp_close();
	}
}


int main(int argc, char **argv){
	int ppid;
	switch(argc){
		case 1 : TESport = DEFAULT_PORT;
			     break;
		case 3 : !strcmp(argv[1],"-p") ? TESport= atoi(argv[2]) : exit(-1);
		         break;
		default:printf("Erro no número de argumentos");
	}
	
	ppid = fork();
	if (ppid < 0){
		printf("Erro a fazer o fork para SSs\n");
		exit(1);
		}
	else if (ppid == 0){
		/*filho udp*/
		/*udp_TES();*/
	}
	else{
		/*pai tcp*/
		tcp_TES();
	}
	return 0;
}
