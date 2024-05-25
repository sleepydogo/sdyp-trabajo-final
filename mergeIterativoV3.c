#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 8
#define ARRAY_SIZE (1 << 10) // Tamaño del array: 2,097,152

int *arr; // Declaración del array como puntero

typedef struct
{
    int thread_id;
} ThreadArgs;

void merge(int arr[], int l, int m, int r);
void *threadedMergeSort(void *args);
void mergeSort(int arr[], int left, int right);
int min(int x, int y) { return (x < y) ? x : y; }
void printArray(int A[], int size);
void fillArrayWithRandomNumbers(int A[], int size);

void mergeSortParallel()
{
    pthread_t threads[NUM_THREADS];
    ThreadArgs args[NUM_THREADS];
    int size = ARRAY_SIZE / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++)
    {
        args[i].thread_id = i;
        pthread_create(&threads[i], NULL, threadedMergeSort, (void *)&args[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    for (int size = ARRAY_SIZE / NUM_THREADS; size < ARRAY_SIZE; size *= 2)
    {
        for (int left_start = 0; left_start < ARRAY_SIZE; left_start += 2 * size)
        {
            int mid = min(left_start + size - 1, ARRAY_SIZE - 1);
            int right_end = min(left_start + 2 * size - 1, ARRAY_SIZE - 1);
            merge(arr, left_start, mid, right_end);
        }
    }
}

void *threadedMergeSort(void *args)
{
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    int thread_id = threadArgs->thread_id;
    int left = thread_id * (ARRAY_SIZE / NUM_THREADS);
    int right = (thread_id == NUM_THREADS - 1) ? (ARRAY_SIZE - 1) : ((thread_id + 1) * (ARRAY_SIZE / NUM_THREADS) - 1);

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

int main()
{
    // Inicializar el generador de números aleatorios
    srand(time(NULL));

    // Asignar memoria dinámicamente para el array
    arr = (int *)malloc(ARRAY_SIZE * sizeof(int));
    if (arr == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    fillArrayWithRandomNumbers(arr, ARRAY_SIZE);

    printf("Given array is \n");
    // printArray(arr, ARRAY_SIZE);

    mergeSortParallel();

    printf("\nSorted array is \n");
    printArray(arr, ARRAY_SIZE);

    // Liberar la memoria asignada
    free(arr);

    return 0;
}
