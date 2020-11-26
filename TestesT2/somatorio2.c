/* Somatorio2.c (Roland Teodorowitsch; 5 nov. 2020) */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define SIZE 1000
#define START 100
#define STEP  100

int main(int argc, char *argv[]) {
  int soma, soma1, soma2;
  int tamanho, tamanho1, tamanho2;
  int vetor[SIZE];
  int i, x, pai, filho1, filho2, p, id;
  MPI_Status status;

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  if (id == 0) {

	// NODO RAIZ - INICIALIZAÇÃO DO VETOR
        for (i=0 ; i<SIZE; i++)
            vetor[i] = i+1;

       	filho1 = id*2+1;
        filho2 = filho1+1;
	for (x=START; x<=SIZE; x+=STEP) {
        	tamanho = x;

	        // DIVIDE 1
        	tamanho1 = tamanho/2;
        	MPI_Send((void *)&tamanho1, 1, MPI_INT, filho1, 0, MPI_COMM_WORLD);
      		MPI_Send((void *)&vetor[0], tamanho1, MPI_INT, filho1, 0, MPI_COMM_WORLD);

        	// DIVIDE 2
        	tamanho2= tamanho - tamanho1;
        	MPI_Send((void *)&tamanho2, 1, MPI_INT, filho2, 0, MPI_COMM_WORLD);
        	MPI_Send((void *)&vetor[tamanho1], tamanho2, MPI_INT, filho2, 0, MPI_COMM_WORLD);

		// CONQUISTA
        	MPI_Recv((void *)&soma1, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        	MPI_Recv((void *)&soma2, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        	soma = soma1 + soma2;
        	printf("%d> para tamanho=%d, resultado final: %d...\n",id,x,soma);
	}

	tamanho = -1;
        MPI_Send((void *)&tamanho, 1, MPI_INT, filho1, 0, MPI_COMM_WORLD);
        MPI_Send((void *)&tamanho, 1, MPI_INT, filho2, 0, MPI_COMM_WORLD);
	printf("RAIZ[%d]> encerrando a execução...\n",id);

  }
  else {

     while (1) {

     MPI_Recv((void *)&tamanho, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
     if (tamanho == -1) {
        if (id >= p/2)
 	   printf("FOLHA[%d]> encerrando a execução...\n",id);
	else {
           filho1 = id*2+1;
           filho2 = filho1+1;
	   tamanho = -1;
           MPI_Send((void *)&tamanho, 1, MPI_INT, filho1, 0, MPI_COMM_WORLD);
           MPI_Send((void *)&tamanho, 1, MPI_INT, filho2, 0, MPI_COMM_WORLD);
 	   printf("INTERMEDIÁRIO[%d]> encerrando a execução...\n",id);
	}
	MPI_Finalize();
	return 0;
     }
     pai = status.MPI_SOURCE;
     MPI_Recv((void *)&vetor[0], tamanho, MPI_INT, pai, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

     if (id >= p/2) {

        // NODO FOLHA - SOMATÓRIO
        soma = 0;
        for (i=0; i<tamanho; ++i)
            soma+= vetor[i];
        MPI_Send((void *)&soma, 1, MPI_INT, pai, 0, MPI_COMM_WORLD);

     }
     else {

        // NODO INTERMEDIÁRIO

        // DIVIDE 1
        filho1 = id*2+1;
        tamanho1 = tamanho/2;
        MPI_Send((void *)&tamanho1, 1, MPI_INT, filho1, 0, MPI_COMM_WORLD);
        MPI_Send((void *)&vetor[0], tamanho1, MPI_INT, filho1, 0, MPI_COMM_WORLD);

        // DIVIDE 2
        filho2 = filho1+1;
        tamanho2= tamanho - tamanho1;
        MPI_Send((void *)&tamanho2, 1, MPI_INT, filho2, 0, MPI_COMM_WORLD);
        MPI_Send((void *)&vetor[tamanho1], tamanho2, MPI_INT, filho2, 0, MPI_COMM_WORLD);

	// CONQUISTA
        MPI_Recv((void *)&soma1, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        MPI_Recv((void *)&soma2, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        soma = soma1 + soma2;
        //printf("%d> resultado parcial: %d...\n",id,soma);
        MPI_Send((void *)&soma, 1, MPI_INT, pai, 0, MPI_COMM_WORLD);

     }

     }

  }

  MPI_Finalize();
  return 0;
}