/* Reaction-diffusion equation in 1D
 * Solution by Jacobi iteration
 * simple MPI implementation
 *
 * C Michael Hanke 2006-12-12
 */


#define MIN(a,b) ((a) < (b) ? (a) : (b))

/* Use MPI */
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

/* define problem to be solved */
#define N 1000   /* number of inner grid points */
#define SMX 10 /* number of iterations */

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

int main(int argc, char *argv[])
{
/* implement coefficient functions */
	// u = x*x-x
	double r(const double x) {return -x;};
	double f(const double x) {return 1+x*x*(x-1);};
	double h = 1.0/(N+1);

/* Initialize MPI */
    MPI_Init(&argc, &argv);
	int p,P;
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &p);
    if (N < P) {
		fprintf(stdout, "Too few discretization points...\n");
		exit(1);
    }
    MPI_Status status;
		
/* local variable */
    int L = N/P;
    int R = N%P;
    int I = (N+P-p-1)/P;    //(number of local elements)
    //n = p*L+MIN(p,R)+i; //(global index for given (p,i)
	int n0 = p*L+MIN(p,R);
	
/* Compute local indices for data distribution */
	double* ff = (double *) malloc(I*sizeof(double));
	double* rr = (double *) malloc(I*sizeof(double));
	for (int j = 0; j<I; j++) {
		ff[j] = f(((double) (n0+j))/(N+1));
		rr[j] = r(((double) (n0+j))/(N+1));
	};

/* arrays */
    double* unew = (double *) malloc(I*sizeof(double));
/* Note: The following allocation includes additionally:
   - boundary conditins are set to zero
   - the initial guess is set to zero */
    double* u = (double *) calloc(I+2, sizeof(double));


/* Jacobi iteration */
    for (int step = 0; step < SMX; step++) {
/* RB communication of overlap */
		if (step>0) {
			if (p%2==1) {
				MPI_Send(unew,1,MPI_DOUBLE,p-1,0,MPI_COMM_WORLD);
				MPI_Recv(u,1,MPI_DOUBLE,p-1,1,MPI_COMM_WORLD,&status);
				printf("0 - %i:\t%g\t%g\t%g\t%g\n",p,u[0],u[I+1],unew[0],unew[I-1]);
				if (p<P-1) {
					MPI_Send(unew+(I-1)*sizeof(double),1,MPI_DOUBLE,p+1,2,MPI_COMM_WORLD);
					MPI_Recv(u+(I+1)*sizeof(double),1,MPI_DOUBLE,p+1,3,MPI_COMM_WORLD,&status);
					printf("1 - %i:\t%g\t%g\t%g\t%g\n",p,u[0],u[I+1],unew[0],unew[I-1]);
				}
			} else {
				if (p<P-1) {
					MPI_Recv(u+(I+1)*sizeof(double),1,MPI_DOUBLE,p+1,0,MPI_COMM_WORLD,&status);
					MPI_Send(unew+(I-1)*sizeof(double),1,MPI_DOUBLE,p+1,1,MPI_COMM_WORLD);
					printf("2 - %i:\t%g\t%g\t%g\t%g\n",p,u[0],u[I+1],unew[0],unew[I-1]);
				}
				if (p>0) {
					MPI_Recv(u,1,MPI_DOUBLE,p-1,2,MPI_COMM_WORLD,&status);
					MPI_Send(unew,1,MPI_DOUBLE,p-1,3,MPI_COMM_WORLD);
					printf("3 - %i:\t%g\t%g\t%g\t%g\n",p,u[0],u[I+1],unew[0],unew[I-1]);
				}
			}
		}
		
/* local iteration step */
		for (int i = 0; i < I; i++) {
			unew[i] = (u[i]+u[i+2]-h*h*ff[i])/(2.0-h*h*rr[i]);
		}
		for (int i = 0; i < I; i++) {
			u[i+1] = unew[i];
		}
		
		//Print loop
		if (p == P/2)
			if ((step+1)%10000==0)
				printf("Step %i: %f\n",step+1,u[0]);
    }

/* output for graphical representation */
	FILE *fp;
	
	if (p==0) {
		fp = fopen("poisson.txt","w");
		fprintf(fp,"%f ",0.0);
		for (int k = 0; k < I; k++) {
			fprintf(fp,"%f ",unew[k]);
		}
		MPI_Send(u,1,MPI_LOGICAL,1,0,MPI_COMM_WORLD);
	} 
	if (p%(P-1)>0) {
		MPI_Recv(u,1,MPI_LOGICAL,p-1,0,MPI_COMM_WORLD,&status);
		fp = fopen("poisson.txt","a");
		for (int k = 0; k < I; k++) {
			fprintf(fp,"%f ",unew[k]);
		}
		MPI_Send(u,1,MPI_LOGICAL,p+1,0,MPI_COMM_WORLD);
	}
	if (p==P-1) {
		MPI_Recv(u,1,MPI_LOGICAL,p-1,0,MPI_COMM_WORLD,&status);
		fp = fopen("poisson.txt","a");
		for (int k = 0; k < I; k++) {
			fprintf(fp,"%f ",unew[k]);
		}
		fprintf(fp,"%f",0.0);
	}
	
	
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

/* That's it */
    MPI_Finalize();
    exit(0);
}
