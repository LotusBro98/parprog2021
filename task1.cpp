#include <cstdio>
#include <omp.h>

int main() {

    int n_threads = omp_get_max_threads();

    omp_lock_t lock[n_threads];
    for (int i = 0; i < n_threads; i++) {
        omp_init_lock(&lock[i]);
        if (i < n_threads - 1) {
            omp_set_lock(&lock[i]);
        }
    }

    #pragma omp parallel shared(lock, n_threads) default(none)
    {
        int i = omp_get_thread_num();
        omp_set_lock(&lock[i]);

        printf("Hello World! My id is %d\n", i);

        if (i > 0)
            omp_unset_lock(&lock[i-1]);
    }
    return 0;
}
