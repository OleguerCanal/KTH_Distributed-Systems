/* Reaction-diffusion equation in 1D
 * Solution by Jacobi iteration
 * simple MPI implementation
 *
 * C Michael Hanke 2006-12-12
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define N 1000   /* number of inner grid points */
#define SMX 1000000 /* number of iterations */

extern double r(const double x) {
    return x;
}
extern double f(const double x) {
    // return exp(x)*(2*cos(x) - sin(x) + x*sin(x));
    return x*x*(x-1) + 2;
}
extern double real_u(const double x) {
    // return exp(x)*sin(x);
    return x*(x-1);
}

void print_vect(const double* vect, int size, int p) {
    for (int i = 0; i < size; i++) {
        printf("%i, %e \n", p, vect[i]);
    }
    printf("------------\n");
}

void print_to_file(const double* unew, int p, int P, int I) {
    double sendBuffer, recieveBuffer;
    int status;
    FILE *fp;
	if (p==0) {
		fp = fopen("poisson.txt","w");
		fprintf(fp,"%f ",0.0);
		for (int k = 0; k < I; k++) {
			fprintf(fp,"%f ",unew[k]);
		}
		fflush(fp);
		MPI_Send(&sendBuffer,1,MPI_DOUBLE,1,0,MPI_COMM_WORLD); // Tell p1 to start writting
	}
	if (p%(P-1)>0) {
		MPI_Recv(&recieveBuffer,1,MPI_DOUBLE,p-1,0,MPI_COMM_WORLD,&status);
		fp = fopen("poisson.txt","a");
		for (int k = 0; k < I; k++) {
			fprintf(fp,"%f ",unew[k]);
		}
		fflush(fp);
		MPI_Send(&sendBuffer,1,MPI_DOUBLE,p+1,0,MPI_COMM_WORLD); // Tell the others to start writting
	}
	if (p==(P-1)) {
		MPI_Recv(&recieveBuffer,1,MPI_DOUBLE,p-1,0,MPI_COMM_WORLD,&status);
		fp = fopen("poisson.txt","a");
		for (int k = 0; k < I; k++) {
			fprintf(fp,"%f ",unew[k]);
		}
		fprintf(fp,"%f",0.0);
    }
}

/* We assume linear data distribution. The formulae according to the lecture
   are:
      L = N/P;
      R = N%P;
      I = (N+P-p-1)/P;    (number of local elements)
      n = p*L+MIN(p,R)+i; (global index for given (p,i)
   Attention: We use a small trick for introducing the boundary conditions:
      - The first ghost point on p = 0 holds u(0);
      - the last ghost point on p = P-1 holds u(1).
   Hence, all local vectors hold I elements while u has I+2 elements.
*/

int main(int argc, char *argv[]) {
    int P, p;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &p);
    if (N < P) {
        fprintf(stdout, "Too few discretization points...\n");
        return 0;
    }
    double h = (double) 1.0/N;

    int I = (N+P-p-1)/P;  // Elements in current processor
    printf("I %i \n", I);
    double* unew = (double*) malloc(I*sizeof(double));
    double* u = (double*) malloc((I+2)*sizeof(double));
    u[0] = 0;
    u[I+1] = 0;

    // Vectors with function points
    double* rr = (double*) malloc(I*sizeof(double));
    double* ff = (double*) malloc(I*sizeof(double));
    double* u_real = (double*) malloc(I*sizeof(double));  // NOTE: This should probably br I+2

    int L = N/P;          // Elements by processor
    int R = N%P;          // 1 element added to first R processors
    for (int i = 0; i < I; i++) {
        double x = h*(p*L + MIN(p,R) + i);
        // printf("I: %i, %e \n", p, x);
        rr[i] = r(x);
        ff[i] = f(x);
        u_real[i] = real_u(x);
    }

/* Jacobi iteration */
    for (int step = 0; step < SMX; step++) {
/* RB communication of overlap */
        if (p%2 == 0) { // If BLACK: first send, then receive
            if (p < P-1) { // No need to send anything for last value
                MPI_Send(&u[I], 1, MPI_DOUBLE, p+1, 0, MPI_COMM_WORLD);
                MPI_Recv(&u[I+1], 1, MPI_DOUBLE, p+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            if (p > 0) { // No need to send anything for first value
                MPI_Send(&u[1], 1, MPI_DOUBLE, p+1, 0, MPI_COMM_WORLD);
                MPI_Recv(&u[0], 1, MPI_DOUBLE, p+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
        else { // If RED: first receive, then send
            if (p > 0) { // No need to send anything for first value
                MPI_Recv(&u[0], 1, MPI_DOUBLE, p-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&u[1], 1, MPI_DOUBLE, p-1, 0, MPI_COMM_WORLD);
            }
            if (p < P-1) { // No need to send anything for last value
                MPI_Recv(&u[I+1], 1, MPI_DOUBLE, p-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&u[I], 1, MPI_DOUBLE, p-1, 0, MPI_COMM_WORLD);
            }
        }
/* local iteration step */
        for (int i = 0; i < I; i++)
            unew[i] = (u[i] + u[i+2]-h*h*ff[i])/(2.0-h*h*rr[i]);
        for (int i = 0; i < I; i++)
            u[i+1] = unew[i]; // OBS: We dont change u[0], u[-1]
    }
    double error = 0;
    for (int i = 0; i < I; i++) {
        error += (u[i]-u_real[i])*(u[i]-u_real[i]);
    }
    printf("Error: %e \n", sqrt(error));
    print_to_file(unew, p, P, I);
    MPI_Finalize();
}