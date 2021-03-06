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

#define DEFAULT_PORT 58018
char udp_hostname[] = "localhost";

char ECSname[50];
int ECSport;
int SID;
char s_quest_ID[25];
char *terminal_input;

int fd, fd2;
struct hostent *hostptr;
struct hostent *hostptr2;
struct sockaddr_in serveraddr;
struct sockaddr_in serveraddr2;
int addrlen, addrlen2, nleft, nwritten, nread;
int err = 0;

extern int errno;
/* 192.168.128.1 */

char *buffer_tn;
char *buffer_rqt;
char *ip_tes, *port_tes;
char *tes_rqt;
#define MAXBUFFSIZE 2580


int conta_digitos_int(int numero){ /*Conta o numero de digitos de um numero*/
	int n_digitos;
	for( n_digitos = 1; numero >= 10; n_digitos++){
		numero = numero/10;
	}
	return n_digitos;
}

int udp_open(int fd){ /*Funcao que trata de abrir uma ligacao UDP*/
	fd = socket(AF_INET, SOCK_DGRAM,0);
	hostptr=gethostbyname(ECSname);
	
	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=((struct in_addr*)(hostptr->h_addr_list[0]))->s_addr;
	serveraddr.sin_port=htons((int)ECSport);
	return fd;
}

void udp_send(int nbytestosend, char* mensagem){ /*Funcao que envia uma mensagem em UDP*/
	sendto(fd,mensagem, nbytestosend*sizeof(char), 0, (struct sockaddr*)&serveraddr, addrlen);
	return;
}

void udp_receive(int nbytestoread){ /*Funcao que recebe uma mensagem em UDP*/
	recvfrom(fd, buffer_tn, nbytestoread*sizeof(char),0, (struct sockaddr*) &serveraddr, &addrlen);
	return;
}

void udp_close(int fd){ /*Funcao que fecha uma ligacao UDP*/
	close(fd);
	return;
}

int tcp_connect(int fd_param){ /*Funcao que inicia uma ligacao TCP*/
	fd_param = socket(AF_INET,SOCK_STREAM,0);
	fflush(stdout);
	hostptr2 = gethostbyname(ip_tes);
	
	memset((void*)&serveraddr2, (int)'\0',sizeof(serveraddr2));
	serveraddr2.sin_family = AF_INET;
	serveraddr2.sin_addr.s_addr = ((struct in_addr *)(hostptr2->h_addr_list[0]))->s_addr;
	serveraddr2.sin_port = htons((short int)atoi(port_tes));

	connect(fd_param,(struct sockaddr*)&serveraddr2,sizeof(serveraddr2));
   return fd_param;
}

void tcp_write(char* msg, int nbytestowrite){ /*Funcao que envia uma mensagem por TCP*/
	char *ptr;
	ptr = msg;   
	nleft=nbytestowrite;
	while(nleft > 0)	{
		if( (nwritten=write(fd,ptr,nleft)) == -1){
			printf("Erro no write.\n");
			exit(1);
		}
		nleft-=nwritten;
		ptr+=nwritten;
	}
	return;
}

void tcp_read(char *msg, int nbytestoread){ /*Funcao que le uma mensagem com um tamanho fixo por TCP*/
	char *ptr;
	nleft=nbytestoread;
	ptr=msg;
	while(nleft>0){
		if((nread = read(fd,ptr,nleft)) == -1){
			printf("Erro a ler a resposta\n");
			exit(1);
		}
		nleft-=nread;
		ptr+=nread;
	}
	return;
}

void tcp_read_alt(char *msg){ /*Funcao que le uma mensagem ate ao caracter ' ' por TCP*/
	char *ptr_temp;
	ptr_temp=&msg[0];
	
	
	do{
		if((nread = read(fd2,ptr_temp,1)) == -1){
			printf("Erro a ler a mensagem\n");
			exit(1);
		}
		ptr_temp+=nread;
	}
	while( (*(ptr_temp-1))!=' ' );
	*(ptr_temp-1)=0;
	return;
}

void tcp_read_alt_n(char *msg){		/*Funcao que le uma mensagem ate ao caracter '\n' por TCP*/
	char *ptr_temp;
	ptr_temp=&msg[0];
	
	
	do{
		if((nread = read(fd2,ptr_temp,1)) == -1){
			printf("Erro a ler a mensagem\n");
			exit(1);
		}
		ptr_temp+=nread;
	}
	while( (*(ptr_temp-1))!='\n' );
	return;
}

void tcp_close(int fd){ /*Funcao que fecha uma ligacao TCP*/
	close(fd);
}


void limpa_buffer(char* buffer, int tamanho){ /*Funcao que limpa uma string*/
	int i;
	for(i=0;i<tamanho;i++){
		buffer[i] = '\0';
	}
	return;
}

