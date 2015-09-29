#include <stdio.h>      /* printf */
#include <time.h>       /* time_t, struct tm, difftime, time, mktime */

int main (){

  double seconds;

  time_t t_registo, t_entrega;
  
  struct tm tm_registo;
  struct tm tm_entrega;
  
  static const char mon_name[][4] = {
    "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
    "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
  };
  char aqt_time[18];
  
  
  /* codigo de registo */
  time(&t_registo);  /*guarda tempo actual*/
  tm_registo = *localtime(&t_registo);  /* copia conteudo para outra estrutura de modo a imprimir */
  
  sprintf(aqt_time, "%.2d%.3s%.4d_%.2d:%.2d:%.2d",
    	tm_registo.tm_mday, mon_name[tm_registo.tm_mon], 1900 + tm_registo.tm_year,
   		tm_registo.tm_hour, tm_registo.tm_min, tm_registo.tm_sec);
  printf("data de entrega : %s\n", aqt_time);
  
  
  
  /* CODIGO DE ENTREGA */
  time(&t_entrega);
  
  /* modificador para testar*/
  tm_entrega = *localtime(&t_entrega);
  tm_entrega.tm_min = tm_entrega.tm_min +5;
  printf("os minutos sao : %d\n", tm_entrega.tm_min);
    
  if( 300 > (seconds = difftime(mktime(&tm_entrega), t_registo) ) ){  /*para testar em vez de t_entrega altero para mktime(&tm_entrega)*/
  	printf("Dentro do tempo seconds: %.f\n", seconds);
  }
  else{
  	printf("Fora do tempo\n");
  }


  return 0;
}
