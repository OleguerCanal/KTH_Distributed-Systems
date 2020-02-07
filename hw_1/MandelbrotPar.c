#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#define w 2048
#define h 2048
#define N 256
#define b 2.0
#define vb 2.0//0.2 //Viewing box
#define vbcx 0//-0.1 //x coordinate of center of viewing box
#define vbcy 0//1 //x coordinate of center of viewing box

unsigned char cal_pixel(double dreal, double dimag, double barg, int Narg);

int main(int argc, char **argv){
	// int w = (int) *argv[1];
	// int h = (int) *argv[2];
	// int N = (int) *argv[3];
	// double b = (double) *argv[4];
	MPI_Init(NULL,NULL);
    int P;
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    int p;
    MPI_Comm_rank(MPI_COMM_WORLD, &p);
    MPI_Status status;
	unsigned char color[w*h];
	FILE *fp;
	
	// Need to check if can devide by p
	// Need to translate to c
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
	
	if (p==0) {
		for (int q = 1; q < P; q++)
			MPI_Recv(color + q*sizeof(color)/P,w*h/P,MPI_UNSIGNED_CHAR,q,0,MPI_COMM_WORLD,&status);
		fp = fopen("color.txt","w");
		for (int j = 0; j < w; j++) {
			for (int i = 0; i < h; i++) {
				fprintf(fp,"%hhu",color[i+j*h]);
				fprintf(fp," ");
			}
			fprintf(fp,"\n");
		}
		fclose(fp);
	} else {
		MPI_Send(color,w*h/P,MPI_UNSIGNED_CHAR,0,0,MPI_COMM_WORLD);
		// printf("%i",p);
	}
	MPI_Finalize();
}

unsigned char cal_pixel(double dreal, double dimag, double barg, int Narg) {
	int count = -1; // Zero or one ?
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