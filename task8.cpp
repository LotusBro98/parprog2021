#include <omp.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <cmath>

#define N_MAX 10000

int get_array(int argc, char * argv[], int32_t** array, int * N) {
    FILE * source;
    if (argc == 1) {
        source = stdout;
    } else if (argc == 2) {
        source = fopen(argv[1], "rt");
        if (source == nullptr) {
            printf("Failed to open file %s\n", argv[1]);
            return -1;
        }
    } else {
        printf("Wrong number of arguments! Required 1 filename or 0 (read from stdin)\n");
        return -1;
    }

    int n;
    fscanf(source, "%d", &n);
    if (n <= 0 || n > N_MAX) {
        printf("N out of bounds\n");
        return -1;
    }

    int32_t * arr = new int32_t [n];
    for (int i = 0; i < n; i++) {
        if (fscanf(source, "%ld", &arr[i]) != 1) {
            printf("Failed to read array element %d\n", i);
            delete[] arr;
            return -1;
        }
    }

    *array = arr;
    *N = n;
    return 0;
}

int main(int argc, char * argv[]) {
    int32_t * array;
    int N;

    if (get_array(argc, argv, &array, &N))
        return -1;



    return 0;
}
