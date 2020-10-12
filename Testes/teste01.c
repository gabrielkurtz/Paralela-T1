// Arquivo: teste01.c (Roland Teodorowitsch; 3 abr. 2018)
#include <stdio.h>

double calculo(int i) {
  double res = 0.0;
  int j;
  for (j=0;j<10000;++j)
      res += i;
  return res;
}

int main() {
  int i;
  double vetor[100000];

  #pragma omp parallel for
  for (i=0;i<100000;++i)
      vetor[i] = calculo(i);
  return 0;
}
