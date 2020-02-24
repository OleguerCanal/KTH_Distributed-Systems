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
    return exp(x)*(2*cos(x) - sin(x) + x*sin(x));
}
extern double real_u(const double x) {
    return exp(x)*sin(x);
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

    int I = (N+P-p-1)/P;
    double* unew = (double*) malloc(I*sizeof(double));
    double* u = (double*) malloc((I+2)*sizeof(double));
    u[0] = 0;
    u[I+1] = 0;
    double* u_real = (double*) malloc(I*sizeof(double));  // NOTE: This should probably br I+2
    double* rr = (double*) malloc(I*sizeof(double));
    double* ff = (double*) malloc(I*sizeof(double));

    int L = N/P;
    int R = N%P;
    for (int i = 0; i < I; i++) {
        double x = p*L + MIN(p,R) + i;
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
	    unew[i] = (u[i]+u[i+2]-h*h*ff[i])/(2.0-h*h*rr[i]);
    for (int i = 0; i < I; i++)
        u[i+1] = unew[i]; // OBS: We dont change u[0], u[-1]
    }

    double error = 0;
    for (int i = 0; i < I; i++) {
        error += (u[i]-u_real[i])*(u[i]-u_real[i]);
    }
    printf("Error: %e \n", sqrt(error));

/* output for graphical representation */
/* Instead of using gather (which may lead to excessive memory requirements
   on the master process) each process will write its own data portion. This
   introduces a sequentialization: the hard disk can only write (efficiently)
   sequentially. Therefore, we use the following strategy:
   1. The master process writes its portion. (file creation)
   2. The master sends a signal to process 1 to start writing.
   3. Process p waites for the signal from process p-1 to arrive.
   4. Process p writes its portion to disk. (append to file)
   5. process p sends the signal to process p+1 (if it exists).
*/
    MPI_Finalize();
}