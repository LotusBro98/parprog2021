#include <omp.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <cmath>
#include <ctime>

/* Доп. условия: Модифицировать исходный массив, не создавая новых (ограничение по памяти) */

/* Макс. количество элементов */
#define N_MAX 10000

/* Оптимизация: минимальное кол-во элементов в блоке, для которого целесообразно создавать новую задачу */
#define N_TASK_MIN 100

/* Получить массив из файла или с клавиатуры в зависимости от аргументов запуска */
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
        fclose(source);
        return -1;
    }

    int32_t * arr = new int32_t [n];
    for (int i = 0; i < n; i++) {
        if (fscanf(source, "%ld", &arr[i]) != 1) {
            printf("Failed to read array element %d\n", i);
            delete[] arr;
            fclose(source);
            return -1;
        }
    }

    *array = arr;
    *N = n;
    fclose(source);
    return 0;
}

void swap(int32_t * a, int32_t * b) {
    int32_t tmp = *a;
    *a = *b;
    *b = tmp;
}

void sort_subset(int32_t * start, int32_t * end);

/* Обертка для оптимизации вырожденных случаев */
inline void sort_subset_task(int32_t * start, int32_t * end) {
    if (end - start <= 1) {
        return;
    } else if (end - start == 2) {
        if (*start > *(end-1)) {
            swap(start, end - 1);
        }
    } else if (end - start < N_TASK_MIN) {
        sort_subset(start, end);
    } else {
        /* Создаем новую задачу, которая может быть обработана другими потоками */
        #pragma omp task default(none) firstprivate(start, end)
        sort_subset(start, end);
    }
}

int32_t * array;

/* Рекурсивная функция быстрой сортировки с обходом Хоара */
void sort_subset(int32_t * start, int32_t * end)
{
    while (end - start > 1)
    {
        /* Оптимизация вырожденного случая */
        if (end - start == 2) {
            if (*start > *(end-1))
                swap(start, end - 1);
            break;
        }

        /* Алгоритм:
        /* 1) Выбираем опорный элемент
         * 2) Делим массив на две части: < опорного и >= опорного, при необходимости переставляя некоторые элементы
         * 3) Проделываем такую же процедуру для обеих частей */

        /* Выбираем опорный элемент
         * Для сведения к нулю вероятности возникновения или специального подбора худшего случая,
         * когда в одной из частей оказывается лишь один элемент и сложность будет O(n^2),
         * выбираем опорный элемент случайным образом */
        int ref_idx = rand() % (end - start);
        int32_t * ref = start + ref_idx;
        int32_t ref_val = *ref;

        /* Будем постепенно увеличивать области, в которых выполнены условия:
         * в левой области все элементы < опорного, в правой все элементы >= опорного
         * Сначала области пустые
         * left, right - указатель на первый элемент, на котором условие не выполняется */
        int32_t * left = start;
        int32_t * right = end - 1;

        while (true) {
            /* Просматриваем элементы с обоих концов, когда на одном конце находим неподходяций,
             * двигаем другой конец, ищем неподходящий в другой части, и меняем их местами.
             * Так как условия взаимоисключающие, то теперь элементы подходящие и можно продвинуть границы дальше
             * Пока границы областей, в которых выполняется условие, не пересекутся */
            while (left < right && *left < ref_val) left++;
            while (left < right && *right >= ref_val) right--;
            if (left >= right)
                break;
            swap(left, right);
        }
        int32_t * border = left;

        /* Теперь проделываем ту же процедуру с левой и правой частью
         * В худшем случае может быть совершено O(n) рекурсивных вызовов, что приведет к переполнению стека
         * Чтобы избежать этого, будем бОльшую часть обрабатывать в цикле в той же функции */
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

    srand(time(NULL));

    /* Получаем массив */
    if (get_array(argc, argv, &array, &N))
        return -1;

    /* Сортируем массив */
    #pragma omp parallel default(none) shared(array, N)
    #pragma omp single
        sort_subset(array, array + N);

    /* Выводим результат */
    for (int i = 0; i < N; ++i) {
        printf("%d\n", array[i]);
    }

    delete[] array;

    return 0;
}
