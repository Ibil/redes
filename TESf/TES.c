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

#define QUEST_TIME 300

int Tnn;
char *Tname;

int TESport = DEFAULT_TES_PORT;
int ECPport = DEFAULT_ECP_PORT;
char ECPname[50];

struct hostent *udp_hostptr;
struct sockaddr_in udp_serveraddr;
int udp_addrlen;

int fd, newfd, udp_fd;
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

int udp_open(int udp_fd){
	udp_fd = socket(AF_INET, SOCK_DGRAM,0);
	
	/* meter "localhost" para testes*/
	udp_hostptr=gethostbyname(ECPname);
	
	memset((void*)&udp_serveraddr, (int)'\0', sizeof(udp_serveraddr));
	udp_serveraddr.sin_family=AF_INET;
	udp_serveraddr.sin_addr.s_addr=((struct in_addr*)(udp_hostptr->h_addr_list[0]))->s_addr;
	udp_serveraddr.sin_port=htons((int)ECPport);
	return udp_fd;
}

void udp_receive(char* buffer, int nbytestoread){
	printf("Pronto a receber\n");
	recvfrom(udp_fd, buffer, nbytestoread*sizeof(char),0, (struct sockaddr*) &udp_serveraddr, &udp_addrlen);
	printf("Recebi resposta: %s", buffer);
	return;
}

void udp_send(char* mensagem, int nbytestosend){
	printf("VOu enviar a mensagem: %s", mensagem);
	sendto(udp_fd,mensagem, nbytestosend*sizeof(char), 0, (struct sockaddr*)&udp_serveraddr, udp_addrlen);
	printf("Mensagem enviada\n");
	return;
}

void udp_close(int udp_fd){
	close(udp_fd);
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
	char filename[20];
	
	limpa_buffer(filename, 20);
	
	sprintf(filename, "%dQF1A.txt", Tnn);
	fp_txt = fopen(filename, "r");
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
	free(respostas);
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
	char filename[20];
	
	limpa_buffer(filename, 20);
	
	sprintf(filename, "%dQF1.pdf", Tnn);
	fp_pdf = fopen(filename, "r+");
	fseek(fp_pdf, 0,SEEK_END);
	new_pos = ftell(fp_pdf);

	fclose(fp_pdf);
	return new_pos;
}

char* get_dados(char* dados, int file_size){
	FILE *fp_pdf;
	char filename[20];
	
	limpa_buffer(filename, 20);
	
	sprintf(filename, "%dQF1.pdf", Tnn);
	fp_pdf = fopen(filename, "r+");
	fread(dados,sizeof(char),file_size, fp_pdf);
	fclose(fp_pdf);
	return dados;
}

void informa_ECP(char* SID, char* QID, char* score){
	char *lixo;
	
	lixo =(char*)malloc( (strlen(QID) + 5) * sizeof(char));

	udp_fd = udp_open(fd);
	udp_addrlen = sizeof(udp_serveraddr);
	
	udp_send("IQR ", 4);
	udp_send(SID,5);
	udp_send(" ",1);
	udp_send(QID,strlen(QID));
	udp_send(" ",1);
	udp_send(Tname,strlen(Tname));
	udp_send(" ",1);
	udp_send(score,strlen(score));
	udp_send("\n",1);
	
	udp_receive(lixo, strlen(lixo));
	
	udp_close(udp_fd);
	return;
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
	free(s_size_of_data);
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
	
	time_t t_entrega;
	
	char s_score[3];
	
	tcp_input_buffer = (char*)malloc(sizeof(char)* 10+ QID_SIZE + 12);
	limpa_buffer(tcp_input_buffer, 27);
	limpa_buffer(s_QID, 24);
	limpa_buffer(respostas, 10);
	limpa_buffer(s_score,3);
	
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
		
		/* Verifica o time*/
		time(&t_entrega);
		if( QUEST_TIME > difftime(t_entrega, db_quest_form[QID].t_registo) ){
			/*calcula_pontuacao();*/
			tcp_read(respostas,10);
			score = verifica_score(respostas); 
			printf("$$$$$$$$$$$$$$$$$$$$$ O score foi : %d\n\n", score);
		}
		else{
			score = 101; /* isto = -1;*/
		}
		
		/*responde ao User: AQS SID SCORE\n */
		tcp_write("AQS ", 4);
		tcp_write(s_stud_ID,5);
		tcp_write(" ", 1);
		if( score != 101 ){
			sprintf(s_score, "%d", score);
		}
		else{
			strcpy(s_score, "-1");
			
		}
		tcp_write(s_score, strlen(s_score));
		tcp_write("\n", 1);
		
		/* informa o ECP*/
		informa_ECP(&s_stud_ID[0], &s_QID[0], &s_score[0]);
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
		if(!strcmp(argv[3],"-p")){
			TESport = atoi(argv[4]);
			strcpy(ECPname, "localhost");
			return;
		}
		if(!strcmp(argv[3],"-n")){
			strcpy(ECPname,argv[4]);
			return;
		}
		if(!strcmp(argv[3],"-e")){
			ECPport= atoi(argv[4]);
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
		if(!strcmp(argv[3],"-p") && !strcmp(argv[5],"-n")){
			TESport = atoi(argv[4]);
			strcpy(ECPname,argv[6]);
			return;
		}
		if(!strcmp(argv[3],"-p") && !strcmp(argv[5],"-e")){
			strcpy(ECPname, "localhost");			
			TESport = atoi(argv[4]);
			ECPport= atoi(argv[6]);
			return;
		}
		if(!strcmp(argv[3],"-n") && !strcmp(argv[5],"-e")){
			strcpy(ECPname,argv[4]);
			ECPport= atoi(argv[6]);
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
	if(argc < 3 || 
		!strcmp(argv[1], "-p") || !strcmp(argv[1], "-n") || !strcmp(argv[1], "-e") ){
		printf("Introduza o Topic_Number e Topic_name.\nConsulte os ficheiros TES_parametros.png e readme.txt\n");
		exit(1);
	}
	Tnn = atoi(argv[1]);
	Tname = (char*)malloc(strlen(argv[2]) * sizeof(char));
	strcpy(Tname,argv[2]);
	switch(argc){
		case 3 : strcpy(ECPname, "localhost");
			 break;
		case 5 : trata_arg_cmb(1,argv);
		         break;
		case 7 : trata_arg_cmb(2,argv);
			 break;
		case 9 : TESport = atoi(argv[4]);
			 strcpy(ECPname, argv[6]);
			 ECPport = atoi(argv[8]);
			 break;
		default:printf("Erro no número de argumentos");
	}
	printf("Tnn: %d\t Tname: %s\t TESport: %d\t ECPname: %s\t ECPport: %d\n", Tnn, Tname, TESport, ECPname, ECPport);
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
