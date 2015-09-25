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

int ECPport = DEFAULT_PORT;

int fd;
struct sockaddr_in serveraddr, clientaddr;
int addrlen;

char* tcp_input_buffer;

extern int errno;

static int QID_indez = 0;

/* EStrutura do TES com resultados*/

typedef struct{
	int SID;
	int QID;
	int Score;
}quest_form;

quest_form db_quest_form[1000];

void cria_instancia_QID(int sid){
	db_quest_form[QID_index].SID = sid;
	db_quest_form[QID_index].SID = sid;
	db_quest_form[QID_index].SID = sid;
	QID_index++;
}

/* fim de estrutura*/


/* ################   FUncoes UDP ################*/

int udp_open_socket(int fd){
	fd = socket(AF_INET, SOCK_DGRAM,0);
	
	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
	serveraddr.sin_port=htons((int)ECPport);
	
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

void tcp_read(char* buffer, int nbytestoread){
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

void copia_Tnames(){
	FILE *fp_txt;
	int t_number;
	
	char TES_name[25];
	char TES_ip[15];
	char TES_port[6];
	int indice_buffer;
	int name_size;
	int i;
	
	fp_txt = fopen("topics.txt", "r+");
	/* ve quantos topicos ha */
	for(t_number = 1;
		1 < fscanf(fp_txt,"%s %s %s",TES_name, TES_ip, TES_port);
		t_number++);
		
	t_number--; /* pois o for incrementa antes de sair*/
	
	if(t_number < 10){
		udp_buffer[4] =  t_number + '0';
		indice_buffer = 5;
	}
	else{
		udp_buffer[4] =  t_number/10 + '0';
		udp_buffer[5] =  t_number%10 + '0';
		indice_buffer = 6;
	}
	
	fseek(fp_txt, 0,SEEK_SET);
	/* vai passar as linhas para o buffer uma a uma*/
	for(t_number = 1;
		1 < fscanf(fp_txt,"%s %s %s",TES_name, TES_ip, TES_port);
		t_number++){
		
		name_size = strlen(TES_name);
		
		/* poe o nome*/
		udp_buffer[indice_buffer]= ' ';
		indice_buffer++;
		for( i = 0; i < name_size; i++){
			udp_buffer[indice_buffer+i]= TES_name[i];
		}
		indice_buffer += name_size;
	}
	udp_buffer[indice_buffer] = '\n';
	udp_buffer[indice_buffer+1] = '\0';
	fclose(fp_txt);
	
}

void copia_TES(){
	int topic_index;
	int buffer_index;
	int t_number, i, ip_size, port_size;
	
	FILE *fp_txt;
	
	char TES_name[25];
	char TES_ip[15];
	char TES_port[6];
	
	/*
	input: "TER Tn\n\0"
	outpu: "AWTES ip port\n\0"
	*/
	
	/* Guardo o Topic_number*/
	if(udp_buffer[5] == ' '){
		topic_index = udp_buffer[4];
	}
	else{
		topic_index = udp_buffer[4]*10 + udp_buffer[5];
	}
	strcpy(udp_buffer, "AWTES ");
	buffer_index = 6;
	
	fp_txt = fopen("topics.txt", "r+");
	

	/*Procura o topic_index*/	
	for(t_number = 1; t_number <= topic_index; t_number++){
		fscanf(fp_txt,"%s %s %s",TES_name, TES_ip, TES_port);
	}
	
	ip_size = strlen(TES_ip);
	port_size = strlen(TES_port);
	
	/* por o ip*/
	for( i = 0; i < ip_size; i++){
		udp_buffer[buffer_index+i]= TES_ip[i];
	}
	buffer_index += ip_size;
	
	/* por a porta*/
	udp_buffer[buffer_index]= ' ';
	buffer_index++;
	
	for( i = 0; i < port_size; i++){
		udp_buffer[buffer_index+i]= TES_port[i];
	}
	buffer_index += port_size;

	/* poe \n e \0 para funcionar bem*/
	udp_buffer[buffer_index] = '\n';
	udp_buffer[buffer_index+1] = '\0';
	
	printf("O udp_buffer com %d size, awtes : %s", strlen(udp_buffer),udp_buffer);
	fclose(fp_txt);
	return;
}


void tcp_envia_AQT(){
	strcpy(buffer, "AQT <resto>\n\0");
	printf("Definir como mandar o AQT\n");
	tcp_write(13);
	return;	
}


void tcp_trata_mensagem(){
	/* 	
	-(INPUT)->  RQT SID\n  
	<-(OUTPUT)- AQT QID time size data\n 
				time: DDMMMYYYY_HH:MM:SS
					  09JAN2015_20:00:00
		  NOTA: apanhar o QID e size com ciclo ate ler ' '
	
	-(INPUT)->  RQS SID V1 V2 V3 V4 V5\n
	<-(OUTPUT)- AQS QID score\n	
	*/

	tcp_input_buffer = (char*)malloc(sizeof(char)* 10+ QID_SIZE + 12);

	tcp_read(tcp_input_buffer, 4);
	
	if( !strcmp("RQT ", buffer)){
		printf("Mensagem entrou no rqt; strtok do numero;\n");
		tcp_read(6); /* le SID*5 + espaco + \n */
		
		cria_instancia_QID(studID);
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
	int i, ppid;
	switch(argc){
		case 1 : ECPport = DEFAULT_PORT;
			     break;
		case 3 : !strcmp(argv[1],"-p") ? ECPport= atoi(argv[2]) : exit(-1);
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
