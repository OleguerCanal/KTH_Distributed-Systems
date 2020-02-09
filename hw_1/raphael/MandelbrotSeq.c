#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define w 2048
#define h 2048
#define N 256
#define b 2.0
#define P 1
#define p 0

unsigned char cal_pixel(double dreal, double dimag, double barg, int Narg);

int main(void){
	unsigned char color[w*h];
	FILE *fp;
	double start = clock();
	// Need to check if can devide by p
	// Need to translate to c
	double dx = 2.0*b/(w-1);
	double dy = 2.0*b/(h-1);
	double xoff = p*w/P;
	double yoff = 0;
	for (int x = 0; x < w; x++){
		double dreal = (x+xoff)*dx-b;
		for (int y = 0; y < h; y++) {
			double dimag = (y+yoff)*dy-b;
			color[x+w*y] = cal_pixel(dreal,dimag,b,N);
		}
	}

	fp = fopen("color.txt","w");
	for (int j = 0; j < w; j++) {
		for (int i = 0; i < h; i++) {
			fprintf(fp,"%hhu",color[i+j*h]);
			fprintf(fp," ");
		}
		fprintf(fp,"\n");
	}
	fclose(fp);
    double end = clock();
    printf("Execution time: %e seconds.\n", ((double) (end - start))/CLOCKS_PER_SEC);
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
		
	// if ((abs(dreal) < 0.001) && (abs(dimag) < 0.001)) {
	// printf("%f",zreal);
	// printf(" ");
	// printf("%f",zimag);
	// printf(" ");
	// printf("%i",zimag*zimag+zreal*zreal < bargsq);
	// printf(" ");
	// printf("%i",count < Narg);
	// printf(" ");
	// printf("%i",count);
	// printf("\n");}
	}
	return count;
}