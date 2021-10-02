#include <cstdio>
#include <omp.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <ctime>

#define N 1000
#define M 1000
#define K 1000

#define N_REPEATS 20

double fRand(double fMin, double fMax)
{
    double f = ((double)rand()) / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

struct timespec start;

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double)(ts.tv_sec - start.tv_sec) + (double)(ts.tv_nsec - start.tv_nsec) / 1.0e9;
}

double A[N][K];
double B[K][M];
double C[N][M];

double B_T[M][K];

double time_all = 0;

int main(int argc, char * argv[]) {

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    srand(time(NULL));

    /* Initialise A and B with random numbers */
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < K; ++j)
            A[i][j] = fRand(-1, 1);

    for (int i = 0; i < K; ++i)
        for (int j = 0; j < M; ++j)
            B[i][j] = fRand(-1, 1);

    /* Run benchmark N_REPEATS times */
    for (int n = 0; n < N_REPEATS; n++) {
        /* record start time */
        double start_clk = get_time();

        /* Transpose matrix for cache-friendly code */
        for (int i = 0; i < K; i++) {
            for (int j = 0; j < M; j++) {
                B_T[j][i] = B[i][j];
            }
        }

        /* Run parallel for loop. Split across N dimension */
        #pragma omp parallel for default(none) shared(A, B_T, C) schedule(dynamic)
        for (int i = 0; i < N; i++) {
            double *A_row = A[i];
            for (int j = 0; j < M; j++) {
                double sum = 0;
                double *B_col = B_T[j];
                for (int k = 0; k < K; k++) {
                    sum += A_row[k] * B_col[k];
                }
                C[i][j] = sum;
            }
        }

        /* record  end time */
        double end_clk = get_time();
        time_all += end_clk - start_clk;
        printf("Benchmark iteration %d / %d. Time: %lf\n", n, N_REPEATS, end_clk - start_clk);
    }

    time_all /= N_REPEATS;
    printf("Average multiplication time = %lf s\n", time_all);

    return 0;
}
