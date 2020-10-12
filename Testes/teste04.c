// Arquivo: teste04.c (Roland Teodorowitsch; 19 abr. 2018)
#include <stdio.h>

#define TAM 12

int main() {
   int i, a[TAM], b[TAM], c[TAM];

   printf("omp_get_num_procs() = %d\n",omp_get_num_procs());
   #pragma omp parallel private(i)
   {
      int id = omp_get_thread_num();
      int nt = omp_get_num_threads();
      int ini = id * TAM / nt;
      int fim = (id+1)*TAM / nt;
      for (i=ini; i<fim; i++) {
          printf("i=%02d [%d/%d]\n",i,omp_get_thread_num(),omp_get_num_threads());
          a[i] = a[i] + b[i];
      }
   }
   return 0;
}
