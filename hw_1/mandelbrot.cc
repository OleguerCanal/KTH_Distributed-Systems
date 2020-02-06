#include <stdio.h>
#include <math.h>
// #include "mpi.h"

#define M 2048
#define N 2048

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

complex f(complex z, complex d) {
    struct complex result = {z.a*z.a - z.b*z.b + d.a, 2*z.a*z.b + d.b};
    return result;
}

double mod(complex z) {
    return sqrt(z.a*z.a + z.b*z.b);
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
    printf("test2");

    double area = 3;
    int max_iterations = 1000;

    unsigned char image[N*M];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
            image[i*M+j] = get_pixel_value(i, j, area, max_iterations);
    save("image.txt", image);
}