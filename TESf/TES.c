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
#include <time.h>

#define DEFAULT_TES_PORT 59000
#define DEFAULT_ECP_PORT 58018

#define QID_SIZE 5

int TESport = DEFAULT_TES_PORT;
int ECPport = DEFAULT_ECP_PORT;
char ECPname[50];

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
	time_t t_registo;
	int Score;
}quest_form;

quest_form db_quest_form[1000];

int cria_instancia_QID(int sid){
	db_quest_form[QID_index].SID = sid;
	db_quest_form[QID_index].QID = QID_index;
	db_quest_form[QID_index].Score = 101;    /* 101 simboliza -1*/
	QID_index++;
	return QID_index - 1;
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

void tcp_read_alt(char *msg){
	char *ptr_temp;
	
	printf("Vou ler a mensagem\n");
	ptr_temp=&msg[0];
	
	
	do{
		if((nread = read(newfd,ptr_temp,1)) == -1){
			printf("Erro a ler a mensagem\n");
			exit(1);
		}
		printf("O char : %c\n", (*ptr_temp));
		ptr_temp+=nread;
	}
	while( (*(ptr_temp-1))!=' ' );
	
	puts("sair do read alt");
	printf("A resposta lida foi: %s\n", msg);
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

char* verifica_respostas(char* vec_respostas){
	FILE *fp_txt;
	
	fp_txt = fopen("1QF1A", "r");
	fread(vec_respostas,sizeof(char),9,fp_txt);
	
	fclose(fp_txt);
	
	return vec_respostas;
}

int verifica_score(char* input){
	int i = 0;
	char *respostas;
	int score = 0;
	
	respostas = (char*)malloc(5*sizeof(char));
	
	verifica_respostas(respostas);
	
	while( i< 9){
		if( input[i] == respostas[i] ){
			score+= 20;
		}
		i+= 2;
	}
	return score;
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
	
	fp_pdf = fopen("1QF1.pdf", "r+");
	fseek(fp_pdf, 0,SEEK_END);
	new_pos = ftell(fp_pdf);

	fclose(fp_pdf);
	return new_pos;
}

char* get_dados(char* dados, int file_size){
	FILE *fp_pdf;
	
	fp_pdf = fopen("1QF1.pdf", "r+");
	fread(dados,sizeof(char),file_size, fp_pdf);
	fclose(fp_pdf);
	return dados;
}


void tcp_envia_AQT(int QID){
	long int file_size;
	char s_QID[25];
	struct tm tm_registo;
	char aqt_time[18];
	char *dados;
	int n_digitos;
	char *s_size_of_data;
	
	static const char mon_name[][4] = {
    	"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
    	"JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
 	};
	
	limpa_buffer(s_QID, 25);
	
	tcp_write("AQT ", 4);
	
	sprintf(s_QID,"%d", QID);
	tcp_write(s_QID, strlen(s_QID));
	tcp_write(" ", 1);
	
	/*guarda tempo actual*/
	time( &(db_quest_form[QID].t_registo) );  
	/* copia conteudo para outra estrutura de modo a imprimir */
  	tm_registo = *localtime( &(db_quest_form[QID].t_registo) );
  	
  	sprintf(aqt_time, "%.2d%.3s%.4d_%.2d:%.2d:%.2d",
    	tm_registo.tm_mday, mon_name[tm_registo.tm_mon], 1900 + tm_registo.tm_year,
   		tm_registo.tm_hour, tm_registo.tm_min, tm_registo.tm_sec);
	
	tcp_write(aqt_time, 18);
	tcp_write(" ", 1);
	
	file_size = get_file_size();
	n_digitos = conta_digitos_long_int(file_size);
	s_size_of_data = (char*)malloc(n_digitos * sizeof(char));
	sprintf(s_size_of_data, "%ld", file_size);
	tcp_write( s_size_of_data, n_digitos);
	tcp_write(" ", 1);
	
	dados=(char*)malloc(file_size*sizeof(char));
	limpa_buffer(dados,file_size);
	dados = get_dados(dados, file_size);
	tcp_write(dados,file_size);
	free(dados);
	
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
	int QID;
	char s_QID[24];
	char respostas[10];
	int score;
	
	tcp_input_buffer = (char*)malloc(sizeof(char)* 10+ QID_SIZE + 12);
	limpa_buffer(tcp_input_buffer, 27);
	limpa_buffer(s_QID, 24);
	limpa_buffer(respostas, 10);
	
	tcp_read(tcp_input_buffer, 4);
	printf("buffer: %s", tcp_input_buffer);
	if( !strcmp("RQT ", tcp_input_buffer)){
		/* Le o SID e grava */
		printf("Li o RQT' '\n");
		tcp_read(s_stud_ID, 5);
		studID =atoi(s_stud_ID);
		/* come o '\n'*/
		tcp_read(tcp_input_buffer, 1);
		
		QID = cria_instancia_QID(studID);
		tcp_envia_AQT(QID);
	}
	else{
		printf("Pedido em TCP-RQT do user mal formatado\n");
		free(tcp_input_buffer);
		return;
	}
	
	/*Recebe RQS */
	limpa_buffer(tcp_input_buffer, 27);
	tcp_read(tcp_input_buffer, 4);
	printf("buffer: %s", tcp_input_buffer);
	if( !strcmp("RQS ", tcp_input_buffer)){
		printf("LI o RQS' '\n");
		
		/* Le o SID*/
		tcp_read(s_stud_ID, 5);
		studID =atoi(s_stud_ID);
		/* le o espaco*/
		tcp_read(tcp_input_buffer, 1);
		
		/* LE o QID*/
		tcp_read_alt(s_QID);
		s_QID[strlen(s_QID)-1] = '\0'; 	/* substitui ' ' do read_alt por '\0' */
		QID = atoi(s_QID);
		
		/*calcula_pontuacao();*/
		tcp_read(respostas,10);
		score = verifica_score(respostas); 
		
		printf("$$$$$$$$$$$$$$$$$$$$$ O score foi : %d\n\n", score);
		/* falta verificar o time*/
		
		/*responde ao User*/
		printf("Fazer o resto!!\n\n");
		
		/* informa o ECP*/
		printf("Fazer o resto!!\n\n");
		
		
	}else{
		printf("Pedido em TCP-RQS do user mal formatado\n");
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

void trata_arg_cmb(int n_args, char **argv){
	if(n_args == 1){
		if(!strcmp(argv[1],"-p")){
			TESport = atoi(argv[2]);
			strcpy(ECPname, "localhost");
			return;
		}
		if(!strcmp(argv[1],"-n")){
			strcpy(ECPname,argv[2]);
			return;
		}
		if(!strcmp(argv[1],"-e")){
			ECPport= atoi(argv[2]);
			strcpy(ECPname, "localhost");
			return;
		}
		else{
			printf("Numero de args invalido 1 arg\n");
			exit(-1);
		}
		return;
	}
	if(n_args == 2){
		if(!strcmp(argv[1],"-p") && !strcmp(argv[3],"-n")){
			TESport = atoi(argv[2]);
			strcpy(ECPname,argv[4]);
			return;
		}
		if(!strcmp(argv[1],"-p") && !strcmp(argv[3],"-e")){
			strcpy(ECPname, "localhost");			
			TESport = atoi(argv[2]);
			ECPport= atoi(argv[4]);
			return;
		}
		if(!strcmp(argv[1],"-n") && !strcmp(argv[3],"-e")){
			strcpy(ECPname,argv[2]);
			ECPport= atoi(argv[4]);
			return;
		}
		else{
			printf("Numero de args invalido 2 arg\n");
		}
		return;		
	}
	else{
		printf("Numero de args invalido todos args\n");
		exit(-1);		
	}
}

int main(int argc, char **argv){
	int ppid;	
	switch(argc){
		case 1 : strcpy(ECPname, "localhost");
			 break;
		case 3 : trata_arg_cmb(1,argv);
		         break;
		case 5 : trata_arg_cmb(2,argv);
			 break;
		case 7 : TESport = atoi(argv[2]);
			 strcpy(ECPname, argv[4]);
			 ECPport = atoi(argv[6]);
			 break;
		default:printf("Erro no número de argumentos");
	}
	printf("TESport: %d\t ECPname: %s\t ECPport: %d\n", TESport, ECPname, ECPport);
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
