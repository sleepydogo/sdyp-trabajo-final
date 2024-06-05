// Merge sort for the array with pthreads, with variable number of threads

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#define MAX 1000000
#define MAX_THREAD 8

int a[MAX];
int part = 0;

void merge(int low, int mid, int high)
{
    int *left = (int *)malloc((mid - low + 1) * sizeof(int));
    int *right = (int *)malloc((high - mid) * sizeof(int));

    int n1 = mid - low + 1, n2 = high - mid, i, j;

    for (i = 0; i < n1; i++)
        left[i] = a[i + low];
    for (i = 0; i < n2; i++)
        right[i] = a[i + mid + 1];

    int k = low;
    i = j = 0;

    // MERGE EN SI
    while (i < n1 && j < n2)
    {
        if (left[i] <= right[j])
            a[k++] = left[i++];
        else
            a[k++] = right[j++];
    }

    // COPIA DE LOS ELEMENTOS RESTANTES
    while (i < n1)
    {
        a[k++] = left[i++];
    }

    while (j < n2)
    {
        a[k++] = right[j++];
    }
    free(left);
    free(right);
}

void merge_sort(int low, int high)
{
    int mid = low + (high - low) / 2;
    if (low < high)
    {
        merge_sort(low, mid);
        merge_sort(mid + 1, high);
        merge(low, mid, high);
    }
}

void *merge_sort_p(void *arg)
{
    int thread_part = part++; // Quien es el thread actual

    int low = thread_part * (MAX / MAX_THREAD);
    int high = (thread_part + 1) * (MAX / MAX_THREAD) - 1;
    // Rango de operacion al principio

    int mid = low + (high - low) / 2; // Se para en el medio de su parte
    if (low < high)
    {
        merge_sort(low, mid);
        merge_sort(mid + 1, high);
        merge(low, mid, high);
    }
}

int main()
{
    int i;
    struct timeval start, end;

    for (i = 0; i < MAX; i++)
        a[i] = rand() % 100;

    pthread_t threads[MAX_THREAD];

    gettimeofday(&start, NULL);

    for (i = 0; i < MAX_THREAD; i++)
        pthread_create(&threads[i], NULL, merge_sort_p, (void *)NULL);

    for (i = 0; i < MAX_THREAD; i++)
        pthread_join(threads[i], NULL);

    merge(0, (MAX / 2 - 1) / 2, MAX / 2 - 1);
    merge(MAX / 2, MAX / 2 + (MAX - 1 - MAX / 2) / 2, MAX - 1);
    merge(0, (MAX - 1) / 2, MAX - 1);

    gettimeofday(&end, NULL);

    // print sorted array
    for (i = 0; i < MAX; i++)
        printf("%d ", a[i]);

    printf("Time taken: %ld microseconds\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));

    return 0;
}
