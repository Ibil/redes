############## parte resposta##########
char* palavra;
char resposta[250];

palavra = strtok(resposta," ");

if(!strcmp(resposta,"ERR") || !strcmp(resposta,"EOF")){
   printf("%s",palavra);
   break;
}

do{
	printf("A palavra : %s\n",palavra);
	palavra = strtok(NULL," ");
}
while(palavra != NULL);

 ############# fim parte resposta##########
 
 struct hostent *h;
 
 /*printf a deixar :: de onde veio o pedido*/
h=gethostbyaddr((char*)&clientaddr.sin_addr,sizeof(struct in_addr),AF_INET);
if(h==NULL){
	printf("LST - [%s:%hu]\n",inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
}
else{
	printf("LST - [%s:%hu]\n",h->h_name,ntohs(clientaddr.sin_port));
}


/*fetch file - versao que tem n_lines ao inicio*/
int i;
FILE *fp_txt;
int n_linhas;
char TES_ip[20];
char TES_port[6];

fp_txt = fopen("teste_le_file.txt", "r+");
fscanf(fp_txt,"%d", &n_linhas);
printf("O numero de linhas: %d\n", n_linhas);
for(i=0;i<n_linhas;i++){
	fscanf(fp_txt,"%s %s", TES_ip, TES_port);
	printf("TES_ip : %s\t TES_Port : %s\n", TES_ip, TES_port);
}
fclose(fp_txt);
 
 /* fetch file - versao que nao sabe n_lines*/
FILE *fp_txt;
int t_number;
char TES_ip[20];
char TES_port[6];

fp_txt = fopen("teste_le_file.txt", "r+");
for(t_number = 1;
	1 < fscanf(fp_txt,"%s %s", TES_ip, TES_port);
	t_number++){
	
	printf("Tnn : %d \t TES_ip : %s\t TES_Port : %s\n", t_number, TES_ip, TES_port);
}
fclose(fp_txt);
 

/* inutil acho - se tiver problemas ver isto
if(n_files<10){
	fseek(fp_SS,2* sizeof(char),SEEK_CUR);
	}
 else{
	fseek(fp_SS,1*sizeof(char),SEEK_CUR);
 }
 */
 
 
 ####  verfica se nome do ficheiro exite ############
 
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
 
 ######## fim do check_file_list ########
 
 
 
 
 
 
 
 
