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
struct hostent *h;
int addrlen;

char* udp_buffer;

extern int errno;

int max_top;


void limpa_buffer(char* buffer, int tamanho){
	int i;
	for(i=0;i<tamanho;i++){
		buffer[i] = '\0';
	}
	return;
}

int udp_open_socket(int fd){
	fd = socket(AF_INET, SOCK_DGRAM,0);
	
	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
	serveraddr.sin_port=htons((int)ECSport);
	
	if (bind(fd,(struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1){
		printf("Erro no bind.\n");
		exit(1);
	}
	return fd;
}

void udp_receive(int nbytestoreceive){
	recvfrom(fd, udp_buffer, nbytestoreceive*sizeof(char),0, (struct sockaddr*) &clientaddr, &addrlen);
	return;
}

void udp_send(int nbytestosend){
	sendto(fd,udp_buffer, nbytestosend*sizeof(char), 0, (struct sockaddr*)&clientaddr, addrlen);
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
	int indice_buffer = 0;
	int name_size;
	int i;
	
	fp_txt = fopen("topics.txt", "r+");
	/* ve quantos topicos ha */
	for(t_number = 1;
		1 < fscanf(fp_txt,"%s %s %s",TES_name, TES_ip, TES_port);
		t_number++);
	
	max_top = t_number-1;
		
	t_number--; /* pois o for incrementa antes de sair*/
	
	if(t_number == 0){
		udp_buffer[indice_buffer] = 'E';	
		indice_buffer++;

		udp_buffer[indice_buffer] = 'O';	
		indice_buffer++;

		udp_buffer[indice_buffer] = 'F';	
		indice_buffer++;	

	}
	else{ 
		strcpy(udp_buffer,"AWT ");
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
			
			/* DEixar o printf a seguir */
			printf("%s %s\n",TES_ip,TES_port);
			
			name_size = strlen(TES_name);
			
			/* poe o nome*/
			udp_buffer[indice_buffer]= ' ';
			indice_buffer++;
			for( i = 0; i < name_size; i++){
				udp_buffer[indice_buffer+i]= TES_name[i];
			}
			indice_buffer += name_size;
	    	} 
	
	}
	udp_buffer[indice_buffer] = '\n';
	udp_buffer[indice_buffer+1] = '\0';
	fclose(fp_txt);
	
}

void copia_TES(){
	int topic_index=0;
	int buffer_index;
	int t_number, i, ip_size, port_size;
	
	FILE *fp_txt;
	
	char TES_name[25];
	char TES_ip[15];
	char TES_port[6];
	
	/*
	input: "TER Tn\n\0"
	output: "AWTES ip port\n\0"
	*/
	
	/* Guardo o Topic_number*/
	if((udp_buffer[4] >='0' && udp_buffer[4]<='9') 
		&& udp_buffer[5] == '\n'){ /* ve se a mensagem acaba apos o primeiro digito*/
			topic_index = udp_buffer[4] - '0';
	}
	else{
		if(udp_buffer[5] >='0' && udp_buffer[5]<='9'){ /* Ve se o segundo digito e um numero */
			topic_index = (udp_buffer[4]-'0')*10 + (udp_buffer[5]-'0');
		}
		else{
			strcpy(udp_buffer, "ERR\n\0");
			return;
		}	
	}
	if((topic_index>max_top) || (topic_index < 1)){ 
		strcpy(udp_buffer,"EOF\n\0");
		return;
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
	
	fclose(fp_txt);
	return;
}

void receive_Stats(){
	FILE *fp_stats;
	int score;
	
	char *s_ID;
	char *q_ID;
	char *topic_name;
	char *s_score;
	char *caixote;
	char awi[29];
	char stat[250];
	
	
	caixote = strtok(udp_buffer, " ");
	s_ID = strtok(NULL, " ");
	q_ID = strtok(NULL, " ");
	topic_name = strtok(NULL, " ");
	s_score = strtok(NULL, " ");
	score = atoi(s_score);
	printf("%s, %s, %d%%\n", s_ID, topic_name, score);

	fp_stats = fopen("stats.txt", "r+");
	fseek(fp_stats, 0, SEEK_END);
	fprintf(fp_stats, "%s:%s:%s:%d\n", s_ID, q_ID, topic_name, score);
	fclose(fp_stats);
	
	/* para depois enviar por udp */
	sprintf(awi, "AWI %s\n", q_ID);
	limpa_buffer(udp_buffer, 250);
	strcpy(udp_buffer, awi);
	
	return;
}

void udp_trata_mensagem(){
	/*TRATA DO TER*/
	if( udp_buffer[0] == 'T' &&
		udp_buffer[1] == 'E' &&
		udp_buffer[2] == 'R' &&
		udp_buffer[3] == ' ')
	{
		copia_TES();
		return;
	}
	/*TRATA DO TQR*/	
	if(!strcmp(udp_buffer,"TQR\n")){
		/* DEixar o printf a seguir */
		h=gethostbyaddr((char*)&clientaddr.sin_addr,sizeof(struct in_addr),AF_INET);
		printf("List request: %s %hu\n", h->h_name ,ntohs(clientaddr.sin_port));
		copia_Tnames();
		return;
	}
	/*TRATA DO IQR SID QID topic_name score*/
	if(udp_buffer[0] == 'I' &&
	   udp_buffer[1] == 'Q' &&
	   udp_buffer[2] == 'R' &&
	   udp_buffer[3] == ' '){
		receive_Stats();
		return;
	}
	strcpy(udp_buffer, "ERR\n\0");	
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
		udp_buffer = (char*)malloc(250*sizeof(char));
		
		limpa_buffer(udp_buffer,250);
		
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
