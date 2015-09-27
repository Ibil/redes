#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>

int CS_port = 58007;
int pid;



void udp_server(){
    int fd_udp;
    struct sockaddr_in serveraddr, clientaddr;
    int addrlen;
    struct hostent *h;

    char buffer[128];

    /*char* msg = "Hello to you too!\n";*/
    char resposta[631];
    char* p_resposta;

    FILE *fp_SS, *fp_fl;
    int n_SS, n_files;
    char n_files_str[3];
    char* SS_ip;
    char* SS_port;
    int i = 0;
    int j;
    char* temp_file_name;
    int server_choice = 1;


    fd_udp = socket(AF_INET,SOCK_DGRAM,0);

    memset((void*) &serveraddr, (int)'\0',sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_addr.s_addr= htonl (INADDR_ANY);
    serveraddr.sin_port=htons((u_short)CS_port);


    bind(fd_udp,(struct sockaddr*) &serveraddr, sizeof(serveraddr));


    addrlen = sizeof(clientaddr);

    SS_ip = (char*)malloc(sizeof(char)*20);
    SS_port = (char*)malloc(sizeof(char)* 6);
    temp_file_name = (char*)malloc(sizeof(char)*22);

    while(1){

        printf("A espera de pedido...\n");
        recvfrom(fd_udp,buffer,sizeof(buffer),0,(struct sockaddr*) &clientaddr,&addrlen);

        p_resposta = &resposta[0];

        if (!strcmp(buffer, "LST\n")){
            /*printf a deixar :: de onde veio o pedido*/
            h=gethostbyaddr((char*)&clientaddr.sin_addr,sizeof(struct in_addr),AF_INET);
            if(h==NULL){
                printf("LST - [%s:%hu]\n",inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
            }
            else{
                printf("LST - [%s:%hu]\n",h->h_name,ntohs(clientaddr.sin_port));
            }

            /*fetch SS*/
            fp_SS = fopen("SS_list.txt", "r+");
            fscanf(fp_SS,"%d",&n_SS);

            /*algoritmo que escolhe o i*/
            
                        for (j=0; j<= i; j++){
                            fscanf(fp_SS,"%s %s", SS_ip, SS_port);
                        }

		i=(i+1)% n_SS;
            /* fim de algo*/

            fclose(fp_SS);

            /*fetch FIles */
            fp_fl = fopen("available_file_list.txt", "r+");
            fscanf(fp_fl,"%d",&n_files);

            if(n_SS<10){
                fseek(fp_SS,2* sizeof(char),SEEK_CUR);
                }
             else{
                fseek(fp_SS,1*sizeof(char),SEEK_CUR);
             }


            sprintf(n_files_str,"%d", n_files);
            if (n_files == 0){
                strcpy(resposta, "EOF\n\0");
            }
            else{
                /*"AWL SS_ip SS_port n_files <Files>\n"*/
                strcpy(p_resposta, "AWL ");
                p_resposta += 4;

                strcpy(p_resposta, SS_ip);
                p_resposta += strlen(SS_ip) +1 ;/* o espaco*/
                *(p_resposta - 1) = ' ';

                strcpy(p_resposta, SS_port);
                p_resposta += strlen(SS_port) + 1;
                *(p_resposta - 1) = ' ';

                strcpy(p_resposta, n_files_str);
                p_resposta += strlen(n_files_str); /* nao tem o espaco por consistencia com o ciclo seguinte*/


                for (i = 0; i< n_files; i++){
                    *(p_resposta) = ' '; /* ordem diferente por necessidade de consistencia do ciclo*/
                    p_resposta++;
                    fscanf(fp_fl,"%s", temp_file_name);
                    strcpy(p_resposta, temp_file_name);
                    p_resposta += strlen(temp_file_name);
                }
                *(p_resposta) = '\n';
                *(p_resposta + 1) = '\0';
            }
            fclose(fp_fl);
        }
        else{
            strcpy(p_resposta, "ERR\n\0");
        }

        printf("A enviar mensagem de resposta\n");
        p_resposta = &resposta[0];
        printf("A resposta = %s", p_resposta);
        sendto(fd_udp, p_resposta, strlen(p_resposta) +1-1, 0, (struct sockaddr*)&clientaddr, addrlen);
        printf("Resposta enviada\n");
    }
    close(fd_udp);
}


int check_file_list(char* nome_de_ficheiro){
    FILE* fp_fl;
    int n_files, i;
    char* temp_file_name;

    temp_file_name = (char*)malloc(sizeof(char)*22);
	
	printf("O arg nome_do_ficheiro = %s\n", nome_de_ficheiro);
	
    fp_fl = fopen("available_file_list.txt", "r+");
    fscanf(fp_fl,"%d",&n_files);
    if (n_files <10){
    	fscanf(fp_fl,"#");
    }
    if (n_files == 0){
    	printf("Lista de ficheiros vazia\n");
        fclose(fp_fl);
        return 0;
    }
    else{
        for (i = 0; i< n_files; i++){
            fscanf(fp_fl,"%s", temp_file_name);
            printf("o temp_file_name = %s\n", temp_file_name);
            if(!strcmp(nome_de_ficheiro, temp_file_name)){
                fclose(fp_fl);
                return 1;
            }
        }
        fclose (fp_fl);
        return 0;
    }
}


void recplicate_to_ss(char* file_name, char* s_size_of_data, char* p_data){
    FILE* fp_SS;
    int n_SS;
    int i, n_CS_port;
    char *SS_ip, *SS_port;
    char *parte1, *p_parte1;

    struct hostent *hostptr;
    struct sockaddr_in serveraddr, clientaddr;

    int fd, newfd,nbytes,nleft,nwritten,nread;
    int clientlen;
    int addrlen;


    SS_ip = (char*)malloc(sizeof(char)*20);
    SS_port = (char*)malloc(sizeof(char)* 6);

    fp_SS = fopen("SS_list.txt", "r+");
    fscanf(fp_SS,"%d",&n_SS);

    for (i = 1; i<= n_SS;i++){
        fscanf(fp_SS,"%s %s", SS_ip, SS_port);
        n_CS_port = atoi(SS_port);

        /*inicio da abertura do tcp com o CS*/

        fd=socket(AF_INET,SOCK_STREAM,0);

        hostptr = gethostbyname(SS_ip);

        memset((void*) &serveraddr, (int)'\0',sizeof(serveraddr));
        serveraddr.sin_family=AF_INET;
        serveraddr.sin_addr.s_addr=((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
        serveraddr.sin_port=htons((u_short)n_CS_port);

        addrlen = sizeof(serveraddr);

        connect(fd,(struct sockaddr*) &serveraddr,sizeof(serveraddr));

        printf("connect aceite\n");

        /*tcp aberto modo client*/

        /* devo enviar "UPS file_name s_size_of_data p_data */
            /* primeiro meto"UPS file_name s_size_of_data "*/

        /* a ultima pos = '\0'*/
        parte1 = (char*)malloc( sizeof(char) * ( 4 + strlen(file_name) + 1 + strlen(s_size_of_data) + 1 +1));
        strcpy(parte1, "UPS ");
        p_parte1 = &parte1[4];

        strcpy(p_parte1, file_name);
        p_parte1 = p_parte1 + strlen(file_name);

        *p_parte1 = ' ';
        p_parte1++;

        strcpy(p_parte1, s_size_of_data);
        p_parte1 = p_parte1 + strlen(s_size_of_data);

        *p_parte1 = ' ';
        p_parte1++;
        *p_parte1 = '\0';

        p_parte1 = &parte1[0];

        /* envio parte1 */
        nbytes = strlen(parte1);
        nleft = nbytes;
        while(nleft > 0)	{
            if( (nwritten=write(fd,p_parte1,nleft)) == -1){
                printf("erro no write\n");
                exit(1);
            }
            nleft-=nwritten;
            p_parte1+=nwritten;
        }
        printf("enviei ao CS a primeira parte\n");
        p_parte1 = &parte1[0];

        /*envio os dados*/
        nbytes = strlen(p_data);
        nleft = nbytes;
        while(nleft > 0)	{
            if( (nwritten=write(fd,p_data,nleft)) == -1){
                printf("erro no write\n");
                exit(1);
            }
            nleft-=nwritten;
            p_data+=nwritten;
        }
        printf("enviei ao CS os dados\n");


        /*recebo resposta "AWS status\n\0"*/

        nbytes = 9;
        nleft = nbytes;

        while(nleft>0){
            if((nread = read(fd,p_parte1,nleft)) == -1){
                printf("Erro a ler a resposta\n");
                exit(1);
            }
            nleft-=nread;
            p_parte1+=nread;
        }
        p_parte1 = '\0';

        printf("resposta lida do CS : %s", parte1);

        free(parte1);



        /* fim do for para este SS*/
    }

    fclose(fp_SS);
}

void update_file_list(char* file_name){
    FILE* fp_fl;
    int n_files;

    fp_fl = fopen("available_file_list.txt", "r+");
    fscanf(fp_fl,"%d",&n_files);


    fseek(fp_fl, 0, SEEK_END);
    fprintf(fp_fl,"%s", file_name); /*pois ja vem com \n*/
    fseek(fp_fl, 0, SEEK_SET);

    if (n_files < 9){
        fprintf(fp_fl, "%d#", ++n_files);
    }
    else{
        fprintf(fp_fl, "%d", ++n_files);
    }
    fclose (fp_fl);
}

void tcp_server(){

    int fd_tcp, addrlen, newfd;

    struct sockaddr_in serveraddr, clientaddr;
    struct hostent *h;

    char buffer[128];
    char* p_buffer;

    char* data;

    int n, w, nbytes, nleft, nread, nwritten;

    char* file_name;

    /*char* msg = "Hello to you too!\n";*/
    char resposta[631];
    char* p_resposta;

    FILE *fp_SS, *fp_fl;
    int n_SS, n_files;
    char n_files_str[3];
    char* SS_ip;
    char* SS_port;
    int i, j , li_file_name;
    char* s_size_of_data;

    int ppid;

    /* vars do UPC*/
    int size_of_data, li_size;
    char *p_data;
    int erro_dados;

    file_name = (char*)malloc(sizeof(char)*22);

    memset((void*) &serveraddr, (int)'\0',sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
	serveraddr.sin_port=htons((u_short)CS_port);

	addrlen = sizeof(serveraddr);

	if ((fd_tcp = socket(AF_INET,SOCK_STREAM,0)) == -1){
		printf("Erro no socket : \n");
		exit(1);
	}
	if (bind(fd_tcp,(struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1){
		printf("Erro no bind : \n");
		exit(1);
	}
	printf("Estou a escuta!\n");
	if (listen (fd_tcp,5) == -1){
		printf("Erro no listen : \n");
		exit(1);
	}

	printf("Ouvi um pedido connect\n");
	while(1){
		if((newfd = accept(fd_tcp,(struct sockaddr*)&serveraddr,&addrlen)) == -1){
			printf("Erro no accept\n");
			exit(1);
		}
		printf("aceitei o pedido connect \n vou ler a mensagem \n");

		/*vejo se e UPR' ' ou UPC' '*/
    p_buffer=&buffer[0];
		nbytes = 4;
		nleft = nbytes;
		while(nleft>0){
			if((nread = read(newfd,p_buffer,nleft)) == -1){
				printf("Erro do read a ler pedido\n");
				exit(1);
			}
			nleft-=nread;
			p_buffer+=nread;
		}
		*p_buffer = '\0';
		p_buffer=&buffer[0];
		printf("O buffer tem : %s\n",p_buffer);
        // compara com UPR' '
        if (!strcmp(buffer, "UPR ")){
        	printf("entrei no upr\n");

            /*printf a deixar :: de onde veio o pedido*/
            h=gethostbyaddr((char*)&clientaddr.sin_addr,sizeof(struct in_addr),AF_INET);
            if(h==NULL){
                printf("UPR - [%s:%hu]\n", inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
            }
            else{
                printf("UPR - [%s:%hu]\n", h->h_name,ntohs(clientaddr.sin_port));
            }

            /* apanho o filename, e vejo se exise no ficheiro*/
            p_buffer=&buffer[0];
            nbytes = 22; /* file_name +\n + \0*/
            nleft = nbytes;
            li_file_name = 0;

            for( i = 0 ; nleft>0; ){
                if (li_file_name == 0){
                    /* se der problemas meter a ler 1 byte de cada vez*/
                    if((nread = read(newfd,p_buffer,nleft)) == -1){
                        printf("Erro do read a ler pedido\n");
                        exit(1);
                    }
                    for (j = 0 ; j < nread ; j++ ){
                        if (buffer[i + j] == '\n'){
                            li_file_name = 1;
                            buffer[i+j] = '\0'; /* /0 para ler o nome do ficheiro bem*/
                            if (i + j < 3){
                                li_file_name = -1; /* pois um file_name tem de ter pelo menos 3 caracteres*/
                            }
                            break;
                        }
                        else if (buffer[i + j] == ' '){
                            li_file_name = -1;
                            break;
                        }
                    }
                    i = i + j;
                    nleft-=nread;
                    p_buffer+=nread;
                }
                else if (li_file_name == 1){

                    /* guarda o file_name*/
                    strcpy(file_name, buffer);
                    printf("O file_name que vou receber chama-se: %s\n", file_name);

                    p_resposta = &resposta[0];
                    if (check_file_list(&buffer[0])){
                        strcpy(p_resposta, "AWR dup\n\0");
                    }
                    else{
                        strcpy(p_resposta, "AWR new\n\0");
                        printf("a resposta tem : %s\n", p_resposta);
                    }
                    break;
                }
                else{
                    p_resposta = &resposta[0];
                    strcpy(p_resposta, "ERR\n\0");
                    break;
                }
            }
            /* acabei de ler o pedido*/
        }
        // compara com AWR' '
        else if(!strcmp(buffer, "UPC ")){

            /*printf a deixar :: de onde veio o pedido*/
            h=gethostbyaddr((char*)&clientaddr.sin_addr,sizeof(struct in_addr),AF_INET);
            if(h==NULL){
                printf("UPC - [%s:%hu]\n", inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
            }
            else{
                printf("UPC - [%s:%hu]\n", h->h_name,ntohs(clientaddr.sin_port));
            }

            /* apanho o size do ficheiro a receber*/
            p_buffer=&buffer[0];
            nbytes = 8; /* 7 + o ' '  ::: 5.000.000*/
            nleft = nbytes;
            li_size = 0;

            for( i = 0 ; nleft>0; ){
                if (li_size == 0){
                    /* se der problemas meter a ler 1 byte de cada vez*/
                    if((nread = read(newfd,p_buffer,nleft)) == -1){
                        printf("Erro do read a ler pedido\n");
                        exit(1);
                    }
                    for (j = 0 ; j < nread ; j++ ){
                        if (buffer[i + j] == ' '){
                            li_size = 1;
                            buffer[i+j] = '\0'; /*para ler bem o numero depois*/
                            break;
                        }
                    }
                    i = i + j;
                    nleft-=nread;
                    p_buffer+=nread;
                }
                else{
                    break;
                }
            }

            /* vou alocar espaco para os dados*/
            p_buffer=&buffer[0];
            s_size_of_data = (char*)malloc(sizeof(char)*strlen(p_buffer));
            strcpy(s_size_of_data, p_buffer);
            size_of_data = atoi(p_buffer);
            p_data = (char*)malloc(sizeof(char)*size_of_data);

            /* vou receber os dados*/
            nbytes = size_of_data;
            nleft = nbytes;
            for( i = 0 ; nleft>0; ){
                /* se der problemas meter a ler 1 byte de cada vez*/
                if((nread = read(newfd,p_data,nleft)) == -1){
                    erro_dados = 1;
                    break;
                }
                i = i + nread;
                nleft-=nread;
                p_data+=nread;
            }

            /* vou escrever a resposta*/
            p_resposta = &resposta[0];
            if (erro_dados){
                strcpy(p_resposta, "AWC nok\n\0");
            }
            else{
                strcpy(p_resposta, "AWC ok\n\0");
            }

/* ################ vou fazer fork e replicar os dados para os SS, depois acutalizo o meu file_list*/
            /* file_name tem o nome do ficheiro
            p_data tem os dados*/

            ppid = fork();
            if (ppid < 0){
                printf("Erro a fazer o fork para SSs\n");
                exit(1);
                }
            else if (ppid == 0){
                //filho
                recplicate_to_ss(file_name, s_size_of_data, p_data);
                update_file_list(file_name);
                exit(0);
            }
            else{
                //pai
                /* continua o que tem de fazer*/
            }

/*################  */
        }
        else{
        		printf("entrei no else\n");
            p_resposta = &resposta[0];
            strcpy(p_resposta, "ERR\n\0");
        }


    /*############  envio de resposta tcp ##############*/
    	p_resposta = &resposta[0];
		printf("recebi a mensagem \n");
		printf("vou enviar a resposta %s\n", p_resposta);

		nbytes = strlen(p_resposta);
		nbytes = 3;
		printf("nbytes = %d\n", nbytes);
		nleft = nbytes;
		while(nleft > 0){
			printf("Entrei no while\n");
			
			
			
			if((nwritten=write(newfd,p_resposta,nleft)) == -1){
				printf("erro no write\n");
				exit(1);
			}
			printf("Passei o write\n");
            nleft-=nwritten;
            p_resposta+=nwritten;
		}
		printf("respota enviada\n");
	}
	close(newfd);
}

int main(int argc, char** argv){
    if (argc == 3){
        (!strcmp(argv[1],"-p")) ? CS_port= atoi(argv[2]) : exit(1);
    }

/*##########################    ZONA DE TESTES          ####################*/




/*##########################    FIM DE TESTES          ####################*/

    pid = fork();
    if (pid < 0){
        printf("Erro a fazer o fork inicial\n");
        return -1;
        }
    else if ( pid == 0){
        //filho
        tcp_server();
        exit(0);
    }
    else{
        //pai
        udp_server();
    }

    return 0;
}
