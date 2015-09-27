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

int fd,fdCS,fdSS;
struct hostent *hostptr;
struct sockaddr_in serveraddr, clientaddr;
int addrlen;
int nleft,nbytes,nwritten,nread;
struct stat st;

char* p_dados;
char* parameter;
char *token;
char msg[36];
char *msgtcp;
char* msg1;
char resposta[9];
char* p_resposta;
char* file_buffer;
FILE *fs;

int udp(char* name,int port){

 	fd=socket(AF_INET,SOCK_DGRAM,0);

 	hostptr = gethostbyname(name);

 	memset((void*) &serveraddr, (int)'\0',sizeof(serveraddr));
 	serveraddr.sin_family=AF_INET;
 	serveraddr.sin_addr.s_addr=((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
 	serveraddr.sin_port=htons((u_short)port);
 	addrlen=sizeof(serveraddr);

    return 0 ;
}

int tcp(char* name, int port){

    fdCS=socket(AF_INET,SOCK_STREAM,0);

	hostptr = gethostbyname(name);

	memset((void*) &serveraddr, (int)'\0',sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
	serveraddr.sin_port=htons((u_short)port);

	addrlen = sizeof(serveraddr);

	connect(fdCS,(struct sockaddr*) &serveraddr,sizeof(serveraddr));

    return 0;

}


char buffer[655];

int main(int argc, char **argv){

	char cs_name[100];
	int cs_port;

	switch(argc){

	case 1 : strcpy(cs_name,"localhost");cs_port = 58007;
	         break;

	case 3 : !strcmp(argv[1],"-n") ? strcpy(cs_name,argv[2]) : exit(-1);
	         cs_port = 58007;
	         printf("%s",argv[1]);
	         break;


    case 5 : !strcmp(argv[1],"-n") ? strcpy(cs_name,argv[2]) : exit(-1);
             !strcmp(argv[3],"-p") ? cs_port=(long int)argv[4] : exit(-1);
             break;

    default:printf("Erro no número de argumentos");

	}

		
       /**DEBUG**/
      // printf("Host:%s,Port:%d\n",cs_name,cs_port);

    char command [2];

    char ss_name[15];
    int ss_port;
    char* IPSS;
    char* output[10];
    int n_files=0;
    int portSS,file_list;
    int i;

		parameter = (char*) malloc(sizeof(char) *20);
	
		for(i = 0; i<20;i++){
			*(parameter + i)='\0';
		}

    printf("Escolha uma das opções possiveis:\n\t list\n\t retrieve\n\t upload\n\t exit\n");
		IPSS = (char*) malloc(sizeof(char) * 17);
		i = 0;
		for(i = 0; i<17;i++){
			*(IPSS + i)='\0';
		}


 	while(1){
			
 		   scanf("%s",command);

 		  if(!strcmp(command,"list")){

 		                udp(cs_name,cs_port);
 		                

                        strcpy(msg,"LST\n");

 		                sendto(fd,msg,strlen(msg) +1,0,(struct sockaddr*) &serveraddr , addrlen);
                        addrlen=sizeof(serveraddr);


                        recvfrom(fd,buffer,sizeof(buffer),0,(struct sockaddr*) &serveraddr, &addrlen);

                        puts(buffer);

                        token = strtok(buffer," ");


                        if(!strcmp(token,"ERR") || !strcmp(token,"EOF")){
                           printf("%s",token);
                           break;

                        }

                        int i=1;
                        int file_index=1;

                        printf("File in the storage server: \n");


                        while(token != NULL){

                            token = strtok(NULL," ");

                            switch(i){
				
                                case 1 : strcpy(IPSS,token);break;
                                case 2 : portSS = atoi(token);break;
                                case 3 : n_files = atoi(token);break;
                                default :if(token!=NULL){
                                                printf("%d - %s \n", file_index,token);
                                                file_index++;
                                    }
                            }
                            i++;
                         }

          }else if(!strcmp(command,"retrive")){

           tcp(IPSS,portSS);
           scanf("%s",parameter);


           			   msg1 =&msg[0];
                       strcpy(msg1,"REQ ");
                       msg1 = msg1 +4*sizeof(char);
                       strcpy(msg1,parameter);
                       msg1 = msg1 + strlen(parameter);
                       *msg1 = '\n';
                       msg1 =&msg[0];



                       nbytes=strlen(msg1);
                       nleft=nbytes;
                    	 	while(nleft > 0)	{
                     		if( (nwritten=write(fdCS,msg1,nleft)) == -1){
                     			printf("erro no write\n");
                     			exit(1);
                     		}
                           if(nwritten==0)break;
                     		nleft-=nwritten;
                     		msg1+=nwritten;
                     	}
                     	
                     	puts(msg1);


                     	p_resposta =&resposta[0];
           			    msg1 =&msg[0];


						nleft = 10;
						nread = 1;
						
                       while(nleft>0){
                       	nread = 0;
                       	printf("entro no while\n");
                         if((nread = read(fdCS,p_resposta,nleft)) == -1){
           		       			printf("Erro a ler a resposta \n");
           		       			exit(1);
                    			}
                        	nleft-=nread;
                         p_resposta+=nread;
                      	}
           						p_resposta =&resposta[0];

          }else if(!strcmp(command,"upload")){
          
            scanf("%s",parameter);
            
		tcp(cs_name,cs_port);
 
			msg1 =&msg[0];
            strcpy(msg1,"UPR ");
            msg1 = msg1 +4*sizeof(char);
            strcpy(msg1,parameter);
            msg1 = msg1 + strlen(parameter);
            *msg1 = '\n';
            *(msg1 + sizeof(char)) = '\0';
            msg1 =&msg[0];

			

            nbytes=strlen(msg1);
            nleft=nbytes;




         	 	while(nleft > 0)	{
          		if( (nwritten=write(fdCS,msg1,nleft)) == -1){
          			printf("erro no write\n");
          			exit(1);
          		}
          		nleft-=nwritten;
          		msg1+=nwritten;
          	}
            	

        p_resposta =&resposta[0];      
		msg1 =&msg[0];
		

		nleft=8;
		nread = 1;
		
		
		

            while(nleft>0){
            	nread = 0;
              if((nread = read(fdCS,p_resposta,nleft)) == -1){
		       			printf("Erro a ler a resposta \n");
		       			exit(1);
         			}
             	nleft-=nread;
                p_resposta+=nread;
           	}
		p_resposta =&resposta[0];
		

		

                  if ( (fs = fopen(parameter,"r+")) ==NULL){
					  printf("Erro\n");
					  } 

                   fseek(fs, 0, SEEK_END);

                   long int size = ftell(fs);

                   fseek(fs, 0, SEEK_SET);

                   file_buffer = malloc (size*sizeof(char));
                   
        
                    

		 if (file_buffer)
                     {
                       fread(file_buffer,1,size,fs);
                     }
			

			*(file_buffer+size)='\0';
			
			
                     fclose(fs);

                     p_dados =(char*)malloc(size*sizeof(char));

                    char* size_s= (char*)malloc(size*sizeof(char));
                   

		    sprintf(size_s,"%ld",size);
		    
		



            msg1 =&buffer[0];
			
            strcpy(msg1,"UPC ");
            msg1 = msg1 +4*sizeof(char);
            strcpy(msg1,size_s);
		    msg1 = msg1 + strlen(size_s);
		    strcpy(msg1," ");
			msg1 = msg1 + strlen(" "); 
            strcpy(msg1,file_buffer);
            msg1 = msg1 + strlen(file_buffer);
            *(msg1) = '\0';

           msg1 =&buffer[0];
           
           

			free(file_buffer);
            free(p_dados);
			free(size_s);
			
                    nbytes=strlen(buffer);
                    nleft=nbytes;
                    
                    
                    
                    while(nleft > 0)	{
                         if( (nwritten=write(fdCS,msg1,nleft)) == -1){
                                printf("erro no write\n");
                                exit(1);
                            		}
                         if(nwritten==0)break;
                         nleft-=nwritten;
                         msg1+=nwritten;
                         }
                         
                p_resposta =&resposta[0];      
		msg1 =&buffer[0];

                         
                         
                         
       		nleft=strlen("AWC ok\n");
		nread = 1;

            while(nleft>0){
              if((nread = read(fdCS,msg1,nleft)) == -1){
		       			printf("Erro a ler a resposta \n");
		       			exit(1);
         			}
		printf("%s",msg1);
             	nleft-=nread;
                msg1+=nread;
           	}
		msg1 =&buffer[0];
		
		printf("p_respota: %s\n",msg1);


                            if(!strcmp(p_resposta,"AWC ok\n")){
                                    printf("Ficheiro Gravado com Sucesso!\n");
                            }else if (!strcmp(p_resposta,"AWC nok\n")){
                                    printf("Ficheiro não foi gravado com sucesso!\n");
                            }else{
                                printf("Erro no protocolo de comunicação!\n");
                            }
		close(fdCS);



          }else if(!strcmp(command,"exit")){


          printf("Bye!\n"); close(fd);return 0 ;


          }else {puts("Command not defined");}



 	}


	return 0;
}
