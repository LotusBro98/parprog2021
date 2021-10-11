#include <cstdio>
#include <omp.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>

/* Доп. условия:
 * N задается аргументом запуска
 * Использовать условия reduction, schedule
 */

int main(int argc, char * argv[]) {
    /* Парсим аргумент N */
    if (argc != 2) {
        printf("Wrong number of arguments! Expected 1 number\n");
        return -1;
    }

    char * end;
    uint64_t N = strtoull(argv[1], &end, 10);
    if (end - argv[1] != strlen(argv[1])) {
        printf("Wrong argument: %s\nExpected number\n");
        return -1;
    }

    /* Используем omp parallel for вместе с reduction для суммирования */
    uint64_t sum = 0;

    #pragma omp parallel for default(none) shared(N) reduction(+:sum) schedule(static)
    for (uint64_t i = 1; i <= N; i++) {
        sum += i;
    }

    /* Выводим результат */
    printf("Sum from 1 to %llu is %llu\n", N, sum);

    return 0;
}
