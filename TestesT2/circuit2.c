#include <stdio.h>
#include <mpi.h>

int check_circuit2(int, int);

int main(int argc, char *argv[]) {
  int i, id, p, total, sol, hs;
  char h[MPI_MAX_PROCESSOR_NAME];

  MPI_Init(&argc, &argv);
  MPI_Get_processor_name(h, &hs);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  for (sol = 0, i = id; i < 65536; i += p)
      sol += check_circuit2(id,i);
  MPI_Reduce(&sol,&total, 1, MPI_INT,
             MPI_SUM, 0, MPI_COMM_WORLD);
  printf("Process %d on %s is done\n", id, h);
  fflush(stdout);
  MPI_Finalize();
  if (id == 0)
     printf("> %d different soluctions\n",
            total);
  return 0;
}

#define EXTRACT_BIT(n,i) ((n&(1<<i))?1:0)

int check_circuit2(int id,int z) {
  int i, res, v[16];

  for (i = 0; i < 16; i++) v[i] = EXTRACT_BIT(z,i);
  res = (v[0] || v[1]) && (!v[1] || !v[3]) && (v[2] || v[3])
   && (!v[3] || !v[4]) && (v[4] || !v[5]) && (v[5] || !v[6])
   && (v[5] || v[6]) && (v[6] || !v[15]) && (v[7] || !v[8])
   && (!v[7] || !v[13]) && (v[8] || v[9]) && (v[8] || !v[9])
   && (!v[9] || !v[10]) && (v[9] || v[11])
   && (v[10] || v[11]) && (v[12] || v[13])
   && (v[13] || !v[14]) && (v[14] || v[15]);
  if (res) {
     printf("%d) %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n", id,
       v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],
       v[10],v[11],v[12],v[13],v[14],v[15]);
     fflush(stdout);
  }
  return res;
}
