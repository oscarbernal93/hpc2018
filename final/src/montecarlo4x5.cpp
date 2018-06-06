#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<inttypes.h>
#include<stdint.h>
#include <time.h>

const int numRest = 5;
const int numVars = 4;

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
        {2,1,1,4},
        {1,0,3,0},
        {0,1,0,3},
        {2,2,1,0},
        {9,0,0,1}
    }; 
    double C[numVars]    = {0};
    double CANS[numVars] = {0};
    double P[numRest]    = {6,4,24,3,41};
    double Z[numVars]    = {18,24,25,12};

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