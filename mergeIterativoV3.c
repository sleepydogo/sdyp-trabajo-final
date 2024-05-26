#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#define ARRAY_SIZE_1 (1 << 15)
#define ARRAY_SIZE_2 (1 << 25)
#define ARRAY_SIZE_3 (1 << 26)
#define ARRAY_SIZE_4 (1 << 27)

sem_t semaphore;
int finished_sorting_threads = 0;
int finished_comparing_threads = 0;

int *arr; 
int *arr2;

typedef struct {
    int thread_id;
    int *arr_part_sorting;       // Puntero a la sección del arreglo a ordenar
    int part_size_sorting;       // Tamaño de la sección del arreglo a ordenar
    int *arr_part_comparing_1;   // Puntero a la sección del arreglo 1 a comparar
    int *arr_part_comparing_2;   // Puntero a la sección del arreglo 2 a comparar
    int part_size_comparing;     // Tamaño de la sección del arreglo a comparar
} ThreadArgs;

int num_threads = 0;
int array_size_choice = 0;
int array_size= 0;

void* mergeSortIterative(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    int thread_id = args->thread_id;
    int *arr_part_sorting = args->arr_part_sorting;
    int part_size_sorting = args->part_size_sorting;
    int *arr_part_comparing_1 = args->arr_part_comparing_1;
    int *arr_part_comparing_2 = args->arr_part_comparing_2;
    int part_size_comparing = args->part_size_comparing;

    int *temp = (int *)malloc(part_size_sorting * sizeof(int));
    if (temp == NULL) {
        printf("No se pudo asignar memoria temporal para el thread %d.\n", thread_id);
        pthread_exit(NULL);
    }

    int i, j, k, lb1, lb2, ub1, ub2, size, equals;
    size = 1;
    while (size < part_size_sorting) {
        lb1 = 0;
        k = 0;

        while (lb1 + size < part_size_sorting) {
            lb2 = lb1 + size;
            ub1 = lb2 - 1;
            if (ub1 + size < part_size_sorting)
                ub2 = ub1 + size;
            else
                ub2 = part_size_sorting - 1;

            i = lb1;
            j = lb2;

            while (i <= ub1 && j <= ub2) {
                if (arr_part_sorting[i] < arr_part_sorting[j])
                    temp[k++] = arr_part_sorting[i++];
                else
                    temp[k++] = arr_part_sorting[j++];
            }

            while (i <= ub1) temp[k++] = arr_part_sorting[i++];
            while (j <= ub2) temp[k++] = arr_part_sorting[j++];

            lb1 = ub2 + 1;
        }

        for (i = 0; i < k; i++) arr_part_sorting[i] = temp[i];

        size = size * 2;

        // printArray(arr_part_sorting, part_size); 
    }

    free(temp);

    sem_wait(&semaphore);
    finished_sorting_threads++;
    sem_post(&semaphore);

    while (finished_sorting_threads < num_threads) {
        // No hacer nada, solo esperar
    }
    equals = 0;
    for (i = 0; i < part_size_comparing; i++) {
        if (arr_part_comparing_1[i] != arr_part_comparing_2[i]) {
            equals = 1;
            break;
        }
    }

    sem_wait(&semaphore);
    finished_comparing_threads += equals; 
    sem_post(&semaphore);

    pthread_exit(NULL);
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
    printf("Valores válidos para <threads>: 8, 16, 32, 64 \n");
    printf("Valores válidos para <arraysize>: 24, 25, 26, 27 \n");
}

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "t:a:")) != -1) {
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
        case 24:
            array_size= ARRAY_SIZE_1;
            break;
        case 25:
            array_size= ARRAY_SIZE_2;
            break;
        case 26:
            array_size= ARRAY_SIZE_3;
            break;
        case 27:
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
    arr2 = (int *)malloc(array_size* sizeof(int));
    
    if ((arr == NULL) || (arr2 == NULL))
    {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Creamos las estructuras para calcular el tiempo de ejecucion 
    struct timespec start, end;
    double time_used;

    // Llenamos el arreglo de numeros aleatorios
    fillArrayWithRandomNumbers(arr, array_size);
    fillArrayWithRandomNumbers(arr2, array_size);

    printf("Given arrays are \n");
    // printArray(arr, ARRAY_SIZE);
    // printArray(arr2, ARRAY_SIZE);

    // declaramos los hilos
    pthread_t threads[num_threads];
    
    // declaramos el arreglo de argumentos para los hilos
    ThreadArgs args[num_threads];

    int i;
    int part_size_sorting = array_size / (num_threads / 2);
    int part_size_comparing = array_size / num_threads;
    sem_init(&semaphore, 0, 1);

    // Crear los threads y pasar los argumentos
    for (i = 0; i < num_threads; i++) {
        args[i].thread_id = i;
        if (i % 2 == 0) {
            args[i].arr_part_sorting = arr + part_size_sorting * i;  // Puntero al inicio de la sección del arreglo 1
        } else {
            args[i].arr_part_sorting = arr2 + part_size_sorting * i;  // Puntero al inicio de la sección del arreglo 2
        }
        args[i].part_size_sorting = part_size_sorting;
        args[i].arr_part_comparing_1 = arr + part_size_comparing * i;  // Puntero al inicio de la sección del arreglo
        args[i].arr_part_comparing_2 = arr2 + part_size_comparing * i;  // Puntero al inicio de la sección del arreglo
        args[i].part_size_comparing = part_size_comparing;

        pthread_create(&threads[i], NULL, mergeSortIterative, (void *)&args[i]);
    }

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    time_used = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("El algoritmo tardó %f segundos en ejecutarse.\n", time_used);


    //printf("\nSorted array is \n");
    //printArray(arr, array_size);

    sem_destroy(&semaphore);
    // Liberar la memoria asignada
    free(arr);
    free(arr2);

    return 0;
}
