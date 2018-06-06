#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<inttypes.h>
#include<stdint.h>

int main(int argc, char *argv[]){
    long int i=0,count=0;
    double x, y, z, p;
    printf("montecarlo!\n");
    long int max=214748364;//7;
    for(i=0;i<max;i++){
	//printf("%g%%\n",(double)100*i/max);
        x = (double) rand()/RAND_MAX;
        y = (double) rand()/RAND_MAX;
        z = x*x + y*y;
	if (z<=1) count++;
    }
    printf("count:%ld\n",count);
    printf("i:%ld\n",i);
    p = (double)count/i*4;
    printf("pi = %g\n",(double)p);
}