#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int *QID_index = 0;

/* EStrutura do TES com resultados*/

typedef struct{
	int QID;
	/*time_t t_registo;*/
}quest_form;

quest_form *db_quest_form;

int cria_instancia_QID(int sid){
	(db_quest_form[*QID_index]).QID = *QID_index;
	*QID_index = (*QID_index) + 1;
	return (*QID_index) - 1;
}


int main(void){
	
	/*db_quest_form = (quest_form*)malloc(sizeof(quest_form) * 100);*/

    QID_index = mmap(NULL, sizeof *QID_index, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	db_quest_form = mmap(NULL, sizeof *db_quest_form, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);


	db_quest_form = (quest_form*)malloc(sizeof(quest_form) * 100);
	

    if (fork() == 0) {
        printf(" pid = 0\n");
        printf("insctancia criada QID: %d\n", cria_instancia_QID(123));
        printf("insctancia criada QID: %d\n", cria_instancia_QID(789));
        exit(EXIT_SUCCESS);
    } else {
    	wait(NULL);
        printf("O pai diz %d\n", *QID_index);
        munmap(QID_index, sizeof *QID_index);
    }
    return 0;
}
