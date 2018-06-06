#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<inttypes.h>
#include<stdint.h>
#include <time.h>

const int numRest = 13;
const int numVars = 12;

double rand_in_range(double m,double n){
    //M + rand() / (RAND_MAX / (N - M + 1) + 1)
    return (double) m + rand() / (RAND_MAX / (n - m + 1) + 1);
}

int main(int argc, char *argv[]){
    srand (time(NULL));
    int i=0,j=0,k=0,count=0;
    long int r=0;
    double x=0,z=0,zans=0,top=0,tmp=0;
float COEF[numRest][numVars] = {
        {1,0,0,0,0,0,1,0,0,0,0,0},
        {0,1,0,0,8,0,0,0,0,1,0,0},
        {0,0,1,0,0,0,1,0,1,0,0,0},
        {0,0,0,1,0,0,0,0,0,3,0,1},
        {0,0,0,0,1,0,0,1,0,0,0,0},
        {0,0,0,0,0,1,0,0,0,0,0,0},
        {0,0,0,0,0,0,1,0,0,0,0,0},
        {0,0,0,0,0,0,0,1,0,0,0,2},
        {0,0,0,0,0,0,0,0,1,0,0,0},
        {0,0,0,0,0,0,0,0,0,1,0,0},
        {0,0,0,0,0,0,0,0,0,0,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1}
    };
    float P[numRest] = {94,93,92,91,90,29,28,27,26,25,24,23};
    float Z[numVars]  = {8,8,7,7,6,6,5,5,4,4,3,2};
    double C[numVars]    = {0};
    double CANS[numVars] = {0};

    printf("montecarlo!\n");
    long int max=214748;//36;//47;
    for(r=0;r<max;r++){
        while(i < numRest && r<max)
        {
            tmp = 0;
            for (j = 0; j < numVars; ++j)
            {   
                if (i == 0){
                    //si esta en la primera vuelta genera nuevos numeros
                    top =  P[i];
                    if (COEF[i][j]>0)
                    {
                        top = P[i]/COEF[i][j];
                    }
                    x = rand_in_range(0,top);
                    C[j] = x; 
                }
                tmp += C[j]*COEF[i][j];
            }
            if(tmp <= P[i]){
                //se cumple la condicion
                i++;
            }else{
                //si no se cumple alguna condicion
                //vuelve a empezar
                i = 0;
                r++;
            }
        }
        //si finaliza es porque se cumplieron todas las condiciones
        z=0;
        for (k = 0; k < numVars; ++k)
        {
            z+= C[k]*Z[k];
        }
        if (z>zans){
            for (k = 0; k < numVars; ++k)
            {
                CANS[k]=C[k];
            }
            zans = z;
        }
    }
    printf("z = %g\n",(double)zans);
    for (k = 0; k < numVars; ++k)
        {
            printf("x%i = %g\n",k+1,(double)CANS[k]);
        }
}