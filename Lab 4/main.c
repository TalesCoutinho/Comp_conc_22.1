#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

int *list;
float *saidaSeq;
float *saidaConc;
int nthreads;

double inicio;
double fim;

int N;

int i_global=0;
pthread_mutex_t bastao;





int ehPrimo(int n){
  int i, flag = 0;
  if (n == 0 || n == 1) return 0;
  for (i = 2; i <= n / 2; ++i) {
    if (n % i == 0) {
      return 0;
    };
  };
  return 1;
};

void processaPrimos(int vetorEntrada[], float vetorSaida[], int dim) {
  for(int i = 0; i < dim; i++){
    if(ehPrimo(list[i]) == 1){
      saidaSeq[i] = sqrt(list[i]);
    }else{
      saidaSeq[i] = list[i];
    };
  };
}



void *conta_primos_2 (void *threadid) {
  pthread_mutex_lock(&bastao);
  int i = i_global;
  i_global++;
  pthread_mutex_unlock(&bastao);
  while (i < N) {
     if(ehPrimo(list[i])) saidaConc[i] = sqrt(list[i]);
     else saidaConc[i] = list[i];
     pthread_mutex_lock(&bastao);
     i = i_global; //incrementa o contador de tarefas realizadas 
     i_global++;
     pthread_mutex_unlock(&bastao);
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if(argc<3) {
        printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
        return 1;
    }
    N = atoi(argv[1]);
    nthreads = atoi(argv[2]);


    srand((unsigned) time(NULL));

    pthread_t *tid;


    list = (int *) malloc(sizeof(int) * N);
    if (list == NULL) {printf("ERRO--malloc\n"); return 2;}
    saidaSeq = (float *) malloc(sizeof(float) * N);
    if (saidaSeq == NULL) {printf("ERRO--malloc\n"); return 2;}
    saidaConc = (float *) malloc(sizeof(float) * N);
    if (saidaConc == NULL) {printf("ERRO--malloc\n"); return 2;}

    for(int i=0; i < N; i++){
        list[i] = (int)(rand()%100);
    };

    printf("------SEQUENCIAL------\n");
    GET_TIME(inicio);
    processaPrimos(list, saidaSeq, N);
    GET_TIME(fim);
    printf("Tempo: %f\n", (fim-inicio));


    printf("\n");

    
    printf("------CONCORRENTE------\n");
    int t, id[nthreads];
    GET_TIME(inicio);
    tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
    if(tid==NULL) {puts("ERRO--malloc"); return 2;}
    pthread_mutex_init(&bastao, NULL);

   for(int i=0; i<nthreads; i++) {
      if(pthread_create(tid+i, NULL, conta_primos_2, NULL)){
         puts("ERRO--pthread_create"); return 3;
      }
   } 

   for(int i=0; i<nthreads; i++) {
      pthread_join(*(tid+i), NULL);
   }
    pthread_mutex_destroy(&bastao);

    GET_TIME(fim);
    printf("Tempo: %f\n", (fim-inicio));

    for(int i = 0; i < N; i++){
        if(saidaSeq[i] != saidaConc[i]){
            printf("Deu ruim\n");
            break;
        }
    }


    free(list);
    free(saidaSeq);
    free(saidaConc);
    free(tid);
    pthread_mutex_destroy(&bastao);
    return 0;
}
