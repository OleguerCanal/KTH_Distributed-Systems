#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

// mpicc odd_even_sort.c -o odd_even_sort; mpiexec -n 1 odd_even_sort;

void fillArray(double *x, int n, int p) {
    // Fills array of length n with random doubles
    srandom(p + 1);  // Set random seed
    for (int i = 0; i < n; i++)
        x[i] = (double)rand() / (double)(RAND_MAX);
}

void swap(double *a, double *b) {
    // Swap two doubles
    double t = *a;
    *a = *b;
    *b = t;
}

int partition(double *arr, int low, int high) {
    double pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j < high; j++)
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(double *arr, int low, int high) {
    // Single-process sorting
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

void mergeArrays(double *in1, int n1, double *in2, int n2, double *out) {
    // Given two soretd arrays, returns sorted merge
    int it1 = 0;
    int it2 = 0;

    double val = -1;
    for (int i = 0; i < n1 + n2; i++) {
        if (it1 < n1 && it2 < n2) {
            if (in1[it1] < in2[it2]) {
                val = in1[it1];
                it1++;
            } else {
                val = in2[it2];
                it2++;
            }
        } else if (it1 < n1) {
            val = in1[it1];
            it1++;
        } else {
            val = in2[it2];
            it2++;
        }
        out[i] = val;
    }
}

void copyArray(double *in, double *out, int low, int high) {
    // Copes in[low:high] to out
    for (int i = low; i < high; i++)
        out[i - low] = in[i];
}

void printArray(double *x, int n, int p) {
    // Prints array
    printf("Processor %i: ", p);
    for (int i = 0; i < n; i++)
        printf("%f ", x[i]);
    printf("\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("No size N given");
        return 0;
    }

    // Get MPI info
    int P, p;  // Number of processors, current processor
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &p);

    // Timing
    // MPI_Barrier(MPI_COMM_WORLD);

    int N = atoi(argv[1]);
    int I = N / P;
    int I_prev = (p - 1 < N % P) ? I + 1 : I;
    int I_next = (p + 1 < N % P) ? I + 1 : I;
    I = (p < N % P) ? I + 1 : I;

    // printf("Processor: %i, ", p);
    // printf("I_prev %i, ", I_prev);
    // printf("I %i, ", I);
    // printf("I_next %i\n", I_next);

    double *x = (double *)malloc(I * sizeof(double));
    double *y_prev = (double *)malloc(I_prev * sizeof(double));
    double *y_next = (double *)malloc(I_next * sizeof(double));
    double *merged = (double *)malloc((2 * I + 2) * sizeof(double));  // Container to merge vectors (only allocate once)
    fillArray(x, I, p);

    double start_time = MPI_Wtime();
    quickSort(x, 0, I - 1);

    bool even_process = p % 2;
    bool first_process = (p == 0);
    bool last_process = (p == P - 1);
    // MPI_Barrier(MPI_COMM_WORLD);

    for (int step = 0; step <= P; step++) {
        bool even_step = step % 2;
        if (even_step) {
            if (even_process && !last_process) {
                MPI_Send(x, I, MPI_DOUBLE, p + 1, 0, MPI_COMM_WORLD);
                MPI_Recv(y_next, I_next, MPI_DOUBLE, p + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                mergeArrays(x, I, y_next, I_next, merged);  // Sorted Merge
                copyArray(merged, x, 0, I);                 // Get Min
            }
            if (!even_process && !first_process) {  // odd porcess
                MPI_Recv(y_prev, I_prev, MPI_DOUBLE, p - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(x, I, MPI_DOUBLE, p - 1, 0, MPI_COMM_WORLD);
                mergeArrays(x, I, y_prev, I_prev, merged);  // Sorted Merge
                copyArray(merged, x, I_prev, I_prev + I);   // Get Max
            }
        } else {  // odd step
            if (even_process && !first_process) {
                MPI_Send(x, I, MPI_DOUBLE, p - 1, 0, MPI_COMM_WORLD);
                MPI_Recv(y_prev, I_prev, MPI_DOUBLE, p - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                mergeArrays(x, I, y_prev, I_prev, merged);  // Sorted Merge
                copyArray(merged, x, I_prev, I_prev + I);   // Get Max
            }
            if (!even_process && !last_process) {  // odd porcess
                MPI_Recv(y_next, I_next, MPI_DOUBLE, p + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(x, I, MPI_DOUBLE, p + 1, 0, MPI_COMM_WORLD);
                mergeArrays(x, I, y_next, I_next, merged);  // Sorted Merge
                copyArray(merged, x, 0, I);                 // Get Min
            }
        }
        // MPI_Barrier(MPI_COMM_WORLD);
    } 

    double processor_time = MPI_Wtime() - start_time;
    // printf("%f\n", processor_time);
    double max_time;
    MPI_Reduce(&processor_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);  // Get max execution time

    if (p == 0) {
        // double parallel_time = MPI_Wtime() - start_time;
        int length = snprintf(NULL, 0, "%d", N);
        char *filename = malloc(length + 1);
        snprintf(filename, length + 1, "%d", N);
        // filename = strcat("times/", filename);
        FILE *file = fopen(filename, "a");
        fprintf(file, "%d ", P);
        fprintf(file, "%f\n", max_time);
        printf("Max time: %f\n", max_time);
    }
    // Print result
    // if (p == 0)
    //     printf("Sorted result: \n");
    // for (int i = 0; i < P; i++)
    //     if (p == i)
    //         printArray(x, I, p);

    MPI_Finalize();
}