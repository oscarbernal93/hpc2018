#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<inttypes.h>
#include<stdint.h>

int main(int argc, char *argv[]){
    long int i=0,count=0;
    double x1, x2, x3, z, zans=0;
    double sx1,sx2,sx3;
    printf("montecarlo!\n");
    long int max=2147;//48;//3647;
    for(i=0;i<max;i++){
	//printf("%g%%\n",(double)100*i/max);
	//M + rand() / (RAND_MAX / (N - M + 1) + 1)
        x1 = (double) 0 + rand() / (RAND_MAX / (1 - 0 + 1) + 1);
        x2 = (double) 0 + rand() / (RAND_MAX / (2 - 0 + 1) + 1);
        x3 = (double) 2 + rand() / (RAND_MAX / (3 - 2 + 1) + 1);
        z =  pow((exp(x1)+x2),2)+3*pow((1-x3),2);
	if (z>zans){
	    sx1 = x1;
	    sx2 = x2;
	    sx3 = x3;
	    zans = z;
	}
    }
    printf("z = %g\n",(double)zans);
    printf("x1 = %g\n",(double)sx1);
    printf("x2 = %g\n",(double)sx2);
    printf("x3 = %g\n",(double)sx3);
}