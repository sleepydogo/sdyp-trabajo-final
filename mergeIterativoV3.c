#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define ARRAY_SIZE_1 (1 << 24)
#define ARRAY_SIZE_2 (1 << 25)
#define ARRAY_SIZE_3 (1 << 26)
#define ARRAY_SIZE_4 (1 << 27)

int *arr; 

typedef struct {
    int thread_id;
    int *arr_part;   // Puntero a la sección del arreglo
    int part_size;   // Tamaño de la sección del arreglo
} ThreadArgs;

int num_threads = 0;
int array_size_choice = 0;
int array_size= 0;


void* mergeSortIterative(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    int thread_id = args->thread_id;
    int *arr_part = args->arr_part;
    int part_size = args->part_size;

    int *temp = (int *)malloc(part_size * sizeof(int));
    if (temp == NULL) {
        printf("No se pudo asignar memoria temporal para el thread %d.\n", thread_id);
        pthread_exit(NULL);
    }

    int i, j, k, lb1, lb2, ub1, ub2, size;
    size = 1;
    while (size < part_size) {
        lb1 = 0;
        k = 0;

        while (lb1 + size < part_size) {
            lb2 = lb1 + size;
            ub1 = lb2 - 1;
            if (ub1 + size < part_size)
                ub2 = ub1 + size;
            else
                ub2 = part_size - 1;

            i = lb1;
            j = lb2;

            while (i <= ub1 && j <= ub2) {
                if (arr_part[i] < arr_part[j])
                    temp[k++] = arr_part[i++];
                else
                    temp[k++] = arr_part[j++];
            }

            while (i <= ub1) temp[k++] = arr_part[i++];
            while (j <= ub2) temp[k++] = arr_part[j++];

            lb1 = ub2 + 1;
        }

        for (i = 0; i < k; i++) arr_part[i] = temp[i];

        size = size * 2;

        // printArray(arr_part, part_size); 
    }

    free(temp);
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
    if (arr == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Creamos las estructuras para calcular el tiempo de ejecucion 
    struct timespec start, end;
    double time_used;

    // Llenamos el arreglo de numeros aleatorios
    fillArrayWithRandomNumbers(arr, array_size);

    printf("Given array is \n");
    // printArray(arr, ARRAY_SIZE);

    // declaramos los hilos
    pthread_t threads[num_threads];
    
    // declaramos el arreglo de argumentos para los hilos
    ThreadArgs args[num_threads];

    int i;
    int part_size = array_size / num_threads;
    // Crear los threads y pasar los argumentos
    for (i = 0; i < num_threads; i++) {
        args[i].thread_id = i;
        args[i].arr_part = arr + part_size * i;  // Puntero al inicio de la sección del arreglo
        args[i].part_size = part_size;

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

    // Liberar la memoria asignada
    free(arr);

    return 0;
}
