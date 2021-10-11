#include <omp.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <algorithm>

/* Аргумент экспоненты */
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

    /* e^x = 1 + x^1/1! + x^2/2! + ... + x^N/n! + o(x^n) */
    /* Слагаемые удобно вычислять рекуррентным образом, тогда сложность вычисления суммы ряда будет O(n) */
    /* Но в таком случае появляется зависимость слагаемого от предыдущего */
    /* Сумма ряда будет разбита на слагаемые вида: an*(1 + a(n+1)/an + a(n+2)/an + ... + a(n+m)/an) */
    /* Суммы в скобках могут быть вычислены независимо, а потом домножены на an и сложены вместе */
    /* Чтобы получить an, каждый блок будет вычислять a(n+m)/an, и они по порядку будут перемножены */

    /* Инициализируем сумму и an */
    double sum = 1;
    double an = 1;
    double x = X;

    /* Вычисляем размер блока */
    int n_threads = omp_get_max_threads();
    uint64_t CHUNK = (N + n_threads) / n_threads;

    #pragma omp parallel for ordered default(none) shared(N, x, sum, an, CHUNK)
    for (uint64_t i0 = 1; i0 < N; i0+= CHUNK) {
        /* инициализируем локальную сумму и a(n+m)/an */
        double an_sum = 0;
        double an_mul = 1;

        /* Вычислим локальную сумму и a(n+m)/an в обычном цикле for */
        uint64_t end = std::min(i0 + CHUNK, N);
        for (uint64_t i = i0; i < end; i++) {
            an_mul *= x / i;
            an_sum += an_mul;
        }

        /* В исходном порядке перемножим a(n+m)/an, чтобы получить an,
         * затем в каждой итерации умножим локальную сумму на an и прибавим к общей */
        #pragma omp ordered
        {
            sum += an_sum * an;
            an *= an_mul;
        }
    }

    /* Выведем результат */
    printf("e^(%.15lf) = %.15lf\n", x, sum);

    return 0;
}
