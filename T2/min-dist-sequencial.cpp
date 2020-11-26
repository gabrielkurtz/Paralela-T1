/* min-dist-dc3.cpp (Roland Teodorowitsch; 29 out. 2020)
 * Compilation: mpic++ -o min-dist-dc-mpi min-dist-dc3.cpp -lm
 * Note: Includes some code from the sequential solution of the
 *       "Closest Pair of Points" problem from the
 *       14th Marathon of Parallel Programming avaiable at
 *       http://lspd.mackenzie.br/marathon/19/points.zip
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <algorithm>
#include <mpi.h>

#define SIZE 10000000
#define START 1000000
#define STEP  1000000

#define EPS 0.00000000001
#define BRUTEFORCESSIZE 200

using namespace std;

typedef struct {
    double x;
    double y;
} point_t;

point_t points[SIZE];
point_t border[SIZE];

unsigned long long llrand() {
    unsigned long long r = 0;
    int i;
    for (i = 0; i < 5; ++i)
        r = (r << 15) | (rand() & 0x7FFF);
    return r & 0xFFFFFFFFFFFFFFFFULL;
}

void points_generate(point_t *points, int size, int seed) {
    int p, i, found;
    double x, y;
    srand(seed);
    p = 0;
    while (p<size) {
        x = ((double)(llrand() % 20000000000) - 10000000000) / 1000.0;
        y = ((double)(llrand() % 20000000000) - 10000000000) / 1000.0;
        if (x >= -10000000.0 && x <= 10000000.0 && y >= -10000000.0 && y <= 10000000.0) {
            points[p].x = x;
            points[p].y = y;
            p++;
        }
    }
}

bool compX(const point_t &a, const point_t &b) {
    if (a.x == b.x)
        return a.y < b.y;
    return a.x < b.x;
}

bool compY(const point_t &a, const point_t &b) {
    if (a.y == b.y)
        return a.x < b.x;
    return a.y < b.y;
}

double points_distance_sqr(point_t *p1, point_t *p2) {
    double dx, dy;
    dx = p1->x - p2->x;
    dy = p1->y - p2->y;
    return dx*dx + dy*dy;
}

double points_min_distance_dc(point_t *point,point_t *border,int l, int r, int id) {
    double minDist = DBL_MAX;
    double dist;
    int i, j;
    if (r-l+1 <= BRUTEFORCESSIZE) {
        for (i=l; i<r; i++){
            for (j = i+1; j<=r; j++) {
                dist = points_distance_sqr(point+i, point+j);
                if (dist<minDist) {
                    minDist = dist;
                }
            }
        }
        return minDist;
    }

    int m = (l+r)/2;
    double dL = points_min_distance_dc(point,border,l,m, id);
    double dR = points_min_distance_dc(point,border,m,r, id);
    // printf("%d) dL - dR: %.2lf %.2lf\n", id, dL, dR);
    minDist = (dL < dR ? dL : dR);

    int k = l;
    for(i=m-1; i>=l && fabs(point[i].x-point[m].x)<minDist; i--)
        border[k++] = point[i];
    for(i=m+1; i<=r && fabs(point[i].x-point[m].x)<minDist; i++)
        border[k++] = point[i];

    if (k-l <= 1) return minDist;

    sort(&border[l], &border[l]+(k-l), compY);

    for (i=l; i<k; i++) {
        for (j=i+1; j<k && border[j].y - border[i].y < minDist; j++) {
            dist = points_distance_sqr(border+i, border+j);
            if (dist < minDist)
                minDist = dist;
        }
    }

    return minDist;
}

int main(int argc, char *argv[]) {
    int i, p, id;
    double elapsed_time;
    
   int raiz=0;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    if(id==raiz)
        printf("\nRAIZ) Comm size: %d\n", p);
    
    printf("%d) Comm rank: %d\n", id, id);

    points_generate(points,SIZE,11);
    sort(&points[0], &points[SIZE], compX);
    
    // MPI_Barrier(MPI_COMM_WORLD);
    for (i=START; i<=SIZE; i+=STEP) {
        
        MPI_Barrier(MPI_COMM_WORLD);
        if(id==raiz) printf("%d) ------ Size: %d ------\n", id, i);
        // MPI_Barrier(MPI_COMM_WORLD);
        elapsed_time = -MPI_Wtime();

        int sector_size = i/p;
        printf("%d) Sector size: %d\n", id, sector_size);
        int start = id*sector_size;
        int end = (i - (p-id-1)*sector_size - 1);
        printf("%d) Start - End: %d %d\n", id, start, end);
        
        double minDist_divisao = sqrt(points_min_distance_dc(points,border,start,end, id));
        double minDist_divisao_reduzido;
        
        printf("%d) Minimo Divisao: %.6lf\n", id, minDist_divisao);
        // MPI_Barrier(MPI_COMM_WORLD);
        // MPI_Reduce(&minDist_divisao, &minDist_divisao_reduzido, 1, MPI_DOUBLE, MPI_MIN, raiz, MPI_COMM_WORLD);
        MPI_Allreduce(&minDist_divisao, &minDist_divisao_reduzido, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);

        printf("%d) Minimo divisao reduzido: %.6lf\n", id, minDist_divisao_reduzido);
 
        // double elapsed_time_reduzido;
        // MPI_Reduce(&elapsed_time, &elapsed_time_reduzido, 1, MPI_DOUBLE, MPI_MIN, raiz, MPI_COMM_WORLD);
        elapsed_time += MPI_Wtime();
        fprintf(stderr,"%d) %d %lf\n",id, i,elapsed_time);
        // MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}