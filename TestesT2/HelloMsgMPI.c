#include <stdio.h>
#include <string.h>
#include "mpi.h"
 
int main(int argc, char** argv) {
  int id;            /* Identificador do processo */
  int n;             /* Numero de processos */
  int tag = 50;      /* Tag das mensagens */
  char message[100]; /* Buffer para as mensagens */
  int hostsize;      /* Tamanho do nome do nodo */
  char hostname[MPI_MAX_PROCESSOR_NAME];
  MPI_Status status; /* Status de retorno */
 
  MPI_Init(&argc, &argv);
  MPI_Get_processor_name(hostname, &hostsize);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &n);
  if (id != 0) {
     int dest = 0;   /* Identificador do processo destino */
     sprintf(message, "Greetings from process %d on %s!", id, hostname);
     MPI_Send (message, strlen(message)+1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
  }
  else {
     int source;   /* Identificador do processo origem */
     for (source = 1; source < n; source++) {
         MPI_Recv (message, 100, MPI_CHAR , source, tag, MPI_COMM_WORLD, &status);
         printf("%s received> %s\n", hostname, message);
     }
  }
  MPI_Finalize();
  return 0;
}
