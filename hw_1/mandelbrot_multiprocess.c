#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define M 2048
#define N 2048

struct complex {
    // c = a + b*i
    double a, b;
};

struct complex f(struct complex z, struct complex d) {
    struct complex result = {z.a*z.a - z.b*z.b + d.a, 2*z.a*z.b + d.b};
    return result;
}

double mod(struct complex z) {
    // return sqrt(z.a*z.a + z.b*z.b);
    return z.a*z.a + z.b*z.b;
}

unsigned char get_pixel_value(int i, int j, double area, int max_iterations, struct complex center) {
    // Build d complex number
    double real = (((double) i)*(2.0*area)/((double) N-1.0)) - area - center.a;
    // printf("%e \n", real);
    double imaginary = (((double) j)*(2.0*area)/((double) M-1.0)) - area - center.b;
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
    // Mandelbrot parameters
    double area = 015;
    struct complex center = {0.0, 0.0};
    int max_iterations = 256;
    char text_img_path[] = "image.txt";

    // Multiprocess vars
    int size;  // Number of processes
    int rank;  // Process id
    int tag = 0;  // Message tag (to filter messages, not really used in this case)
    int status;  // MPI status

    status = MPI_Init(NULL, NULL);
    status = MPI_Comm_size(MPI_COMM_WORLD, &size);  // Get size
    status = MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get ranks

    // TIMING
    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    int n = N/size;
    if (rank == 0) {  // If is master
        // Compute first chunk of image
        FILE *fp;
        fp = fopen(text_img_path, "w");
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < M; j++)
                fprintf(fp, "%hhu ", get_pixel_value(i, j, area, max_iterations, center));
            fprintf(fp, "\n");
        }

        // Receive from all other processes and append to main image
        unsigned char slave_partial_image[n*M];
        for (int process = 1; process < size; process++) {
            status = MPI_Recv(&slave_partial_image, n*M, MPI_UNSIGNED_CHAR, process, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < M; ++j)
                    fprintf(fp, "%hhu ", slave_partial_image[i*M + j]);
                fprintf(fp, "\n");
            }
        }
        fclose(fp);

        // TIMING
        double end = MPI_Wtime();
        printf("Execution time: %e seconds.\n", end - start);
    } else {  // If slave
        // Compute corresponding part of image
        unsigned char partial_image[n*M];
        for (int i = 0; i < n; i++)
            for (int j = 0; j < M; j++)
                partial_image[i*M+j] = get_pixel_value(rank*n + i, j, area, max_iterations, center);
        
        // Send partial image
        status = MPI_Send(&partial_image, n*M, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD);
    }    
    status = MPI_Finalize();
}