#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>

int fd, addrlen, newfd;                   
struct sockaddr_in addr;

struct hostent *h;

int n, nw;           
char *ptr, buffer[128],*p_buffer, buffer2[128], *p_buffer2; 
char *token;
int size;
int flag_file = 0;
int flag_file_w = 0;

char file_name[30];

FILE *infile;
FILE *outfile;

char *msgFile;
char *sSize;
char *msg, *msgTemp;
int nbytes, nleft, nread, nwritten;

char *tokenFn;
char *tokenSize;
char *tokenData;
int sizeData = 0;

int main(int argc, char** argv){
	int PORT = 59000;
    if (argc == 3){
        (!strcmp(argv[1],"-p")) ? PORT= atoi(argv[2]) : exit(1);
    }

	memset((void*) &addr, (int)'\0',sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons((u_short)PORT);

	addrlen = sizeof(addr);

	if ((fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
		printf("Erro no socket : \n");
		exit(1);
	}
	if (bind(fd,(struct sockaddr*)&addr, sizeof(addr)) == -1){
		printf("Erro no bind : \n");
		exit(1);
	}
	printf("Estou a escuta!\n");
	if (listen (fd,5) == -1){
		printf("Erro no listen : \n");
		exit(1);
	}

	int a = 1;

		
		
	
	while(a > 0){
		if((newfd = accept(fd,(struct sockaddr*)&addr,&addrlen)) == -1){
			printf("Erro no accept\n");
			exit(1);
		}
		printf("aceitei o pedido connect\nvou ler a mensagem\n");

		ptr=&buffer[0];
		nbytes = 60;
		nleft = nbytes;
		while(nleft>0){
			if((nread = read(newfd,ptr,1)) == -1){
				printf("Erro a ler a resposta\n");
				exit(1);
			}
			if(*(ptr) == '\n'){
				*(ptr) = '\0';
				break;
			}
			nleft-=nread;
			ptr+=nread;
		}
		printf("recebi do user : %s\n", buffer);

		
		p_buffer = &buffer[0];
		p_buffer2 = &buffer2[0];
		strcpy(p_buffer2, p_buffer);
		
		
		token = strtok(buffer, " ");
		printf("o buffer:%s\n", buffer);
		printf("o buffer : %s\n", p_buffer2);
		

		if( strcmp(token, "REQ") == 0){
		
			
			token = strtok(NULL, " ");

			
/* ##############################################*/
			
			printf("cheguei1: %s\n", buffer2);
			int i = 0;
			for(i = 0; buffer2[i]!= ' '; i++){
				
			}
			
			i++;
			p_buffer2 = &buffer2[i];
			printf("cheguei2, o meu resto do buffer com o nome tem: %s\n", p_buffer2);
			
			int j = 0;
			for(j = 0; *(p_buffer2 + j) != '\0'; j++, i++){
				file_name[j] = *(p_buffer2 + j);
			}
			file_name[j] = '\0';
			
			printf("cheguei3 : %s\n", file_name);
			
			//ve se foi feito fclose(infile) anteriormente
			if(flag_file == 0){
			printf("vou abrir a primeira\n");
				if ( (infile = fopen(file_name, "r+")) != NULL){
					flag_file = 1;
				}
			}else{
				printf("vou abrir a segunda\n");
				 if (fclose(infile) != 0){
				 		printf("ERRO A FAZER FCLOSE");
			 		}
			 		printf("passei o fclose\n");
				if ((infile = fopen(file_name, "r+")) == NULL){
					printf("erro a fazer fopen\n");
					flag_file = 0; 
				}
				//infile = freopen(file_name, "r+", infile);
			}
			
		
			
/* ##############################################*/
			
			printf("Foi solicitado o ficheiro %s pela maquina", file_name );
			
			h=gethostbyaddr((char*)&addr.sin_addr,sizeof(struct in_addr),AF_INET);
			if(h==NULL)
				printf("[%s:%hu]\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
			else printf("[%s:%hu]\n",h->h_name, ntohs(addr.sin_port));
			
			
/////////////////////////////////////////////////			
			
			printf("passei o fopen\n");
			//infile == NULL;
			printf("passei o NULL\n");
			if (infile != NULL){
				printf("coisa2\n");
				msgTemp = "REP ok ";
				//char c;
				//char* car;
				
				fseek(infile, 0L, SEEK_END);
				size = ftell(infile);
				fseek(infile, 0L, SEEK_SET);
				
				
				
				msgFile = (char*)malloc(size * sizeof(char)); 
				
				printf("msgfile\n");
				
				sSize = (char*)malloc(2 * sizeof(char));
				
				
				
				fread(msgFile, sizeof(char), size, infile);
				
				//fclose(infile);
				printf("msgFile :%s\n", msgFile);


				sprintf(sSize, "%d", size);
				msg = (char*)malloc(sizeof(msgTemp) + sizeof(sSize) + (sizeof(msgFile) + sizeof(char)*3));
				strcpy(msg, msgTemp);
				strcat(msg, sSize);
				strcat(msg, " ");
				strcat(msg, msgFile);

				strcat(msg, "\n\0");
				free(sSize);
				free(msgFile);
				*sSize == 2;

			} else {
				printf("entrei no else\n");
				msg = "REP nok\n\0"; 
				printf("msg\n");
				printf("msg:%s\n", msg);
				printf("passei o fclose\n");
			}
		}
		else if(strcmp(token, "UPS") == 0){  //requeste
			
			//p_buffer2
			
			strtok_r(p_buffer2 , " ", &tokenFn);  //tokenFn = file_name + resto
			
			strtok_r(tokenFn, " ", &tokenSize);  //tokenFn = file_name; tokensize = size +data
			strtok_r(tokenSize, " ", &tokenData); //tokensize = size; tokenData = data

			sizeData = atoi(tokenSize);
			//*********************novo************
			
				//ve se foi feito fclose(outfile) anteriormente
			if(flag_file_w == 0){
			printf("vou abrir a primeira\n");
				if ( (outfile = fopen(tokenFn, "w")) != NULL){   //troca file_name por tokenFn
					flag_file_w = 1;
				}
			}else{
				printf("vou abrir a segunda\n");
				 if (fclose(outfile) != 0){
				 		printf("ERRO A FAZER FCLOSE");
			 		}
			 		printf("passei o fclose\n");
				if ((outfile = fopen(tokenFn, "w")) == NULL){   //troca file_name por tokenFn
					printf("erro a fazer fopen\n");
					flag_file_w = 0; 
				}
				//infile = freopen(file_name, "r+", infile);
			}
			
			//*************************novo************
			
			
			//outfile = fopen(token, "w");
			
			
			
			if (outfile != NULL) { 
				fwrite(tokenData, 1, sizeData, outfile);
				msg = "AWS ok\n\0";
				printf("O ficheiro %s proveniente do servidor central foi guardado com sucesso.\n", tokenFn);
			} else {
				msg = "AWS nok\n\0";
				printf("Não foi possivel guardar o ficheiro %s proveniente do servidor central.\n", tokenFn);
			}
			//fclose(outfile);
			
			
		}
		else{
			msg = "ERR\n\0";
		}
		//printf("antes do free\n");
		//free(token);
		//printf("depois do free\n");
		nbytes = strlen(msg);
		printf("vou enviar a resposta\n");
		ptr =strcpy(buffer,msg);
		nleft = nbytes;
		while(nleft > 0){
			if( (nwritten=write(newfd,ptr,nleft)) == -1){
				printf("erro no write\n");
				exit(1);
			}
			nleft-=nwritten;
			ptr+=nwritten;
		}
		printf("respsta enviada\n");
		printf("escrevi a resposta: %s\n", msg);
		close(newfd);
		//free(msg);
	}

	
	close(fd);
	return 0 ;
}


