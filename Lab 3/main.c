#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include"timer.h"

int nthreads;
long long int N;

float *list;

double inicio;
double fim;

typedef struct{
    float min;
    float max;
} Valores;

void * tarefa(void * arg){
    int id = (intptr_t) arg;
    Valores *local;

    local = (Valores*) malloc(sizeof(Valores));
    if(local == NULL){
        printf("ERRO--malloc\n");
        exit(2);
    };
    local->max = list[0];
    local->min = list[0];
    for(int i = id; i < N; i += nthreads){
        if(list[i] > local->max) local->max = list[i];
        if(list[i] < local->min) local->min = list[i];
    }
    pthread_exit((void *) local); 
}

void sequencial(float list[]){
    printf("-----SEQUENCIAL-----\n");
    GET_TIME(inicio);
    Valores minmax;
    minmax.min = list[0];
    minmax.max = list[0];
    for(int i = 0; i < N; i++){
        if(list[i] > minmax.max) minmax.max = list[i];
        if(list[i] < minmax.min) minmax.min = list[i];
    }
    GET_TIME(fim);
    printf("Maximo: %f\n", minmax.max);
    printf("Minimo: %f\n", minmax.min);
    double delta = fim - inicio;
    printf("Tempo: %lf\n", delta);
}

int main(int argc, char* argv[]){
    
    srand((unsigned) time(NULL));
    pthread_t *tid;

    Valores absoluto;
    Valores *resposta;

    if(argc<2) {
        printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
        return 1;
    }
    N = atoi(argv[1]);
    nthreads = atoi(argv[2]);

    list = (float*) malloc(sizeof(float)*N);
    if(list == NULL) {
        printf("ERRO--malloc\n");
        return 2;
    }
    for(int i=0; i < N; i++){
        list[i] = (float)(rand()%1000000)+(1.0/(float)((rand()%1000)+1.0));
    }

    
    sequencial(list);

    printf("\n\n-----CONCORRENTE-----\n");

    GET_TIME(inicio);
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
        if(tid==NULL) {
            printf("ERRO--malloc\n");
            return 2;
        }

    for(int i = 0; i < nthreads; i++){
        if(pthread_create(tid+i, NULL, tarefa, (void*) (intptr_t) i)){
           printf("ERRO--pthread_create\n");
           return 3;
        }
    }

    absoluto.max = list[0];
    absoluto.min = list[0];

    for(int i = 0; i < nthreads; i++){
        if(pthread_join(*(tid+i), (void**) &resposta)){
            fprintf(stderr, "ERRO--pthread_join\n");
            return 3;
        }
        if(absoluto.max < resposta->max) absoluto.max = resposta->max;
        if(absoluto.min > resposta->min) absoluto.min = resposta->min;

    };
    GET_TIME(fim);
  
    printf("Maximo:  %lf\n", absoluto.max);
    printf("Minimo:  %lf\n", absoluto.min);
    printf("Tempo:  %lf\n", fim - inicio);
    
    free(resposta);
    free(tid);
    free(list);
    return 0;
}