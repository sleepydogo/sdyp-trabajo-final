/*
Changelog: 
    - Tom (22/05): 
        * Agrego funciones para generar arreglos de 2^10 de enteros desordenados.
        * Agrego mediciones de tiempo
        * Agrego variables para ejecucion en entorno local y para ejecutar en el cluster.

*/


#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

// Local
#define ARRAY_SIZE (1 << 12) // 2^10 = 1024
#define NUM_THREADS 2

// Cluster
const int NUM_THREADS_CLUSTER[] = {8, 16, 32, 64};
const int ARRAY_SIZE_CLUSTER[] = {(1 << 24), (1 << 25), (1 << 26), (1 << 27)};

typedef struct {
    int *arr;
    int left;
    int right;
} ThreadArgs;

int* generate_array(size_t* size) {
    int *array = (int *)malloc(ARRAY_SIZE * sizeof(int));
    
    if (array == NULL) {
        printf("No se pudo asignar memoria suficiente.\n");
        return NULL;
    }

    // Inicializar el generador de números aleatorios con la semilla basada en el tiempo actual
    srand(time(NULL));

    // Rellenar el array con números aleatorios entre 1 y 100
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = (rand() % 100) + 1; // Genera un número entre 1 y 100
    }

    // Asignar el tamaño del array al parámetro size
    *size = ARRAY_SIZE;

    return array;
}

void merge(int *arr, int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    int *L = (int*)malloc(n1 * sizeof(int));
    int *R = (int*)malloc(n2 * sizeof(int));

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

void* parallel_mergesort(void *arg) {
    ThreadArgs *args = (ThreadArgs*)arg;
    int *arr = args->arr;
    int left = args->left;
    int right = args->right;
    
    if (left < right) {
        int mid = left + (right - left) / 2;

        pthread_t threads[NUM_THREADS];
        ThreadArgs thread_args[NUM_THREADS];
        int segment_size = (right - left + 1) / NUM_THREADS;

        // Crear los hilos
        for (int i = 0; i < NUM_THREADS; i++) {
            int segment_left = left + i * segment_size;
            int segment_right = (i == NUM_THREADS - 1) ? right : (segment_left + segment_size - 1);

            thread_args[i].arr = arr;
            thread_args[i].left = segment_left;
            thread_args[i].right = segment_right;

            pthread_create(&threads[i], NULL, parallel_mergesort, &thread_args[i]);
        }

        // Esperar a que los hilos terminen
        for (int i = 0; i < NUM_THREADS; i++) {
            pthread_join(threads[i], NULL);
        }

        // Realizar merge de los segmentos procesados
        for (int i = 0; i < NUM_THREADS - 1; i++) {
            int segment_left = left + i * segment_size;
            int segment_right = (i == NUM_THREADS - 1) ? right : (segment_left + segment_size - 1);
            int next_segment_right = (i + 1 == NUM_THREADS - 1) ? right : (segment_left + segment_size * 2 - 1);

            merge(arr, segment_left, segment_right, next_segment_right);
        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {

    // Se tiene que ingresar un parametro que especifica donde se esta ejecutando el programa

    if (argc != 2) {
        // Si el usuario no proporciona exactamente un argumento, mostrar un mensaje de error y salir
        printf("Uso: %s <parametro>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "local") == 0) {
        
        
    } else if (strcmp(argv[1], "cluster") == 0) {


    } else {
        printf("Uso del programa: \n\t ./programa local - para entornos locales \n\t ./programa cluster - para ejecutar en el cluster III-LIDI \n\t ./programa help - imprime este mensaje");
        return 1;
    }

    size_t array_size; // almacena la cantidad de elementos del arreglo
    struct timespec start, end; // estructura para medir tiempos

    // Generamos un puntero al inicio del arreglo y pasamos la posicion de memoria de la variable que almacena el tamanio
    int *array = generate_array(&array_size);
    
    // Si no hay memoria no ejecutamos 
    if (array == NULL) {
        printf("No se pudo asignar la suficiente memoria.\n");
        return 1; 
    }

    // Imprimimos el array desordenado
    for (size_t i = 0; i < array_size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
    

    printf("--> array_size: %zu elementos \n", array_size);
    
    // Enviamos el puntero al arreglo
    ThreadArgs args = {array, 0, array_size - 1};
    pthread_t main_thread;

    // Obtener el tiempo de inicio
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    pthread_create(&main_thread, NULL, parallel_mergesort, &args);
    pthread_join(main_thread, NULL);

    // Obtener el tiempo de fin
    clock_gettime(CLOCK_MONOTONIC, &end);

    printf("Array ordenado: \n");
    for (int i = 0; i < array_size; i++)
        printf("%d ", array[i]);
    printf("\n");

    double elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("El tiempo total de ejecución de los hilos es: %f segundos.\n", elapsed_time);
    
    free(array);

    return 0;
}
