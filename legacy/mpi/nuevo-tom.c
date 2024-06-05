#include <stdio.h>
#include <stdlib.h> // atoi
#include <math.h>   // log2
#include <time.h>   // srand time
#include <mpi.h>
#include <string.h>
#include <time.h>
#include <sys/time.h> // dwalltime
// macros
#define min(a, b) (a < b ? a : b)

// NO EXISTEN VARIABLES COMPARTIDAS EN MPI
int getSize(int argc, char *argv[]);
void p0(int N, int cp, int id);
void p1(int N, int cp, int id);
void combinar(int *V, int left, int medio, int right);
// prototipos comunes
void ordenarVector(int *V, int size);

double dwalltime()
{
    double sec;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    sec = tv.tv_sec + tv.tv_usec / 1000000.0;
    return sec;
}

void merge(int *array1, int *array2, int tam, int *arrayRes)
{

    int i = 0, j = 0, k = 0;
    while (i < tam && j < tam)
    {
        if (array1[i] <= array2[j])
        {
            arrayRes[k++] = array1[i++];
        }
        else
        {
            arrayRes[k++] = array2[j++];
        }
    }

    while (i < tam)
    {
        arrayRes[k++] = array1[i++];
    }

    while (j < tam)
    {
        arrayRes[k++] = array2[j++];
    }
}

// programa principal
int main(int argc, char *argv[])
{

    // siempre primero esto!!
    MPI_Init(&argc, &argv);

    // variables locales
    int id;
    int cantProcesos;
    int N = getSize(argc, argv);

    // obtener id y cant procesos
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &cantProcesos);

    if (id == 0)
        p0(N, cantProcesos, id);
    else
        p1(N, cantProcesos, id);

    // siempre por ultimo esto!!
    MPI_Finalize();

    return 0;
}

