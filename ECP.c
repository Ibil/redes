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

#define DEFAULT_PORT 58018

char ECSname[50];
int ECSport = DEFAULT_PORT;

int fd;
struct sockaddr_in serveraddr, clientaddr;
int addrlen;

char* udp_buffer;

extern int errno;


int udp_open_socket(int fd){
	fd = socket(AF_INET, SOCK_DGRAM,0);
	
	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
	serveraddr.sin_port=htons((int)ECSport);
	
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
	printf("VOu enviar resposta:\n");
	sendto(fd,udp_buffer, nbytestosend*sizeof(char), 0, (struct sockaddr*)&clientaddr, addrlen);
	printf("Resposta enviada \n");
	return;
}

void udp_close(int fd){
	close(fd);
	return;
}

void copia_Tnames(){
	FILE *fp_txt;
	int t_number;
	
	char TES_name[25];
	char TES_ip[15];
	char TES_port[6];
	int indice_buffer;
	int name_size, ip_size, port_size;
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


void udp_trata_mensagem(){
	if(!strcmp(udp_buffer,"TQR\n")){
		strcpy(udp_buffer,"AWT ");
		copia_Tnames();
	}
	if(!strcmp(udp_buffer,"TER")){
		copia_TES();
	}
	return;
}


int main(int argc, char **argv){
	int i;
	switch(argc){
		case 1 : ECSport = DEFAULT_PORT;
			     break;
		case 3 : !strcmp(argv[1],"-p") ? ECSport= atoi(argv[2]) : exit(-1);
		         break;
		default:printf("Erro no número de argumentos");
	}
	
	while(1){
		udp_buffer = (char*)malloc(250*sizeof(char));
		for(i= 0; i<250;i++){
			udp_buffer[i] = '\0';
		}
		
		fd = udp_open_socket(fd);
		addrlen = sizeof(clientaddr);
	
		udp_receive(250);
		udp_trata_mensagem();
		udp_send(strlen(udp_buffer));
	
		udp_close(fd);
		
		free(udp_buffer);
	}
	return 0;
}
