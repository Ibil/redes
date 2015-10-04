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

#define QID_SIZE 24

#define QUEST_TIME 300

int Tnn;
char *Tname;
int Tname_size;

int TESport = DEFAULT_TES_PORT;
char ECPname[50];
int ECPport = DEFAULT_ECP_PORT;

struct hostent *udp_hostptr;
struct sockaddr_in udp_serveraddr;
int udp_addrlen;

int tcp_socket_fd, tcp_connection_fd, udp_fd;
int nread, nwritten,nleft;
struct sockaddr_in serveraddr, clientaddr;
int addrlen, clientlen;

extern int errno;


/* ################   FUncoes UDP ################*/

int udp_open(int udp_fd){
	udp_fd = socket(AF_INET, SOCK_DGRAM,0);
	
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

void tcp_set_socket(){
	if ((tcp_socket_fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
		printf("Erro no socket : \n");
		exit(1);
	}
	memset((void*) &serveraddr, (int)'\0',sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
	serveraddr.sin_port=htons((short int)TESport);
	if (bind(tcp_socket_fd,(struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1){
		printf("Erro no bind : \n");
		exit(1);
	}
}

void tcp_accept_connection(){
	if (listen (tcp_socket_fd,5) == -1){
		printf("Erro no listen : \n");
		exit(1);
	}
	printf("Estou a escuta!\n");
	clientlen = sizeof(clientaddr);
	if((tcp_connection_fd = accept(tcp_socket_fd,(struct sockaddr*)&clientaddr,&clientlen)) == -1){
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
		if((nread = read(tcp_connection_fd,ptr,nleft)) == -1){
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
		if((nread = read(tcp_connection_fd,ptr_temp,1)) == -1){
			printf("Erro a ler a mensagem\n");
			exit(1);
		}
		printf("O char : %c\n", (*ptr_temp));
		ptr_temp+=nread;
	}
	while( (*(ptr_temp-1))!=' ' );
	*(ptr_temp-1)=0;
	
	puts("sair do read alt");
	printf("A resposta lida foi: %s\n", msg);
	return;
}

void tcp_write(char* buffer, int nbytestowrite){
	char *ptr;
	printf("vou enviar a resposta: %s\n", buffer);
	ptr = buffer;
	nleft = nbytestowrite;
	while(nleft > 0){
		if( (nwritten=write(tcp_connection_fd,ptr,nleft)) == -1){
			printf("erro no write\n");
			exit(1);
		}
	nleft-=nwritten;
	ptr+=nwritten;
	}
	printf("Resposta enviada\n");
	return;
}



void tcp_close_connection(){	
	close(tcp_connection_fd);
	return;
}

void tcp_close_socket(){	
	close(tcp_socket_fd);
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
	int tamanho = 4 + strlen(SID) + 1 + QID_SIZE + 1 + Tname_size + 1 + strlen(score) + 1;
	char resposta[tamanho];
	
	limpa_buffer(resposta,tamanho);
	
	printf("o TName tem size: %d ; %d e tem la : %s\n",strlen(Tname), Tname_size, Tname);
	
	printf("%s %s %s %s\n", SID, QID, Tname, score);
	
	lixo =(char*)malloc( (strlen(QID) + 5) * sizeof(char));

	udp_fd = udp_open(udp_fd);
	udp_addrlen = sizeof(udp_serveraddr);
	
	sprintf(resposta, "IQR %s %s %s %s\n", SID, QID, Tname, score);
	printf("A comunicacao com o ECP: %s\n", resposta);
	udp_send(resposta, tamanho);
	udp_receive(lixo, strlen(lixo));
	
	udp_close(udp_fd);
	return;
}


void tcp_envia_AQT(char* s_SID, char* s_QID){
	long int file_size;
	time_t t_registo;
	struct tm tm_registo;
	char aqt_time[18];
	char *dados;
	int n_digitos;
	char *s_size_of_data;
	
	static const char mon_name[][4] = {
    	"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
    	"JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
 	};
	
	
	tcp_write("AQT ", 4);
	
	/*guarda tempo actual*/
	time(&t_registo);  
	/* copia conteudo para outra estrutura de modo a imprimir */
  	tm_registo = *localtime(&t_registo);
  	/*guarda o tempo de registo*/
  	sprintf(aqt_time, "%.2d%.3s%.4d_%.2d:%.2d:%.2d",
    	tm_registo.tm_mday, mon_name[tm_registo.tm_mon], 1900 + tm_registo.tm_year,
   		tm_registo.tm_hour, tm_registo.tm_min, tm_registo.tm_sec);
   		
	/* COncatena o SID com o tempo de registo para obeter o QID e envia-o*/
	sprintf(s_QID,"%s_%s", s_SID, aqt_time);
	tcp_write(s_QID, QID_SIZE);
	tcp_write(" ", 1);
	
	tcp_write(aqt_time, 18);
	tcp_write(" ", 1);
	
	file_size = get_file_size();
	printf("ola\n");
	n_digitos = conta_digitos_long_int(file_size);
	printf("ola\n");
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
	char* tcp_input_buffer;	
	char s_stud_ID[6];
	time_t t_registo, t_entrega;
	struct tm tm_registo;
	char s_QID[25];
	char respostas[10];
	int score, i;
	char s_score[3];
	
	static const char mon_name[][4] = {
    	"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
    	"JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
 	};
	
	char DD[3]; char MMM[4]; char YYYY[5];
	char HH[3]; char MM[3];  char SS[3];
	

	tcp_input_buffer = (char*)malloc(sizeof(char)* 10+ QID_SIZE + 12);
	
	limpa_buffer(tcp_input_buffer, 46);
	limpa_buffer(s_QID, QID_SIZE + 1);
	limpa_buffer(respostas, 10);
	limpa_buffer(s_score,3);
	limpa_buffer(s_stud_ID,6);
	limpa_buffer(DD,3);
	limpa_buffer(MMM,4);
	limpa_buffer(YYYY,5);
	limpa_buffer(HH, 3);
	limpa_buffer(MM, 3);
	limpa_buffer(SS,3);
	
	tcp_read(tcp_input_buffer, 4);
	printf("buffer: %s\n", tcp_input_buffer);
	if( !strcmp("RQT ", tcp_input_buffer)){
		printf("Li o RQT' '\n");
		/* 	
		-(INPUT)->  RQT SID\n  
		<-(OUTPUT)- AQT QID time size data\n 
					time: DDMMMYYYY_HH:MM:SS
						  09JAN2015_20:00:00
		*/
		/* Le o SID e grava */
		tcp_read(s_stud_ID, 5);
		/* come o '\n'*/
		tcp_read(tcp_input_buffer, 1);
		tcp_envia_AQT(&s_stud_ID[0], &s_QID[0]);
	}
	else if( !strcmp("RQS ", tcp_input_buffer)){
		/*
		-(INPUT)->  RQS SID V1 V2 V3 V4 V5\n
		<-(OUTPUT)- AQS QID score\n	
		*/
		printf("LI o RQS' '\n");
		
		tcp_read(s_stud_ID, 5); /* Le o SID*/
		tcp_read(tcp_input_buffer, 1);
		
		
		tcp_read_alt(s_QID);	/* LE o QID*/
		/*s_QID[strlen(s_QID)-1] = '\0';*/ 	/* substitui ' ' do read_alt por '\0' */
		
		/* obtem t_registo a partir do QID */
			/*guarda tempo actual*/
		time(&t_registo);  
			/* copia conteudo para outra estrutura de modo a imprimir */
	  	tm_registo = *localtime(&t_registo);
	  		/*guarda o tempo de registo*/
	  		
  			
  		DD[0] = s_QID[6]; DD[1] = s_QID[7];
  		tm_registo.tm_mday = atoi(DD);
  		
  		
  		
  		/* Copia o nome do mes ; vai a procura do seu indice para atribuir ao tm_registo*/
  		MMM[0] = s_QID[8]; MMM[1] = s_QID[9]; MMM[2] = s_QID[10];
  		
  		for( i = 0; strcmp(MMM, mon_name[i])&&(i< 12) ; i++);
  		tm_registo.tm_mon = i;
  		printf("O mes: %d %s\n", tm_registo.tm_mon, mon_name[tm_registo.tm_mon]);
  		
  		
  		YYYY[0] = s_QID[11]; YYYY[1] = s_QID[12];YYYY[2] = s_QID[13];YYYY[3] = s_QID[14];
  		tm_registo.tm_year = atoi(YYYY) - 1900;
  		printf("o ano: %d\n", tm_registo.tm_year);
  		
  		
  		HH[0] = s_QID[16]; HH[1] = s_QID[17];
  		tm_registo.tm_hour = atoi(HH);
  		printf("A hora: %d\n", tm_registo.tm_hour);
  		
  		MM[0] = s_QID[19]; MM[1] = s_QID[20];
  		tm_registo.tm_min = atoi(MM);
  		
  		
  		printf(" ## antes dos ss DEBUG ##%.2d%.3s%.4d_%.2d:%.2d:%.2d\n",
    	tm_registo.tm_mday, mon_name[tm_registo.tm_mon], 1900 + tm_registo.tm_year,
   		tm_registo.tm_hour, tm_registo.tm_min, tm_registo.tm_sec);
  		
  		SS[0] = s_QID[22]; SS[1] = s_QID[23];
  		printf("O atoi dos SS : %d  ja que S_qid= %c %c\n", atoi(SS), s_QID[22],s_QID[23]);
  		tm_registo.tm_sec = atoi(SS);		
	
		printf(" ## DEBUG ##%.2d%.3s%.4d_%.2d:%.2d:%.2d\n",
    	tm_registo.tm_mday, mon_name[tm_registo.tm_mon], 1900 + tm_registo.tm_year,
   		tm_registo.tm_hour, tm_registo.tm_min, tm_registo.tm_sec);
		/* Verifica o time*/
		time(&t_entrega);
		
		printf(" ## DEBUG ##%.2d%.3s%.4d_%.2d:%.2d:%.2d\n",
    	tm_registo.tm_mday, mon_name[tm_registo.tm_mon], 1900 + tm_registo.tm_year,
   		tm_registo.tm_hour, tm_registo.tm_min, tm_registo.tm_sec);
		
		if( QUEST_TIME > difftime(t_entrega, mktime(&tm_registo))){
			/* calcula pontuacao */
			tcp_read(respostas,10);
			score = verifica_score(respostas); 
			printf("$$$$$$$$$$$$$$$$$$$$$ O score foi : %d\n\n", score);
		}
		else{
			score = 101; /* isto == -1;*/
		}
		
		/*responde ao User: AQS QID SCORE\n */
		tcp_write("AQS ", 4);
		tcp_write(s_QID,QID_SIZE);
		tcp_write(" ", 1);
		if( score != 101 ){
			sprintf(s_score, "%d", score);
			tcp_write(s_score, strlen(s_score));
			tcp_write("\n", 1);
			informa_ECP(&s_stud_ID[0], &s_QID[0], &s_score[0]);/*informa o ECP*/
		}
		else{
			strcpy(s_score, "-1");
			tcp_write(s_score, strlen(s_score));
			tcp_write("\n", 1);
		}
	}else{
		printf("Pedido em TCP-RQS do user mal formatado\n");
	}
	free(tcp_input_buffer);
	return;
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
	if(argc < 3 || 
		!strcmp(argv[1], "-p") || !strcmp(argv[1], "-n") || !strcmp(argv[1], "-e") ){
		printf("Introduza o Topic_Number e Topic_name.\nConsulte os ficheiros TES_parametros.png e readme.txt\n");
		exit(1);
	}
	Tnn = atoi(argv[1]);
	Tname_size = strlen(argv[2]);
	printf("O tname size: %d\n", Tname_size);
	Tname = (char*)malloc((Tname_size + 1) * sizeof(char));
	limpa_buffer(Tname, Tname_size + 1);
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
		
	tcp_set_socket();
	while(1){
		tcp_accept_connection();
		if (fork() == 0) {
			/* o filho vai fechar o socket para so tratar o pedido recebido*/
			tcp_close_socket();
			tcp_trata_mensagem();
			tcp_close_connection();
			exit(0);
		}
		else {
			/* O pai vai aceitar uma ligacao nova*/
			tcp_close_connection();
			continue;
		}
	}
	tcp_close_socket();
	
	return 0;
}