// funcion que ejecuta el proceso con id=0
void p0(int N, int cp, int id)
{
    int *Vtot = (int *)malloc(N * sizeof(int));
    int *Vtot2 = (int *)malloc(N * sizeof(int));

    int u;

    // generar numeros
    srand(time(NULL));

    for (int a = 0; a < N; a++)
    {
        Vtot[a] = rand() % 100;
    }
    memcpy(Vtot2, Vtot, (N * sizeof(int)));
    for (u = 0; u < N; u++)
    {
        printf("%d ", Vtot[u]);
    }
    printf("\n\n ");

    for (u = 0; u < N; u++)
    {
        printf("%d ", Vtot2[u]);
    }
    printf("\n\n ");
    // repartir porcion a cada proceso
    const int tamanio = N / cp;
    double t0 = dwalltime();
    int *vLoc = (int *)malloc(tamanio * sizeof(int));
    int *vLoc2 = (int *)malloc(tamanio * sizeof(int));

    // Scatter(sendbuf, sendcount, sendtype, recbuf, reccount, rectype, root, comm)
    MPI_Scatter(Vtot, tamanio, MPI_INT, vLoc, tamanio, MPI_INT, 0, MPI_COMM_WORLD);

    // realizar ordenación de mi porcion asignada
    ordenarVector(vLoc, tamanio);

    int lol, *vNeigh, *vFusion, paso;

    int tamanioActual = tamanio;
    for (paso = 1; paso < cp; paso *= 2)
    {
        // COMO SON BLOQUEANTES RECEPCION Y EMISION, NO NECESITO BARRERAS????
        if (((id % (2 * paso)) == 0) || (id == 0))
        {
            vNeigh = (int *)malloc(tamanioActual * sizeof(int));
            // Aca los que reciben del vecino [vNeigh], recibo de (id + paso)
            MPI_Recv(vNeigh, tamanioActual, MPI_INT, (id + paso), MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            tamanioActual *= 2; // Duplico el tamanio actual para reservar el nuevo espacio donde se va a juntar
            vFusion = (int *)malloc(tamanioActual * sizeof(int));

            // mergeo vNeigh con vLoc en Vfusion.
            merge(vLoc, vNeigh, tamanioActual / 2, vFusion);
            // for (u = 0; u < tamanioActual; u++)
            //{
            //     printf("%d ", vFusion[u]);
            // }
            // printf("\n");
            //  free vLoc, malloc vLoc con doble tamanio, asigno vFusion al nuevo vLoc, free vFusion
            free(vLoc);
            vLoc = NULL;

            vLoc = (int *)malloc(tamanioActual * sizeof(int));
            memcpy(vLoc, vFusion, tamanioActual * sizeof(int));

            free(vNeigh);
            vNeigh = NULL;

            free(vFusion);
            vFusion = NULL;
        }
        // free(vLoc); // Tanto si entras al if como sino siempre queda un vLoc alocado
    }

    // Una vez que salis de aca, como sos el 0 en tu vLoc tenes el ordenado.
    for (u = 0; u < N; u++)
    {
        printf("%d ", vLoc[u]);
    }
    printf("\n\n ");

    // generar numeros
    srand(time(NULL));

    // Scatter(sendbuf, sendcount, sendtype, recbuf, reccount, rectype, root, comm)
    MPI_Scatter(Vtot2, tamanio, MPI_INT, vLoc2, tamanio, MPI_INT, 0, MPI_COMM_WORLD);

    // realizar ordenación de mi porcion asignada
    ordenarVector(vLoc2, tamanio);
    tamanioActual = tamanio;

    for (paso = 1; paso < cp; paso *= 2)
    {
        // COMO SON BLOQUEANTES RECEPCION Y EMISION, NO NECESITO BARRERAS????
        if (((id % (2 * paso)) == 0) || (id == 0))
        {
            vNeigh = (int *)malloc(tamanioActual * sizeof(int));
            // Aca los que reciben del vecino [vNeigh], recibo de (id + paso)
            MPI_Recv(vNeigh, tamanioActual, MPI_INT, (id + paso), MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            tamanioActual *= 2; // Duplico el tamanio actual para reservar el nuevo espacio donde se va a juntar
            vFusion = (int *)malloc(tamanioActual * sizeof(int));

            // mergeo vNeigh con vLoc en Vfusion.
            merge(vLoc2, vNeigh, tamanioActual / 2, vFusion);

            // for (u = 0; u < tamanioActual; u++)
            //{
            //     printf("%d ", vFusion[u]);
            // }
            // printf("\n");
            //  free vLoc, malloc vLoc con doble tamanio, asigno vFusion al nuevo vLoc, free vFusion
            free(vLoc2);
            vLoc2 = NULL;

            vLoc2 = (int *)malloc(tamanioActual * sizeof(int));
            memcpy(vLoc2, vFusion, tamanioActual * sizeof(int));

            free(vNeigh);
            vNeigh = NULL;

            free(vFusion);
            vFusion = NULL;
        }
        // free(vLoc); // Tanto si entras al if como sino siempre queda un vLoc alocado
    }

    // Una vez que salis de aca, como sos el 0 en tu vLoc tenes el ordenado.
    for (u = 0; u < N; u++)
    {
        printf("%d ", vLoc2[u]);
    }
    // liberar recursos

    printf("\n\n ");

    for (u = 1; u < cp; u++)
    {
        MPI_Send(&vLoc[u * tamanio], tamanio, MPI_INT, u, id, MPI_COMM_WORLD);
        MPI_Send(&vLoc2[u * tamanio], tamanio, MPI_INT, u, id, MPI_COMM_WORLD);
    }
    int sumador = 0, totalizador = 0;

    for (int m = 0; m < tamanio; m++)
    {
        if (vLoc2[m] != vLoc[m])
        {
            sumador = 1;
            break;
        }
    }

    for (u = 1; u < cp - 1; u++)
    {
        MPI_Recv(&sumador, sizeof(int), MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        totalizador += sumador;
    }

    if (totalizador == 0)
    {
        printf("todo cheto cumpa");
    }
    else
    {
        printf("cagaste");
    }

    free(Vtot);
    free(vLoc);
}

// procesos secundarios
void p1(int N, int cp, int id)
{

    int *Vtot = (int *)malloc(N * sizeof(int));
    const int tamanio = N / cp;
    double t0 = dwalltime();
    int *vLoc = (int *)malloc(tamanio * sizeof(int));
    // Scatter(sendbuf, sendcount, sendtype, recbuf, reccount, rectype, root, comm)
    MPI_Scatter(Vtot, tamanio, MPI_INT, vLoc, tamanio, MPI_INT, 0, MPI_COMM_WORLD);

    // realizar ordenación de mi porcion asignada
    ordenarVector(vLoc, tamanio);

    int paso;
    int *vNeigh, *vFusion;
    int tamanioActual = tamanio;
    for (paso = 1; paso < cp; paso *= 2)
    {

        // COMO SON BLOQUEANTES RECEPCION Y EMISION, NO NECESITO BARRERAS????
        if (((id % (2 * paso)) == 0) || (id == 0))
        {

            vNeigh = (int *)malloc(tamanioActual * sizeof(int));
            // Aca los que reciben del vecino [vNeigh], recibo de (id + paso)
            MPI_Recv(vNeigh, tamanioActual, MPI_INT, (id + paso), MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            tamanioActual *= 2; // Duplico el tamanio actual para reservar el nuevo espacio donde se va a juntar
            vFusion = (int *)malloc(tamanioActual * sizeof(int));

            // mergeo vNeigh con vLoc en Vfusion.
            merge(vLoc, vNeigh, tamanioActual / 2, vFusion);

            // free vLoc, malloc vLoc con doble tamanio, asigno vFusion al nuevo vLoc, free vFusion
            free(vLoc);
            vLoc = NULL;
            free(vNeigh);
            vNeigh = NULL;
            vLoc = (int *)malloc(tamanioActual * sizeof(int));
            memcpy(vLoc, vFusion, tamanioActual * sizeof(int));
            free(vFusion);
            vFusion = NULL;
        }
        else
        {
            // Los que envian al vecino
            MPI_Send(vLoc, tamanioActual, MPI_INT, (id - paso), id, MPI_COMM_WORLD);
            // Espero que reciba
            free(vLoc);
            break;
            // break
        }

        //  free(vLoc); // Tanto si entras al if como sino siempre queda un vLoc alocado
    }

    MPI_Scatter(Vtot, tamanio, MPI_INT, vLoc, tamanio, MPI_INT, 0, MPI_COMM_WORLD);

    // realizar ordenación de mi porcion asignada
    ordenarVector(vLoc, tamanio);

    for (paso = 1; paso < cp; paso *= 2)
    {

        // COMO SON BLOQUEANTES RECEPCION Y EMISION, NO NECESITO BARRERAS????
        if (((id % (2 * paso)) == 0) || (id == 0))
        {

            vNeigh = (int *)malloc(tamanioActual * sizeof(int));
            // Aca los que reciben del vecino [vNeigh], recibo de (id + paso)
            MPI_Recv(vNeigh, tamanioActual, MPI_INT, (id + paso), MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            tamanioActual *= 2; // Duplico el tamanio actual para reservar el nuevo espacio donde se va a juntar
            vFusion = (int *)malloc(tamanioActual * sizeof(int));

            // mergeo vNeigh con vLoc en Vfusion.
            merge(vLoc, vNeigh, tamanioActual / 2, vFusion);

            // free vLoc, malloc vLoc con doble tamanio, asigno vFusion al nuevo vLoc, free vFusion
            free(vLoc);
            vLoc = NULL;
            free(vNeigh);
            vNeigh = NULL;
            vLoc = (int *)malloc(tamanioActual * sizeof(int));
            memcpy(vLoc, vFusion, tamanioActual * sizeof(int));
            free(vFusion);
            vFusion = NULL;
        }
        else
        {
            // Los que envian al vecino
            MPI_Send(vLoc, tamanioActual, MPI_INT, (id - paso), id, MPI_COMM_WORLD);
            // Espero que reciba
            free(vLoc);
            break;
            // break
        }

        int sumador = 0;
        int *vComparing = (int *)malloc(tamanio * sizeof(int));
        int *vComparing2 = (int *)malloc(tamanio * sizeof(int));

        MPI_Recv(vComparing, tamanio, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(vComparing2, tamanio, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int m = 0; m < tamanio; m++)
        {
            if (vComparing2[m] != vComparing[m])
            {
                sumador = 1;
                break;
            }
        }
        MPI_Send(&sumador, sizeof(int), MPI_INT, 0, id, MPI_COMM_WORLD);
        free(vComparing2);
        free(vComparing);
        free(vLoc); //

        //  free(vLoc); // Tanto si entras al if como sino siempre queda un vLoc alocado
    }
}

// funciones comunes
void ordenarVector(int *V, int size)
{

    // empezar ordenando de a 2, luego de 4, de a 8 y asi...
    int len, left, medio, right;
    int fin = size - 1;

    for (len = 2; len <= size; len *= 2)
    {
        for (left = 0; left < size; left += len)
        {
            medio = left + len / 2 - 1;
            right = min(left + len - 1, fin);

            if (medio >= fin)
                break; // ya ordenado
            combinar(V, left, medio, right);
        }
    }

    // ordenar ultima parte sobrante si N no es potencia de 2
    if (len != 2 * size)
    {
        medio = len / 2 - 1;
        combinar(V, 0, medio, fin);
    }
}

void combinar(int *V, int left, int medio, int right)
{
    int len1 = (medio - left) + 1;
    int len2 = (right - medio);
    int k;

    // arreglos temporales
    int *L = (int *)malloc(len1 * sizeof(int));
    int *R = (int *)malloc(len2 * sizeof(int));

    // copiar
    for (k = 0; k < len1; k++)
        L[k] = V[left + k];
    for (k = 0; k < len2; k++)
        R[k] = V[medio + k + 1];

    // ordenar eligiendo menor entre L y R
    int i = 0, j = 0;

    for (k = left; k <= right; k++)
    {
        if (i >= len1)
            V[k] = R[j++];
        else if (j >= len2)
            V[k] = L[i++];
        else if (L[i] < R[j])
            V[k] = L[i++];
        else
            V[k] = R[j++];
    }

    // liberar temporales
    free(L);
    free(R);
}

// control de argumentos
int getSize(int argc, char *argv[])
{
    if (argc < 2)
        return -1;
    return atoi(argv[1]);
}