#include <cstdio>
#include <omp.h>

/* Доп. условие: Потоки распечатывают свои идентификаторы в обратном порядке */

int main() {

    int max_threads = omp_get_max_threads();

    /* используем omp_lock для синхронизации */
    omp_lock_t lock[max_threads];
    for (int i = 0; i < max_threads; i++) {
        omp_init_lock(&lock[i]);
        omp_set_lock(&lock[i]);
    }

    #pragma omp parallel shared(lock) default(none)
    {
        int i = omp_get_thread_num();
        int n = omp_get_num_threads();

        if (i < n - 1)
            omp_set_lock(&lock[i]);

        printf("Hello World! My id is %d\n", i);

        if (i > 0)
            omp_unset_lock(&lock[i-1]);
    }
    return 0;
}
