#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#define ARRAY_SIZE (1 << 24)
#define NUM_THREADS 4
sem_t semaphore;
int finished_sorting_threads = 0;
int finished_comparing_threads = 0;

int *arr, *arr2;

typedef struct
{
    int thread_id; // Tamaño de la sección del arreglo a ordenar
    int *arr1;     // Puntero a la sección del arreglo 1 a comparar
    int *arr2;     // Puntero a la sección del arreglo 2 a comparar
} ThreadArgs;

int num_threads = NUM_THREADS;
int array_size = ARRAY_SIZE;

pthread_barrier_t barreras[NUM_THREADS];
pthread_barrier_t barreras2[NUM_THREADS];
pthread_barrier_t barreraFull;
void printArray(int A[], int size);

void merge(int *array, int start, int mid, int end)
{
    int n1 = mid - start + 1;
    int n2 = end - mid;
    int *left = (int *)malloc(n1 * sizeof(int));
    int *right = (int *)malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++)
        left[i] = array[start + i];
    for (int i = 0; i < n2; i++)
        right[i] = array[mid + 1 + i];

    int i = 0, j = 0, k = start;
    while (i < n1 && j < n2)
    {
        if (left[i] <= right[j])
        {
            array[k++] = left[i++];
        }
        else
        {
            array[k++] = right[j++];
        }
    }

    while (i < n1)
    {
        array[k++] = left[i++];
    }

    while (j < n2)
    {
        array[k++] = right[j++];
    }

    free(left);
    free(right);
}

void *mergeSortIterative(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    int thread_id = args->thread_id;
    int *array1 = args->arr1;
    int *array2 = args->arr2;
    int *sorting_array;
    int part_size_sorting = ((array_size * 2) / (num_threads)); // Parte que va a ordenar

    int posStart = (thread_id / 2) * part_size_sorting;
    int posEnd = posStart + part_size_sorting - 1;

    if (thread_id % 2 == 0)
    {
        sorting_array = array1;
    }
    else
    {
        sorting_array = array2;
    }

    int *temp = (int *)malloc(part_size_sorting * sizeof(int)); // Reservo temporal del tamanio de mi parte a ORDENAR
    if (temp == NULL)
    {
        printf("No se pudo asignar memoria temporal para el thread %d.\n", thread_id);
        pthread_exit(NULL);
    }

    int i, j, k, lb1, lb2, ub1, ub2, size, equals;
    size = 1;
    while (size < part_size_sorting) // Mientras el tamanio que ordeno es menor al tamanio total de la parte
    {
        lb1 = posStart; // Inicio de ordenar la primera parte
        k = 0;          // k es el indice del array temporal, como este es local a cada proceso empieza siempre en 0

        while (lb1 + size < part_size_sorting + posStart)
        {
            lb2 = lb1 + size;                              // Inicio de la segunda fraccion a ordenar
            ub1 = lb2 - 1;                                 // tope de la primera parte
            if (ub1 + size < part_size_sorting + posStart) // si el final de la primera parte mas el tamanio a ordenar actual es menor, ese es el tope de la segunda parte
                ub2 = ub1 + size;
            else // sino, el tope superior de la segunda es el maximo punto que podemos tomar
                ub2 = part_size_sorting + posStart - 1;

            i = lb1; // i empieza en el principio de la primera parte
            j = lb2; // j empieza en el principio de la segunda parte

            while (i <= ub1 && j <= ub2) // mientras que los indices esten dentro de los rangos a ordenar
            {
                if (sorting_array[i] < sorting_array[j]) // si el actual de la parte 1 es mas chico que el actual de la parte 2
                    temp[k++] = sorting_array[i++];      // va en el array final el de la parte 1
                else
                    temp[k++] = sorting_array[j++]; // sino va el de la parte dos
            }

            while (i <= ub1)                    // mientras que el indice de la primera parte no llegue a su tope
                temp[k++] = sorting_array[i++]; // copio en el temporal lo que queda de esa parte
            while (j <= ub2)                    // lo mismo pasa con la segunda parte
                temp[k++] = sorting_array[j++];

            lb1 = ub2 + 1;
        }

        for (i = 0; i < k; i++)
            sorting_array[i + posStart] = temp[i]; // actualizo lo que ordene al original

        size = size * 2;
    }

    // dichoso de gran fortuna el retonio

    if (temp != NULL)
    {
        free(temp);
        temp = NULL;
    }

    if (thread_id % 2 == 0)
    {
        // Es un thread par
        // barrra 32

        pthread_barrier_wait(&barreras[NUM_THREADS / 2]);

        for (i = 4; i <= num_threads; i *= 2)
        {
            int posMid = posEnd;
            posEnd = posStart + part_size_sorting * (i / 2) - 1;

            if ((thread_id % i == 0) || (thread_id == 0))
            {
                // rangoQueProcesas = rangoQueProcesas*2;

                // Mergeas el rango que te toca procesar (Las dos mitades)
                merge(sorting_array, posStart, posMid, posEnd);

                pthread_barrier_wait(&barreras[NUM_THREADS / i]);
                // barreraX  --> Barrera[barr] la esperan 64/i procesos, por ejemplo cuando i=4, la barrera es de 16, cuando i es de 8 la barrera es de 8...
            }
        }
    }

    else
    { // Si somos impares

        pthread_barrier_wait(&barreras2[NUM_THREADS / 2]);

        for (i = 4; i <= num_threads; i *= 2)
        {
            int posMid = posEnd;
            posEnd = posStart + part_size_sorting * (i / 2) - 1;

            if (((thread_id - 1) % i == 0) || (thread_id == 1))
            {
                // rangoQueProcesas = rangoQueProcesas*2;

                // Mergeas el rango que te toca procesar (Las dos mitades)
                merge(sorting_array, posStart, posMid, posEnd);

                pthread_barrier_wait(&barreras2[NUM_THREADS / i]);
                // barreraX  --> Barrera[barr] la esperan 64/i procesos, por ejemplo cuando i=4, la barrera es de 16, cuando i es de 8 la barrera es de 8...
            }
        }
    }
    // EFECTIVAMENTE EL NENE SIGUE CHETO

    // barrera de P
    pthread_barrier_wait(&barreraFull);
    int iguales = 1;

    part_size_sorting = ((array_size) / (num_threads)); // Parte que va a ordenar

    posStart = (thread_id)*part_size_sorting;
    posEnd = posStart + part_size_sorting - 1;

    for (i = posStart; i <= posEnd; i++)
    {
        if (array1[i] != array2[i])
        {
            iguales = 0;
            break;
        }
    }
    sem_wait(&semaphore);
    finished_comparing_threads += iguales;
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
        A[i] = (rand() % 1000) + 1; // Números aleatorios entre 0 y 99
    }
}

