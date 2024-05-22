/*
    Changelog: 
        - Tom (22/05): Agrego funciones para generar arreglos de 2^10 de enteros desordenados.

*/


#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define ARRAY_SIZE (1 << 10) // 2^10 = 1024

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

        pthread_t threads[2];
        ThreadArgs args1 = {arr, left, mid};
        ThreadArgs args2 = {arr, mid + 1, right};
        
        pthread_create(&threads[0], NULL, parallel_mergesort, &args1);
        pthread_create(&threads[1], NULL, parallel_mergesort, &args2);
        
        pthread_join(threads[0], NULL);
        pthread_join(threads[1], NULL);

        merge(arr, left, mid, right);
    }

    return NULL;
}

int main() {
    // Definimos la variable que almacena el tamanio del arreglo
    size_t array_size;

    // Generamos un puntero al inicio del arreglo y pasamos la posicion de memoria de la variable que almacena el tamanio
    int *array = generate_array(&array_size);
    
    // Si no hay memoria no ejecutamos 
    if (array == NULL) {
        printf("No se pudo asignar la suficiente memoria.\n");
        return 1; 
    }

    for (size_t i = 0; i < array_size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");


    printf("--> array_size: %zu elementos \n", array_size);
    
    // Enviamos el puntero al arreglo
    ThreadArgs args = {array, 0, array_size - 1};
    pthread_t main_thread;
    
    pthread_create(&main_thread, NULL, parallel_mergesort, &args);
    pthread_join(main_thread, NULL);

    printf("Sorted array: \n");
    for (int i = 0; i < array_size; i++)
        printf("%d ", array[i]);
    printf("\n");
    
    free(array);

    return 0;
}
