#include <omp.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <cmath>
#include <ctime>

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

struct timespec start;

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double)(ts.tv_sec - start.tv_sec) + (double)(ts.tv_nsec - start.tv_nsec) / 1.0e9;
}

int32_t * array;

void swap(int32_t * a, int32_t * b) {
    int32_t tmp = *a;
    *a = *b;
    *b = tmp;
}

void sort_subset(int32_t * start, int32_t * end);

inline void sort_subset_task(int32_t * start, int32_t * end) {
    if (end - start <= 1) {
        return;
    } else if (end - start == 2) {
        if (*start > *(end-1)) {
            swap(start, end - 1);
        }
    } else {
        #pragma omp task default(none) firstprivate(start, end)
        sort_subset(start, end);
    }
}

void sort_subset(int32_t * start, int32_t * end)
{
//    printf("%d\n", end - start);
    while (end - start > 1)
    {
        int32_t * ref = start + (rand() % (end - start));
        int32_t ref_val = *ref;

        int32_t * left = start;
        int32_t * right = end - 1;

        while (true) {
            while (left < right && *left < ref_val) left++;
            while (left < right && *right >= ref_val) right--;
            if (left >= right)
                break;
            swap(left, right);
        }
        int32_t * border = left;

        if (border - start < end - border) {
            sort_subset_task(start, border);
            start = border;
        } else {
            sort_subset_task(border, end);
            end = border;
        }
    }
}

int main(int argc, char * argv[]) {
    int N;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    srand(time(NULL));

    if (get_array(argc, argv, &array, &N))
        return -1;

    double start_clk = get_time();

    for (int i = 0; i < 1; i++) {

    #pragma omp parallel default(none) shared(array, N)
    #pragma omp single nowait
        sort_subset(array, array + N);
    }

    double end_clk = get_time();

    printf("%lf\n", (end_clk - start_clk) / 1);

    for (int i = 0; i < N; ++i) {
        if (array[i] != i + 1)
            printf("Error\n");
//        printf("%d\n", array[i]);
    }

    return 0;
}
