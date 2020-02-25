
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>


void mergeTop (const double* in1, double* out, const double I1, const double* in2, const double I2)
{
	int i1 = I1-1;
	int i2 = I2-1;
	for (int k = I1; k > 0; k--) {
		if (in1[i1] > in2[i2]) {
			out[k-1] = in1[i1];
			i1--;
		} else {
			out[k-1] = in2[i2];
			i2--;
		}
	}
	return;
}

void mergeBot (const double* in1, double* out, const double I1, const double* in2, const double I2)
{
	int i1 = 0;
	int i2 = 0;
	int k = 0;
	while (k < I1 && i2 < I2) {
		if (in1[i1] < in2[i2]) {
			out[k] = in1[i1];
			i1++;
		} else {
			out[k] = in2[i2];
			i2++;
		}
		k++;
	}
	if (k < I1)
		out[k] = in1[0];
	return;
}

int cmpfunc (const void * a, const void * b)
{
  if (*(double*)a > *(double*)b)
    return 1;
  else if (*(double*)a < *(double*)b)
    return -1;
  else
    return 0;  
}


int main(int argc, char **argv)
{
	int p,P;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &P);
	MPI_Comm_rank(MPI_COMM_WORLD, &p);
    MPI_Status status;
	
	/* Find problem size N from command line */
	if (argc < 2) {
		printf("No size N given");
		exit(1);
	}
	int N = atoi(argv[1]);
	
	/* local size*/
	int I = (N+P-p-1)/P;
	int Ipminus1 = (N+P-p)/P; //I of p-1
	int Ipplus1 = (N+P-p-2)/P;	//I of p+1
	
	/* Init vector */
	double* x = (double *) malloc(I*sizeof(double));
	double* y = (double *) malloc(I*sizeof(double));
	double* z = (double *) malloc(Ipminus1*sizeof(double));
	double* a;
	
	/* random number generator initialization */
	srandom(p+1);
	
	/* data generation */
	for (int i = 0; i < I; i++)
		x[i] = ((double) random())/(((double) RAND_MAX)+1);
		
	/* Sort local data */
	qsort(x, I, sizeof(double), cmpfunc);


	for (int step = 0; step < N; step++) { 											//Stop earlier ?
		/*printf("it %i, proc %i: ",step, p);
		for (int i = 0; i < I; i++) {
			printf("%.3f ",x[i]);
		}	
		printf("\n");
		fflush(stdout);*/
		
		if ((p+step)%2 == 0) {
			if (p < P-1) {
				MPI_Send(x,I,MPI_DOUBLE,p+1,0,MPI_COMM_WORLD);
				MPI_Recv(z,Ipplus1,MPI_DOUBLE,p+1,0,MPI_COMM_WORLD,&status);
				mergeBot(x,y,I,z,Ipplus1);
				a = x;
				x = y;
				y = a;
			}
		} else {
			if (p > 0) {
				MPI_Recv(z,Ipminus1,MPI_DOUBLE,p-1,0,MPI_COMM_WORLD,&status);
				MPI_Send(x,I,MPI_DOUBLE,p-1,0,MPI_COMM_WORLD);
				mergeTop(x,y,I,z,Ipminus1);	
				a = x;
				x = y;
				y = a;
			}	
		}
	}
	
	printf("%i: ",p);
	for (int i = 0; i < I; i++) {
		printf("%.3f ",x[i]);
	}	
	printf("\n");
	
	MPI_Finalize();
	exit(0);
}