#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
// #define threads 8
// #define array_size(1 << 10) // Tamaño del array: 2,097,152

#define ARRAY_SIZE_1 (1 << 24)
#define ARRAY_SIZE_2 (1 << 25)
#define ARRAY_SIZE_3 (1 << 26)
#define ARRAY_SIZE_4 (1 << 27)

int *arr; // Declaración del array como puntero

typedef struct
{
    int thread_id;
} ThreadArgs;

int num_threads = 0;
int array_size_choice = 0;
int array_size= 0;
struct timespec start, end;
double time_used;

void merge(int arr[], int l, int m, int r);
void *threadedMergeSort(void *args);
void mergeSort(int arr[], int left, int right);
int min(int x, int y) { return (x < y) ? x : y; }
void printArray(int A[], int size);
void fillArrayWithRandomNumbers(int A[], int size);

void mergeSortParallel()
{
    pthread_t threads[num_threads];
    ThreadArgs args[num_threads];
    int size = array_size / num_threads;

    for (int i = 0; i < num_threads; i++)
    {
        args[i].thread_id = i;
        pthread_create(&threads[i], NULL, threadedMergeSort, (void *)&args[i]);
    }
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    time_used = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("El algoritmo tardó %f segundos en ejecutarse.\n", time_used);

    for (int size = array_size/ num_threads; size < array_size; size *= 2)
    {
        for (int left_start = 0; left_start < array_size; left_start += 2 * size)
        {
            int mid = min(left_start + size - 1, array_size- 1);
            int right_end = min(left_start + 2 * size - 1, array_size- 1);
            merge(arr, left_start, mid, right_end);
        }
    }
}

void *threadedMergeSort(void *args)
{
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    int thread_id = threadArgs->thread_id;
    int left = thread_id * (array_size/ num_threads);
    int right = (thread_id == num_threads - 1) ? (array_size- 1) : ((thread_id + 1) * (array_size/ num_threads) - 1);

    mergeSort(arr, left, right);
    pthread_exit(NULL);
}

void mergeSort(int arr[], int left, int right)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

void printArray(int A[], int size)
{
    for (int i = 0; i < size; i++)
        printf("%d ", A[i]);
    printf("\n");
}

void fillArrayWithRandomNumbers(int A[], int size)
{
    for (int i = 0; i < size; i++)
    {
        A[i] = rand() % 100; // Números aleatorios entre 0 y 99
    }
}

void print_usage(const char *prog_name) {
    printf("Uso: %s -threads N -arraysize N\n", prog_name);
    printf("Valores válidos para <threads>: 8, 16, 32, 64\n");
    printf("Valores válidos para <arraysize>: 1 (2²⁴), 2 (2²⁵), 3 (2²⁶), 4 (2²⁷)\n");
}

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "threads:arraysize:")) != -1) {
        switch (opt) {
            case 't':
                num_threads = atoi(optarg);
                break;
            case 'a':
                array_size_choice = atoi(optarg);
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    if (num_threads == 0 || array_size_choice == 0) {
        print_usage(argv[0]);
        return 1;
    }

    // Verificar el valor de threads
    if (num_threads != 8 && num_threads != 16 && num_threads != 32 && num_threads != 64) {
        printf("Error: Valor de threads no válido. Debe ser uno de los siguientes: 8, 16, 32, 64\n");
        return 1;
    }

    // Verificar el valor de array_sizey asignar el tamaño correspondiente
    switch (array_size_choice) {
        case 1:
            array_size= ARRAY_SIZE_1;
            break;
        case 2:
            array_size= ARRAY_SIZE_2;
            break;
        case 3:
            array_size= ARRAY_SIZE_3;
            break;
        case 4:
            array_size= ARRAY_SIZE_4;
            break;
        default:
            printf("Error: Valor de array_sizeno válido. Debe ser uno de los siguientes: 1, 2, 3, 4\n");
            return 1;
    }

    // Imprimir los valores recibidos y el tamaño del array
    printf("Número de threads: %d\n", num_threads);
    printf("Tamaño del array: %d\n", array_size);

   // Inicializar el generador de números aleatorios
    srand(time(NULL));

    // Asignar memoria dinámicamente para el array
    arr = (int *)malloc(array_size* sizeof(int));
    if (arr == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    fillArrayWithRandomNumbers(arr, array_size);

    printf("Given array is \n");
    // printArray(arr, ARRAY_SIZE);

    mergeSortParallel();

    printf("\nSorted array is \n");
    printArray(arr, array_size);

    // Liberar la memoria asignada
    free(arr);

    return 0;
}
