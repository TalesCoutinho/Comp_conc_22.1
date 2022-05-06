#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include"timer.h"

int * matriz1;
int * matriz2;

int * saida;
int * saida_seq;

int nthreads;

typedef struct{
    int id;
    int dim;
} tArgs;



void * tarefa(void * arg){
    int soma = 0;
    tArgs *linha = (tArgs *) arg;
    for(int i = linha->id; i < linha->dim; i +=nthreads){
         for(int j = 0; j < linha->dim; j++){
             for(int k = 0; k < linha->dim; k++){
                soma += matriz1[i * linha->dim + k] * matriz2[k * linha->dim + j];
             }
            saida[i * linha->dim + j] = soma;
            soma = 0;
        }
    }
    pthread_exit(NULL);
}

void sequencial(int dim){
    int soma = 0;
    for(int i = 0; i < dim; i++){
         for(int j = 0; j < dim; j++){
             for(int k = 0; k < dim; k++){
                soma += matriz1[i * dim + k] * matriz2[k * dim + j];
             }
            saida_seq[i * dim + j] = soma;
            soma = 0;
        }   
    }
    // for(int i = 1; i < dim * dim + 1; i++){
    //     printf("%d ", saida[i -1]);
    //     if(i%dim == 0) printf("\n");
    // }
}


int main(int argc, char* argv[]) {
    int dim;
    pthread_t *tid;
    
    tArgs *args;

    double inicio, fim;

    GET_TIME(inicio);
    // Recebe os argumentos e atribui às variáveis
    if(argc<3) {
        printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
        return 1;
    }
    dim = atoi(argv[1]);
    nthreads = atoi(argv[2]);
    if (nthreads > dim) nthreads=dim;


    //Aloca a memória para as matrizes

    matriz1 = (int *) malloc(sizeof(int) * dim * dim);
    if(matriz1 == NULL){ printf("ERRO--malloc\n"); return 2;}

    matriz2 = (int *) malloc(sizeof(int) * dim * dim);
    if(matriz2 == NULL){ printf("ERRO--malloc\n"); return 2;}

    saida = (int *) malloc(sizeof(int) * dim * dim);
    if(saida == NULL){ printf("ERRO--malloc\n"); return 2;}

    saida_seq = (int *) malloc(sizeof(int) * dim * dim);
    if(saida_seq == NULL){ printf("ERRO--malloc\n"); return 2;}
    

    //Preenche as matrizes

    srand(time(NULL));
    for(int i = 0; i < dim * dim; i++){
            matriz1[i] = rand() % 100;
            matriz2[i] = rand() % 100;
            saida[i] = 0;
    };
    
    GET_TIME(fim);
    double delta = fim - inicio;
    printf("Tempo inicializacao:%lf\n", delta);

    
    GET_TIME(inicio);

    tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
    if(tid==NULL) {puts("ERRO--malloc"); return 2;}
    args = (tArgs*) malloc(sizeof(tArgs)*nthreads);
    if(args==NULL) {puts("ERRO--malloc"); return 2;}

    int div = dim / nthreads;
    int rest = dim % nthreads;

    for(int i=0; i<nthreads; i++) {
      (args+i)->id = i;
      (args+i)->dim = dim;
      if(pthread_create(tid+i, NULL, tarefa, (void*) (args+i))){
         puts("ERRO--pthread_create"); return 3;
      }
   } 
      for(int i=0; i<nthreads; i++) {
      pthread_join(*(tid+i), NULL);
   }

   /*////////////////////////////////////////////////////
       for(int i = 1; i <= dim * dim; i++){
       printf("%d ", matriz1[i-1]);
       if(i % dim == 0) printf("\n");
   }
    printf("\n");
      for(int i = 1; i <= dim * dim; i++){
       printf("%d ", matriz2[i-1]);
       if(i % dim == 0) printf("\n");
   }
    printf("\n");

      for(int i = 1; i <= dim * dim; i++){
       printf("%d ", saida[i-1]);
       if(i % dim == 0) printf("\n");
   }
    printf("\n");
*/////////////////////////////////////////////////////

   for(int i=0; i<nthreads; i++) {
      pthread_join(*(tid+i), NULL);
   }
   
   GET_TIME(fim)   
   double delta_conc = fim - inicio;
   printf("Tempo multiplicacao:%lf\n", delta_conc);

   GET_TIME(inicio);
   sequencial(dim);
   GET_TIME(fim);
   double delta_seq = fim - inicio;
   printf("Tempo multiplicacao sequencial:%lf\n", delta_seq);
   int igual = 0;
   for(int i = 0; i < dim * dim; i++){
       if(saida[i] != saida_seq[i]) igual = 1;
   };

   printf("Ganho de tempo: %lf\n", (delta_seq/delta_conc));

   if(igual == 0){
       printf("Tudo em ordem.\n");
   } else{
       printf("Os resultados nao sao iguais.\n");
   };

    //Libera a memória alocada para as matrizes

    free(matriz1);
    free(matriz2);
    free(saida);

    return 0;
}