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

char udp_buffer[100];

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
		t_number++){
	
		printf("Tnn : %d \t TES_ip : %s\t TES_Port : %s\n", t_number, TES_ip, TES_port);
	}
	if(t_number < 10){
		udp_buffer[4] =  t_number + '0';
		indice_buffer = 5;
	}
	else{
		udp_buffer[4] =  t_number/10 + '0';
		udp_buffer[5] =  t_number%10 + '0';
		indice_buffer = 6;
	}
	
	/* vai passar as linhas para o buffer uma a uma*/
	for(t_number = 1;
		1 < fscanf(fp_txt,"%s %s %s",TES_name, TES_ip, TES_port);
		t_number++){
		
		name_size = strlen(TES_name);
		ip_size = strlen(TES_ip);
		port_size = strlen(TES_port);
		
		/* poe o nome*/
		udp_buffer[indice_buffer]= ' ';
		indice_buffer++;
		for( i = 0; i < name_size; i++){
			udp_buffer[indice_buffer+i]= TES_name[i];
		}
		indice_buffer += name_size;
		
		/* por o ip*/
		udp_buffer[indice_buffer]= ' ';
		indice_buffer++;
		
		for( i = 0; i < ip_size; i++){
			udp_buffer[indice_buffer+i]= TES_ip[i];
		}
		indice_buffer += name_size;
		
		/* por a porta*/
		udp_buffer[indice_buffer]= ' ';
		indice_buffer++;
		
		for( i = 0; i < port_size; i++){
			udp_buffer[indice_buffer+i]= TES_port[i];
		}
		indice_buffer += name_size;
		
		printf("Tnn : %d \t TES_ip : %s\t TES_Port : %s\n", t_number, TES_ip, TES_port);
	}
	udp_buffer[indice_buffer] = '\n';
	udp_buffer[indice_buffer+1] = '\0';
	
	fclose(fp_txt);
	
}

void udp_trata_mensagem(){
	printf("recebi\n");
	if(!strcmp(udp_buffer,"TQR\n")){
		strcpy(udp_buffer,"AWT ");
		copia_Tnames();
	}
	else{
		printf("ERR: Pedido invalido\n");
	}
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
		fd = udp_open_socket(fd);
		printf("abri\n");
		addrlen = sizeof(clientaddr);
	
		udp_receive(250);
		udp_trata_mensagem();
		udp_send(strlen(udp_buffer));
	
		udp_close(fd);
	}
	return 0;
}
