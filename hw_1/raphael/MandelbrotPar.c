#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#define w 2048 //Width
#define h 2048 //Height
#define N 256  //Max number of loops in cal_pixel
#define b 2.0  //Boundary
#define vb 2.0 //Viewing box
#define vbcx 0 //x coordinate of center of viewing box
#define vbcy 0 //y coordinate of center of viewing box

unsigned char cal_pixel(double dreal, double dimag, double barg, int Narg);

int main(int argc, char **argv){
	//INIT
	MPI_Init(NULL,NULL);
    int P;
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    int p;
    MPI_Comm_rank(MPI_COMM_WORLD, &p);
    MPI_Status status;
	unsigned char color[w*h];
	FILE *fp;
	
	//CHECK
	if (w%P != 0) {
		MPI_Finalize();
		printf("Width of image not dividable by the number of processes\n");
		return -1;
	}
	
	//TIME
	MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();
	
	//COMPUTE ALL
	double dx = 2.0*vb/(w-1);
	double dy = 2.0*vb/(h-1);
	double xoff = 0;
	double yoff = p*w/P;
	for (int x = 0; x < w; x++){
		double dreal = vbcx+(x+xoff)*dx-vb;
		for (int y = 0; y < h/P; y++) {
			double dimag = vbcy+(y+yoff)*dy-vb;
			color[x+w*y] = cal_pixel(dreal,dimag,b,N);
		}
	}
	
	//MASTER: recieve, write to file, time
	if (p==0) {
		//RECIEVE
		for (int q = 1; q < P; q++)
			MPI_Recv(color + q*sizeof(color)/P,w*h/P,MPI_UNSIGNED_CHAR,q,0,MPI_COMM_WORLD,&status);
		
		//WRITE
		fp = fopen("color.txt","w");
		for (int j = 0; j < w; j++) {
			for (int i = 0; i < h; i++) {
				fprintf(fp,"%hhu",color[i+j*h]);
				fprintf(fp," ");
			}
			fprintf(fp,"\n");
		}
		fclose(fp);
		
		//TIME
		double end = MPI_Wtime();
        printf("Execution time: %e seconds.\n", end - start);
		
	//SLAVE: send
	} else {
		MPI_Send(color,w*h/P,MPI_UNSIGNED_CHAR,0,0,MPI_COMM_WORLD);
	}
	MPI_Finalize();
}

unsigned char cal_pixel(double dreal, double dimag, double barg, int Narg) {
	//For a given pixel (dreal+i*dimag), compute how much iterations to go out
	//output = 0 if already outside, Narg-1 if numbers of iterations bigger or equal to Narg
	int count = -1;
	double zreal = 0;
	double zimag = 0;
	double bargsq = barg*barg;
	while ((zimag*zimag+zreal*zreal < bargsq) && (count < Narg-1)) {
		double zrealn = zreal*zreal - zimag*zimag + dreal;
		zimag = zreal*zimag*2 + dimag;
		zreal = zrealn;
		count++;
	}
	return count;
}