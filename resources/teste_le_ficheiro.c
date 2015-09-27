#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>

#define PORT 58000

int fd, nleft,nwritten,nread;
int nbytestoread = 35, nbytestowrite= 20;

char *ptr,buffer[128];
char* msg ;

struct hostent *hostptr;
struct sockaddr_in serveraddr;

int tcp_connect(int fd){
	fd=socket(AF_INET,SOCK_STREAM,0);
	hostptr = gethostbyname("localhost");

	memset((void*)&serveraddr, (int)'\0',sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
	serveraddr.sin_port=htons((short int)PORT);

	connect(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
   printf("connect aceite\n");
   return fd;
}

void tcp_write(int nbytestowrite){
	ptr =strcpy(buffer,"client: Ola server\n");
   nleft=nbytestowrite;
   printf("vou enviar mensagem: %s\n", ptr);
	while(nleft > 0)	{
		if( (nwritten=write(fd,ptr,nleft)) == -1){
			printf("erro no write\n");
			exit(1);
		}
		nleft-=nwritten;
		ptr+=nwritten;
	}
	printf("Mensagem Enviada\n");
	return;
}

void tcp_read(int nbytestoread){
	nleft=nbytestoread;
	/* vou escrever por cima que nao faz mal neste exemplo*/
	ptr=&buffer[0];
	printf("Vou ler a resposta\n");
	while(nleft>0){
		if((nread = read(fd,ptr,nleft)) == -1){
			printf("Erro a ler a resposta\n");
			exit(1);
		}
		nleft-=nread;
		ptr+=nread;
	}
	printf("A resposta lida foi : %s\n", buffer);
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

long int get_file_size(){
	FILE *fp_pdf;
	long int new_pos;
	
	fp_pdf = fopen("aula2.pdf", "r+");
	fseek(fp_pdf, 0,SEEK_END);
	new_pos = ftell(fp_pdf);

	fclose(fp_pdf);
	return new_pos;
}

char* get_dados(char* dados, int file_size){
	FILE *fp_pdf;
	
	fp_pdf = fopen("aula2.pdf", "r+");
	fread(dados,file_size,file_size, fp_pdf);
	fclose(fp_pdf);
	return dados;
}

int main(){

	long int file_size;
	char *dados;
	
	file_size = get_file_size();

	dados= (char*)malloc(file_size*sizeof(char));
	limpa_buffer(dados,file_size);
	dados = get_dados(dados, file_size);
	printf("os dados:\n%s", dados);
	free(dados);

	
 	return 0;
}
