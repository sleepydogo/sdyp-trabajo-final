#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void merge(int* arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    int* L = (int*)malloc(n1 * sizeof(int));
    int* R = (int*)malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
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

void mergeSortIterative(int* arr, int n) {
    for (int curr_size = 1; curr_size <= n - 1; curr_size = 2 * curr_size) {
        for (int left_start = 0; left_start < n - 1; left_start += 2 * curr_size) {
            int mid = left_start + curr_size - 1;
            int right_end = ((left_start + 2 * curr_size - 1) < (n - 1)) ? (left_start + 2 * curr_size - 1) : (n - 1);

            merge(arr, left_start, mid, right_end);
        }
    }
}

int main(int argc, char* argv[]) {
    int rank, size, n;
    int* array = NULL;
    int* local_array = NULL;
    int local_n;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        n = 16;  // Número de elementos en el array (puedes cambiar esto o leer desde argv)
        array = (int*)malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            array[i] = rand() % 100;  // Llena el array con valores aleatorios
        }

        printf("Array original: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    local_n = n / size;
    local_array = (int*)malloc(local_n * sizeof(int));

    MPI_Scatter(array, local_n, MPI_INT, local_array, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    mergeSortIterative(local_array, local_n);

    MPI_Gather(local_array, local_n, MPI_INT, array, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Hacer el merge final en el proceso root
        int curr_size = local_n;
        while (curr_size < n) {
            for (int i = 0; i < n; i += 2 * curr_size) {
                int mid = i + curr_size - 1;
                int right_end = ((i + 2 * curr_size - 1) < (n - 1)) ? (i + 2 * curr_size - 1) : (n - 1);
                if (mid < right_end) {
                    merge(array, i, mid, right_end);
                }
            }
            curr_size *= 2;
        }

        printf("Array ordenado: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");

        free(array);
    }

    free(local_array);
    MPI_Finalize();

    return 0;
}