int main(int argc, char *argv[])
{

    // Imprimir los valores recibidos y el tamaño del array
    printf("Número de threads: %d\n", num_threads);
    printf("Tamaño del array: %d\n", array_size);

    // Inicializar el generador de números aleatorios
    srand(time(NULL));

    // Asignar memoria dinámicamente para el array
    arr = (int *)malloc(array_size * sizeof(int));
    arr2 = (int *)malloc(array_size * sizeof(int));

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
    // fillArrayWithRandomNumbers(arr2, array_size);
    memcpy(arr2, arr, array_size * sizeof(int));
    int aux = arr2[16777215];
    arr2[16777215] = arr2[32];
    arr2[32] = aux;

    // declaramos los hilos
    pthread_t threads[num_threads];

    // declaramos el arreglo de argumentos para los hilos
    ThreadArgs args[num_threads];

    int i;
    sem_init(&semaphore, 0, 1);
    int j;
    for (j = 1; j <= NUM_THREADS; j *= 2)
    {
        pthread_barrier_init(&barreras[j], NULL, j);
        pthread_barrier_init(&barreras2[j], NULL, j);
    }
    pthread_barrier_init(&barreraFull, NULL, NUM_THREADS);

    // Crear los threads y pasar los argumentos
    for (i = 0; i < num_threads; i++)
    {
        args[i].thread_id = i;
        args[i].arr1 = arr;
        args[i].arr2 = arr2;

        if (pthread_create(&threads[i], NULL, mergeSortIterative, (void *)&args[i]) != 0)
        {
            printf("Error creando el hilo %d.\n", i);
            return 1;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    time_used = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("El algoritmo tardó %f segundos en ejecutarse.\n", time_used);

    if (finished_comparing_threads == NUM_THREADS)
    {
        printf("Los arrays son iguales");
    }
    else
    {
        printf("Los arrays son distintos");
    }

    sem_destroy(&semaphore);
    for (j = 1; j <= NUM_THREADS; j *= 2)
    {
        pthread_barrier_destroy(&barreras[j]);
        pthread_barrier_destroy(&barreras2[j]);
        pthread_barrier_destroy(&barreraFull);
    }
    // Liberar la memoria asignada

    if (arr != NULL)
    {
        free(arr);
        arr = NULL;
    }
    if (arr2 != NULL)
    {
        free(arr2);
        arr = NULL;
    }

    return 0;
}
