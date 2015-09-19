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

#define DEFAULT_PORT 58017

char ECSname[50];
int ECSport;
int SID;
char terminal_input[50];


void trata_args(char* arg1, char* arg2){
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
	while(1){
		printf("Escolha uma das opções possiveis:\n\t list\n\t request\n\t submit\n\t exit\n");
		scanf("%s", terminal_input);
		if(!strcmp(terminal_input, "list")){
			/*udp_list();*/
		}

		if(!strcmp(terminal_input, "exit")){
			break;
		}
	}
	return 0;
}
