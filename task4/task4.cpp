#include <cstdio>
#include <omp.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <ctime>

/* Доп. условия:
 * Адаптировать программу для перемножения больших матриц (>1000x1000).
 * Использовать методы динамического распределения нагрузки,
 * проанализировать эффективность. */

/* A(NxK) @ B(KxM) = C(NxM) */

#define N 1000
#define M 1000
#define K 1000

/* Сколько раз повторять перемножение */
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

/* Выделим память для матриц в секции data */
double A[N][K];
double B[K][M];
double C[N][M];

/* Транспонированная копия матрицы B */
double B_T[M][K];

int main(int argc, char * argv[]) {
    double time_all = 0;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    srand(time(NULL));

    /* Инициализируем A и B случайными числами */
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < K; ++j)
            A[i][j] = fRand(-1, 1);

    for (int i = 0; i < K; ++i)
        for (int j = 0; j < M; ++j)
            B[i][j] = fRand(-1, 1);

    /* N_REPEATS раз запустим перемножение, замерим время выполнения */
    for (int n = 0; n < N_REPEATS; n++) {
        /* Замерим время начала */
        double start_clk = get_time();

        /* Транспонируем матрицу B для cache-friendly кода */
        for (int i = 0; i < K; i++) {
            for (int j = 0; j < M; j++) {
                B_T[j][i] = B[i][j];
            }
        }

        /* Запустим перемножение в параллельном цикле for с разбиением вдоль измерения N */
        /* schedule(dynamic) динамически распределяет нагрузку между потоками */
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

        /* замерим время конца */
        double end_clk = get_time();
        time_all += end_clk - start_clk;
        printf("Benchmark iteration %d / %d. Time: %lf\n", n, N_REPEATS, end_clk - start_clk);
    }

    time_all /= N_REPEATS;
    printf("Average multiplication time = %lf s\n", time_all);

    return 0;
}
