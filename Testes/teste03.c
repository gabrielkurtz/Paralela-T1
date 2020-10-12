// Arquivo: teste03.c (Roland Teodorowitsch; 19 abr. 2018)
#include <stdio.h>

#define TAM 12

int main() {
   int i, a[TAM], b[TAM], c[TAM];

   printf("omp_get_num_procs() = %d\n",omp_get_num_procs());
#pragma omp parallel for schedule(static)
   for (i=0; i<TAM; ++i) {
       printf("i=%02d [%d/%d]\n",i,omp_get_thread_num(),omp_get_num_threads());
       a[i] = a[i] + b[i];
   }
   return 0;
}
