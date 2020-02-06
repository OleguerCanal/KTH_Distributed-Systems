#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define M 2000
#define N 2000

struct complex {
    // c = a + b*i
    double a, b;
};

void save(const char* path, unsigned char image[]) {
    FILE *fp;
    fp = fopen(path,"w");
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < M; i++)
            fprintf(fp, "%hhu ", image[i+j*M]);
        fprintf(fp, "\n");
    }
    fclose(fp);
}

struct complex f(struct complex z, struct complex d) {
    struct complex result = {z.a*z.a - z.b*z.b + d.a, 2*z.a*z.b + d.b};
    return result;
}

double mod(struct complex z) {
    // return sqrt(z.a*z.a + z.b*z.b);
    return z.a*z.a + z.b*z.b;
}

unsigned char get_pixel_value(int i, int j, double area, int max_iterations) {
    // Build d complex number
    double real = i*(2*area)/(N-1) - area;
    double imaginary = j*(2*area)/(M-1) - area;
    struct complex d = {real, imaginary};

    // Iterate
    struct complex z = {0, 0};
    int count = 1;
    while(mod(z) < area && count < max_iterations) {
        z = f(z, d);
        count = count + 1;
    }
    unsigned char iterations = (unsigned char) count;
    return iterations;
}

int main() {
    printf("GO! \n");

    // Mandelbrot parameters
    double area = 3;
    int max_iterations = 1000;

    // Multiprocess vars
    int size;  // Number of processes
    int rank;  // Process id
    int tag = 0;  // Message tag (to filter messages, not really used in this case)
    int status;  // MPI status

    status = MPI_Init(NULL, NULL);
    status = MPI_Comm_size(MPI_COMM_WORLD, &size);  // Get size
    status = MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get ranks


    // Compute corresponding part of image
    int n = N/size;
    // printf("%d \n", n);

    unsigned char partial_image[n*M];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < M; j++) {
            // printf("%d \n", i*M+j);
            partial_image[i*M+j] = get_pixel_value(rank*n + i, j, area, max_iterations);}

    // printf("okii");
    
    if (rank == 0) {  // If is master
        // Create and copy computed values to complete image
        unsigned char image[N*M];
        for (int i = 0; i < n*M; i++)
            image[i] = partial_image[i];

        // printf("ok");

        // Receive from all other processes and append to main image
        unsigned char slave_partial_image[n*M];
        for (int process = 1; process < size; process++) {
            status = MPI_Recv(&slave_partial_image, n*M, MPI_UNSIGNED_CHAR, process, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int i = 0; i < n*M; i++)
                image[process*n*M + i] = slave_partial_image[i];
        }
        save("image.txt", image);
    } else {  // If slave
        // Send partial image 
        status = MPI_Send(&partial_image, n*M, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD);
    }    
    status = MPI_Finalize();
}