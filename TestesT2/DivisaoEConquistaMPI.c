// DivisaoEConquista.c (origem desconhecida; adaptado por Roland Teodorowitsch; 22 out. 2019)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <sys/time.h>

#define TAMANHO 100000


void swap(int *a, int *b);
int partition(int *vetor, int left, int right);
void quicksort(int *vetor, int left, int right);
void calcula_parentesco(int size, int id, int *pai, int *filhoEsq, int *filhoDir, int *level);
int *interleaving(int *vetor, int tam);
void mostraVetor(int *v, int tam);


/*-- inicio do quick sort --*/

void swap(int *a, int *b) {
  int tmp;
  tmp = *a;
  *a = *b;
  *b = tmp;
}

int partition(int *vetor, int left, int right) {
  int i, j;

  i = left;
  for (j = left + 1; j <= right; ++j) {
      if (vetor[j] < vetor[left]) {
         ++i;
         swap(&vetor[i], &vetor[j]);
      }
  }
  swap(&vetor[left], &vetor[i]);
  return i;
}

void quicksort(int *vetor, int left, int right) {
  int r;

  if (right > left) {
     r = partition(vetor, left, right);
     quicksort(vetor, left, r - 1);
     quicksort(vetor, r + 1, right);
  }
}

/*-- fim do quick sort --*/


/* calcula o parentesco considerando uma arvore binária heap-like */
void calcula_parentesco(int size, int id, int *pai, int *filhoEsq, int *filhoDir, int *level) {
  *filhoEsq = 2 * id + 1;
  *filhoDir = 2 * id + 2;
  *pai = (int)floor((id - 1) / 2);
  *level = floor(log2(*pai + 1)) + 1;
  if (*filhoEsq >= size)
  	*filhoEsq = -1;
  if (*filhoDir >= size)
  	*filhoDir = -1;
}

/* intercala as duas particoes do vetor */
int *interleaving(int *vetor, int tam) {
  int *vetor_auxiliar;
  int i1, i2, i_aux;

  vetor_auxiliar = (int *)malloc(sizeof(int) * tam);
  i1 = 0;
  i2 = tam / 2;
  for (i_aux = 0; i_aux < tam; i_aux++) {
      if (((vetor[i1] <= vetor[i2]) && (i1 < (tam / 2))) || (i2 == tam))
         vetor_auxiliar[i_aux] = vetor[i1++];
      else
         vetor_auxiliar[i_aux] = vetor[i2++];
  }
  return vetor_auxiliar;
}

void mostraVetor(int *v, int tam) {
  int i;
  printf("[ %d",v[0]);
  for (i=1; i<tam; ++i)
      printf(", %d", v[i]);
  printf(" ]\n");
}

int main(int argc, char **argv) {
  MPI_Status status;
  int *vetor, *vetor_aux;
  int id, size, mid, tag = 10;
  int pai, filhoEsq, filhoDir, level;

  vetor = (int *)malloc(TAMANHO * sizeof(int));
  if (vetor == NULL) {
     fprintf(stderr,"Erro ao alocar vetor\n");
     exit(1);
  }

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  calcula_parentesco(size, id, &pai, &filhoEsq, &filhoDir, &level);

  /* inicializa o vetor */
  if (id == 0) {
     double tempo;
     int i;
     for (i = 0; i < TAMANHO; ++i)
         vetor[i] = TAMANHO - i;
     tempo = -MPI_Wtime();
     mid = TAMANHO / 2;
     // envia particao para o filho da esquerda
     MPI_Send(vetor, mid, MPI_INT, filhoEsq, tag, MPI_COMM_WORLD);
     // envia particao para o filho da direita
     MPI_Send(vetor + mid, mid, MPI_INT, filhoDir, tag, MPI_COMM_WORLD);
     // recebe resultados do filho da esquerda
     MPI_Recv(vetor, mid, MPI_INT, filhoEsq, tag, MPI_COMM_WORLD, &status);
     // recebe resultados do filho da direita
     MPI_Recv(vetor + mid, mid, MPI_INT, filhoDir, tag, MPI_COMM_WORLD, &status);
     // mescla os resultados recebidos dos filhos
     vetor_aux = interleaving(vetor, TAMANHO);
     tempo += MPI_Wtime();
     // Verifica se esta ordenado
     for (i = 0; i < TAMANHO-1; ++i) {
         if (vetor_aux[i] > vetor_aux[i+1]) {
            free((void *)vetor_aux);
            free((void *)vetor);
            MPI_Finalize();
            exit(1);
         }
     }
     free((void *)vetor_aux);
     printf("%d %d %lf\n", TAMANHO, size, tempo);
  }
  else {
     int tam = TAMANHO / pow(2, level);
     mid = tam / 2;
     // recebe particao do pai
     MPI_Recv(vetor, tam, MPI_INT, pai, tag, MPI_COMM_WORLD, &status);
     // verifica se possui filhos
     if ((filhoEsq != -1) && (filhoDir != -1)) {  // se possui filhos...
        // envia particao para o filho da esquerda
  	MPI_Send(vetor, mid, MPI_INT, filhoEsq, tag, MPI_COMM_WORLD);
        // envia particao para o filho da direita
  	MPI_Send(vetor + mid, mid, MPI_INT, filhoDir, tag, MPI_COMM_WORLD);
        // recebe resultados do filho da esquerda
  	MPI_Recv(vetor, mid, MPI_INT, filhoEsq, tag, MPI_COMM_WORLD, &status);
        // recebe resultados do filho da direita
        MPI_Recv(vetor + mid, mid, MPI_INT, filhoDir, tag, MPI_COMM_WORLD, &status);
        // mescla os resultados recebidos dos filhos
  	vetor_aux = interleaving(vetor, tam);
        // envia resultados para o pai
  	MPI_Send(vetor_aux, tam, MPI_INT, pai, tag, MPI_COMM_WORLD);
        free((void *)vetor_aux);
     }
     else {
        // se nao possui filhos, ordena a particao localmente
        quicksort(vetor, 0, tam - 1);
        // envia resultados para o pai
  	MPI_Send(vetor, tam, MPI_INT, pai, tag, MPI_COMM_WORLD);
     }
  }
  MPI_Finalize();
  free((void *)vetor);
  return 0;
}

