// Arquivo: teste02.c (Roland Teodorowitsch; 3 abr. 2018)
#include <stdio.h>
#include <omp.h>

void calculo(int i);
void calculo(int i) {
  printf("%2d: omp_get_thread_num() = %d (total = %d)\n",i,omp_get_thread_num(),omp_get_num_threads());
}

int main() {
  int i,num_cpus;

  num_cpus = omp_get_num_procs();
  printf("omp_get_num_procs() = %d\n",num_cpus);
  printf("omp_get_num_threads() = %d\n",omp_get_num_threads());

  #pragma omp parallel for
  for (i=0;i<16;++i)
      calculo(i);

  omp_set_num_threads(num_cpus);
  #pragma omp parallel for
  for (i=16;i<32;++i)
      calculo(i);

  return 0;
}