int udp_list(){ /*Funcao que trata o pedido de "list" do utilizador*/

	int contador, n_top;
	char *palavra;

	fd = udp_open(fd);
	addrlen = sizeof(serveraddr);
	udp_send(4, "TQR\n\0");
	
	buffer_tn = (char*)malloc(MAXBUFFSIZE*sizeof(char));
	limpa_buffer(buffer_tn, MAXBUFFSIZE);
	udp_receive(MAXBUFFSIZE);	

	udp_close(fd);
	
	palavra = strtok(buffer_tn, "\n"); /*Verifica se a mensagem e EOF ou ERR*/

	if(!strcmp(palavra, "EOF")){
		printf("Nao ha questionarios disponiveis.\n");
		exit(1);
	}
	else{
		if(!strcmp(palavra, "ERR")){
			printf("Pedido mal formulado.\n");
			return 0;
		}
	}

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


void udp_request(char* input){ /*Funcao que trata da parte UDP do pedido de "request" do utilizador*/
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

	strtok(buffer_tn, "\n"); /*Verifica se a mensagem e EOF*/
	if(!strcmp("EOF", buffer_tn)){
		err = 1;
		printf("Numero de topico invalido, tente de novo.\n");
		return;
	}
	
	strtok(buffer_tn, " ");
	ptr = strtok(NULL, " ");
	ip_tes = (char*)malloc( strlen(ptr) * sizeof(char) );
	strcpy(ip_tes,ptr);
	
	ptr = strtok(NULL, " ");
	port_tes = (char*)malloc( strlen(ptr) * sizeof(char) );
	strcpy(port_tes,ptr);
	
	
	free (buffer_tn);
	udp_close(fd);
	
	return;
}

void tcp_RQT(){ /*Funcao que trata da parte TCP do pedido de "request" do utilizador*/
	char tes_rqt[255];
	char tes_aqt[255];
	char deadline[19];
	char s_file_size[255];
	long int file_size;
	char *data;
	char caixote[1];
	char file_name[30];
	
	FILE *f;
	
	limpa_buffer(tes_rqt, 255);
	limpa_buffer(tes_aqt, 255);
	limpa_buffer(s_quest_ID, 25);
	limpa_buffer(s_file_size, 255);
	limpa_buffer(deadline, 19);
	limpa_buffer(file_name, 30);
	
	fd2 = tcp_connect(fd2);

	/* envia <RQT SID\n> */
	sprintf(tes_rqt, "RQT %d\n", SID);
	tcp_write(tes_rqt, 10);


	/* Le o <AQT QID TIME SIZE DATA\N> */
	tcp_read(tes_aqt, 4);
	if( !strcmp("AQT ", tes_aqt)){
		tcp_read_alt(s_quest_ID);	/* Le o QID e grava */
		tcp_read(deadline, 18);		/*Le o time */
		tcp_read(caixote, 1);		/* le ' ' */
		tcp_read_alt(s_file_size);	/*Le o tamanho do ficheiro*/
		file_size = atoi(s_file_size);
		
		data = (char*)malloc(file_size*sizeof(char));
		tcp_read(data, file_size);
		sprintf(file_name, "%s.pdf", s_quest_ID);
		f = fopen(file_name, "w");
		if(f==NULL){
			printf("Error opening file.\n");
			exit(1);
		}
		fwrite(data, sizeof(char), file_size, f);
		fclose(f);
				

		
		tcp_read(caixote, 1); /* le '\n' */
		
	}
	else{	/*ERR*/
		if(!strcmp("ERR\n", tes_aqt)){
			printf("Pedido mal formulado.\n");
		}
	} 

	tcp_close(fd2);
	printf("Received file %s.pdf\n", s_quest_ID);
}

void tcp_submit(char* input){ /*Funcao que trata do pedido de "submit" do utilizador*/
	char answers[11];
	char answers_b[11];
	char rqs_msg[255];
	char aqs_msg[255];
	char s_quest_ID_recvd[25];
	char s_score[4];
	char s_SID[5];
	char temp_qid[25];
	int i, qid_len, msg_len, score;	

	limpa_buffer(temp_qid, 25);
	limpa_buffer(s_score, 4);
	limpa_buffer(s_quest_ID_recvd, 25);
	limpa_buffer(answers, 11);
	limpa_buffer(aqs_msg, 255);
	limpa_buffer(rqs_msg, 255);

	fd2 = tcp_connect(fd2);

	getchar();
	for(i=0;i<10;i=i+1){
		answers[i] = getchar(); 
	}
	answers[9] = '\n';
	answers[10] = '\0';
	
	sprintf(rqs_msg, "RQS %d %s %s", SID, s_quest_ID, answers);
	
	strcpy(answers_b,answers);
	
	qid_len = strlen(s_quest_ID);
	sprintf(s_SID, "%d", SID);
	sprintf(temp_qid, "%s", s_quest_ID);
	
	tcp_write("RQS ", 4);
	tcp_write(s_SID, 5);
	tcp_write(" ", 1);
	tcp_write(temp_qid, qid_len);
	tcp_write(" ", 1);
	tcp_write(answers_b, 10);
	

	/*LE AQS QID SCORE*/
	tcp_read(aqs_msg, 4);
	if( !strcmp("AQS ", aqs_msg)){
		tcp_read_alt(s_quest_ID_recvd);
		tcp_read_alt_n(s_score);
		if(s_score[0] == '-'){
			printf("Tempo limite de submissao de resposta excedido.\n");
			return;
		}
		else{
			score = atoi(s_score);
			printf("Obtained score: %d%%\n", score);
		}
	}
	else{  /*ERR*/
		printf("Pedido mal formulado, tente de novo.\n");
	}

	tcp_close(fd2);
	return;
}

void trata_args(char* arg1, char* arg2){ /*Funcao que trata os argumentos com que o programa e chamado*/
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
			continue;
		}
		if(!strcmp(terminal_input, "request")){
			udp_request(terminal_input);
			if(err == 1)
				continue;
			tcp_RQT();
			continue;
		}
		if(!strcmp(terminal_input, "submit")){
			tcp_submit(terminal_input);
			continue;
		}
		if(!strcmp(terminal_input, "exit")){
			break;
		}
		printf("Pedido mal formulado, tente de novo.\n");
	}
	return 0;
}
