#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"

// mpicc odd_even_sort.c -o odd_even_sort; mpiexec -n 1 odd_even_sort;

void fillArray(double* x, int n, int p) {
    // Fills array of length n with random doubles
    srandom(p+1); // Set random seed
    for (int i = 0; i < n; i++)
        x[i] = (double)rand()/(double)(RAND_MAX);
}

void swap(double* a, double* b) {
    // Swap two doubles
    double t = *a;
    *a = *b;
    *b = t;
}

int partition(double* arr, int low, int high) {
    double pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j < high; j++)
        if (arr[j] < pivot)  {
            i++;
            swap(&arr[i], &arr[j]);
        }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(double* arr, int low, int high) {
    // Single-process sorting
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    } 
}

void mergeArrays(double* in1, int n1, double* in2, int n2, double* out) {
    // Given two soretd arrays, returns sorted merge
    int it1 = 0;
    int it2 = 0;

    double val = -1;
    for (int i = 0; i < n1+n2; i++) {
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

void copyArray(double* in, double* out, int low, int high) {
    // Copes in[low:high] to out
    for (int i = low; i < high; i++)
        out[i-low] = in[i];
}

void updateArrayX(double* x, double* y, int I, bool get_min) {
    // Merges arrays x and y and:
    // if get_min: assigns first half to x
    // else: assigns second half to x
    double merged[2*I];
    mergeArrays(x, I, y, I, merged);
    if (get_min)
        copyArray(merged, x, 0, I); // Get min
    else
        copyArray(merged, x, I, 2*I); // Get max
}

void printArray(double* x, int n, int p) {
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
    int P, p; // Number of processors, current processor
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &p);

    // Timing
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();
    
    int N = atoi(argv[1]);
    int I = N/P; // Local size (think if n mod p not 0!!)
    double x[I], y[I];
    fillArray(x, I, p);
    quickSort(x, 0, I-1);
    // printArray(x, I, p);

    bool even_process = p%2;
    bool first_process = (p == 0);
    bool last_process = (p == P-1);
    for (int step = 0; step < P; step++) {
        bool even_step = step%2;
        if (even_step) {
            if (even_process && !last_process) {
                MPI_Send(&x, I, MPI_DOUBLE, p+1, 0, MPI_COMM_WORLD);
                MPI_Recv(&y, I, MPI_DOUBLE, p+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                updateArrayX(x, y, I, true);
            }
            if (!even_process && !first_process) { // odd porcess
                MPI_Recv(&y, I, MPI_DOUBLE, p-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&x, I, MPI_DOUBLE, p-1, 0, MPI_COMM_WORLD);
                updateArrayX(x, y, I, false);
            }
        } else { // odd step
            if (even_process && !first_process) {
                MPI_Send(&x, I, MPI_DOUBLE, p-1, 0, MPI_COMM_WORLD);
                MPI_Recv(&y, I, MPI_DOUBLE, p-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                updateArrayX(x, y, I, false);
            }
            if (!even_process && !last_process) { // odd porcess
                MPI_Recv(&y, I, MPI_DOUBLE, p+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&x, I, MPI_DOUBLE, p+1, 0, MPI_COMM_WORLD);
                updateArrayX(x, y, I, true);
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    if (p == 0) {
        double end_time = MPI_Wtime();
        printf("Execution time: %e seconds.\n", end_time - start_time);
    }

    // Print result
    if (p == 0)
        printf("Sorted result: \n");
    for (int i = 0; i < P; i++)
        if (p == i)
            printArray(x, I, p);

    MPI_Finalize();
}