#include <omp.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <cmath>

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

    /* Решето Эратосфена */

    /* Число N является простым если у него нет делителей от 1 до N кроме 1 и N */
    /* Используем принцип динамического программирования - будем вычеркивать все кратные уже просмотренных простых чисел и сохранять результат */
    /* если число a > sqrt(N) делитель N, то N = b*a, b < sqrt(N). То есть если у N нет делителей <= sqrt(N), то и > sqrt(N) тоже нет */

    /* Создаем маску, в которой будем вычеркивать составные числа, заполняем нулями */
    bool * mask = new bool[N + 1];
    #pragma omp parallel for default(none) shared(mask, N)
    for (uint64_t i = 1; i <= N; i++)
        mask[i] = false;

    /* Так как оценка кол-ва простых чисел от 1 до N порядка log(N) то можно позволить себе создать структуру для их хранения */
    std::vector<uint64_t> numbers;

    /* Перебираем числа i от 2 до sqrt(N) и вычеркиваем все кратные i */
    for (uint64_t i = 2; i*i <= N; i++) {
        /* Если оно составное, не ищем кратные ему, так как они кратны простым числам, которые меньше i, и уже отмечены ранее */
        if (mask[i])
            continue;

        /* Отмечаем все кратные этому простому числу от i^2 до N */
        /* Можно начинать с i^2, потому что если число a < i^2 кратно i, то a = i * b, b < i, поэтому все числа кратные b уже вычеркнуты, поэтому и a уже вычеркнуто */
        #pragma omp parallel for default(none) shared(mask, N, i)
        for (uint64_t j = i*i; j <= N; j += i) {
            mask[j] = true;
        }
    }

    /* Собираем оставшиеся числа в более компактный вид */
    #pragma omp parallel for ordered default(none) shared(mask, N, numbers)
    for (uint64_t i = 1; i <= N; i++) {
        if (!mask[i]) {
            #pragma omp ordered
                numbers.push_back(i);
        }
    }

    delete[] mask;

    /* Печатаем результат */
    for (uint64_t i : numbers)
        printf("%llu ", i);
    printf("\n");

    return 0;
}
