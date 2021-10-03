#include <omp.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <algorithm>

#define X 1

int parse_args(int argc, char * argv[], uint64_t * N) {
    if (argc != 2) {
        printf("Wrong number of arguments! Expected 1 number\n");
        return -1;
    }

    char * pend;
    *N = strtoull(argv[1], &pend, 10);
    if (pend - argv[1] != strlen(argv[1])) {
        printf("Wrong argument: %s\nExpected number\n");
        return -1;
    }

    return 0;
}

int main(int argc, char * argv[]) {
    uint64_t N;
    if (parse_args(argc, argv, &N))
        return -1;

    double sum = 1;
    double an = 1;
    double x = X;
    int n_threads = omp_get_max_threads();
    uint64_t CHUNK = (N + n_threads) / n_threads;

    #pragma omp parallel for ordered default(none) shared(N, x, sum, an, CHUNK)
    for (uint64_t i0 = 1; i0 < N; i0+= CHUNK) {
        double an_mul = 1;
        double an_sum = 0;
        uint64_t end = std::min(i0 + CHUNK, N);
        for (uint64_t i = i0; i < end; i++) {
            an_mul *= x / i;
            an_sum += an_mul;
        }
        #pragma omp ordered
        {
            sum += an_sum * an;
            an *= an_mul;
        }
    }

    printf("e^(%.15lf) = %.15lf\n", x, sum);

    return 0;
}
