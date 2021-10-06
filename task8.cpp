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



    return 0;
}
