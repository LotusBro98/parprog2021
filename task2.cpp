#include <cstdio>
#include <omp.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>

int main(int argc, char * argv[]) {
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

    uint64_t sum = 0;

    #pragma omp parallel for default(none) shared(N) reduction(+:sum) schedule(static)
    for (uint64_t i = 1; i <= N; i++) {
        sum += i;
    }

    printf("Sum from 1 to %llu is %llu\n", N, sum);

    return 0;
}
