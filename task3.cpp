#include <cstdio>
#include <omp.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#define N 100000

int main(int argc, char * argv[]) {
    uint64_t a[N];
    uint64_t b[N];

    /* Generate array */
    for (uint64_t i = 0; i < N; i++) {
        a[i] = i;
    }

    /* Generate right answer for comparison only */
    for (uint64_t i = 0; i < N; i++) {
        b[i] = a[(i - 1) % N] * a[i] * a[(i + 1) % N] / 3;
    }

    /* Transform "a" in parallel manner without allocation of new array */
    #pragma omp parallel default(none) shared(a)
    {
        int id = omp_get_thread_num();
        int n_threads = omp_get_num_threads();

        /* Calculate range */
        int n_elems = (N + id) / n_threads;
        int n0 = N / n_threads;
        int start = (n_elems == n0) ? n_elems * id : N - n_elems * (n_threads - id);
        int end = start + n_elems;

        /* bufferize edge elements */
        uint64_t ai_1 = a[(start-1) % N];
        uint64_t a_last = a[end % N];
        #pragma omp barrier

        /* perform calculations for range */
        for (uint64_t i = start; i < end - 1; i++) {
            uint64_t tmp = a[i];
            a[i] = ai_1 * a[i] * a[i+1] / 3;
            ai_1 = tmp;
        }
        a[end - 1] = ai_1 * a[end - 1] * a_last / 3;
    }

    /* Compare arrays */
    for (int i = 0; i < N; i++) {
        if (a[i] != b[i]) {
            printf("Error i=%d a'[i]=%llu b[i]=%llu\n", i, a[i], b[i]);
            return -1;
        }
    }

    return 0;
}
