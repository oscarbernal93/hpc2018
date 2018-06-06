#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void read_header(FILE* fp,int *numVars,int *numRest);
void read_data(FILE* fp,double *COEF,double *P,double *Z);
void print_matrix(double *data,int mr, int mc);
void print_array(double *data,int len);

__global__ void montecarlo(double *COEF,double *P,double *Z,double *C,double *CANS,int numRest, int numVars)
{ 
    int pos = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;
    double z = 0,zans = 0;
    int i = 0,r = 0,j = 0;
    int max=1024;
    // x,y = x * numCol + y
    for(r=0;r<max;r++){
        while(i < numRest && r<max)
        {
            tmp = 0;
            for (j = 0; j < numVars; ++j)
            {   
                if (i == 0){
                    //si esta en la primera vuelta genera nuevos numeros
                    top =  P[i];
                    if (COEF[i*numVars+j]>0)
                    {
                        top = P[i]/COEF[i*numVars+j];
                    }
                    x = rand_in_range(0,top);
                    C[j] = x; 
                }
                tmp += C[j]*COEF[i*numVars+j];
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
                CANS­[pos*(numVars+1)+k]=C[k];
            }
            CANS[pos*(numVars+1)+numVars] = z;
            zans = z;
        }
    }
}

int main(int argc, char **argv)
{
    if( 2 != argc ){
        printf("Incorrect number of params: %d\n",argc - 1);
        return 1;
    }
    //Vars Declaration
    int tile_dim = 1024,index = 0;
    int numRest,numVars,i = 0;
    double zans = 0,z = 0;
    double *gpu_COEF, *cpu_COEF;
    double *gpu_P, *cpu_P;
    double *gpu_Z, *cpu_Z;
    double *gpu_C, *cpu_C;
    double *gpu_CANS, *cpu_CANS;
    /*
    cudaError_t err = cudaSuccess;
    */
    // Open the files
    FILE* fp = fopen(argv[1], "r");
    // Check if files exists
    if (fp == NULL)
    {
        printf("Could not open file %s",argv[1] );
        return 1;
    }
    //Read the number of rows and cols of each matrix
    read_header(fp,&numVars,&numRest);
    printf("%i,%i\n", numVars,numRest);

    //reserve memory to each matrix
    int drv = numVars*numRest*sizeof(double);
    int dv = numVars*sizeof(double);
    int dr = numRest*sizeof(double);
    int cs = pow(tile_dim,2)*(numVars + 1)*sizeof(double);

    cpu_COEF = (double*)malloc(drv);
    cpu_P = (double*)malloc(dr);
    cpu_Z = (double*)malloc(dv);
    cpu_C = (double*)calloc(numVars+1,sizeof(double));
    cpu_CANS = (double*)calloc(pow(tile_dim,2)*(numVars + 1),sizeof(double));

    err = cudaMalloc((void**)&gpu_COEF,drv);
    if(err != cudaSuccess){printf("Error with COEF\n");exit(1);}
    err = cudaMalloc((void**)&gpu_P,dr);
    if(err != cudaSuccess){printf("Error with P\n");exit(1);}
    err = cudaMalloc((void**)&gpu_Z,dv);
    if(err != cudaSuccess){printf("Error with Z\n");exit(1);}
    err = cudaMalloc((void**)&gpu_C,dv);
    if(err != cudaSuccess){printf("Error with C\n");exit(1);}
    err = cudaMalloc((void**)&gpu_CANS,cs);
    if(err != cudaSuccess){printf("Error with CANS\n");exit(1);}

    //Read the Files to the CPU memory
    read_data(fp,cpu_COEF,cpu_P,cpu_Z);
    //Now the files can be closed
    fclose(fp);
    //Copy each matrix to the device
    err = cudaMemcpy(gpu_COEF, cpu_COEF, drv, cudaMemcpyHostToDevice);
    if(err != cudaSuccess){printf("Error Coping COEF\n");exit(1);}
    err = cudaMemcpy(gpu_P, cpu_P, dr, cudaMemcpyHostToDevice);
    if(err != cudaSuccess){printf("Error Coping P\n");exit(1);}
    err = cudaMemcpy(gpu_Z, cpu_Z, dv, cudaMemcpyHostToDevice);
    if(err != cudaSuccess){printf("Error Coping Z\n");exit(1);}

    //Execute the Kernel in the Device
    dim3 block_dim(tile_dim,1,1);
    dim3 grid_dim(tile_dim,1,1);

    montecarlo<<<grid_dim,block_dim>>>(gpu_COEF,gpu_P,gpu_Z,gpu_C,gpu_CANS,numRest, numVars);

    //copy the result to Host mem
    err = cudaMemcpy(cpu_CANS,gpu_CANS,cs, cudaMemcpyDeviceToHost);
    if(err != cudaSuccess){printf("Error Coping Matrix R\n");exit(1);}

    for (i = 0; i < tile_dim*tile_dim; ++i)
    {
        z = cpu_CANS[i*(numVars+1)+numVars];
        if(z < zans)­{
            zans = z;
            index = i;
        }
    }
    for (int i = 0; i < numVars; ++i)
    {
        C[i] = cpu_CANS[index*(numVars+1)+i];
    }  
    //to this point
    //zans = contains the greather Z
    //C contains the value of each var 

    free(cpu_COEF);
    free(cpu_P);
    free(cpu_Z);
    free(cpu_C);
    free(cpu_CANS);
}

void read_header(FILE* fp,int *numVars,int *numRest){
    char c;
    int nc = 0;
    int temp = 0;
    int pot = 0;
	for (c = getc(fp); c != '\n'; c = getc(fp)){
        nc = (int)c;
        if(nc >= 48 && nc <= 57){
            //add other decimal
            temp = (temp * (int)pow(10,pot)) + (nc - 48);
            if(pot == 0){
                pot = 1;
            }
        }
        if (c == ','){
            (*numVars) = temp;
            temp = 0;
            pot = 0;
        }
    } 
    (*numRest) = temp;
    rewind(fp);
}
void read_data(FILE* fp,double *COEF,double *P,double *Z){
    char c;
    int nc,numVars,numRest;
    int temp = 0;
    int pot = 0;
    int index = 0;
    //read vars & rest
    for (c = getc(fp); c != '\n'; c = getc(fp)){
        nc = (int)c;
        if(nc >= 48 && nc <= 57){
            //add other decimal
            temp = (temp * (int)pow(10,pot)) + (nc - 48);
            if(pot == 0){
                pot = 1;
            }
        }
        if (c == ','){
            numVars = temp;
            temp = 0;
            pot = 0;
        }
    }
    numRest = temp;
    temp = 0;
    pot = 0;
    //read the COEF
    for (c = getc(fp); index < numRest*numVars; c = getc(fp)){
        nc = (int)c;
        if (c == ',' || c == '\n') {
            //save & restart
            COEF[index] = (double)temp;
            temp = 0;
            pot = 0;
            index++;
        }else if(nc >= 48 && nc <= 57){
            //add other decimal
            temp = (temp * (int)pow(10,pot)) + (nc - 48);
            if(pot == 0){
                pot = 1;
            }
        }
    }
    //read P
    nc = (int)c;
    index = 0;
    temp = nc - 48;
    pot = 1;
    //read the COEF
    for (c = getc(fp); index < numRest; c = getc(fp)){
        nc = (int)c;
        if (c == ',' || c == '\n') {
            //save & restart
            P[index] = (double)temp;
            temp = 0;
            pot = 0;
            index++;
        }else if(nc >= 48 && nc <= 57){
            //add other decimal
            temp = (temp * (int)pow(10,pot)) + (nc - 48);
            if(pot == 0){
                pot = 1;
            }
        }
    }
    //read Z
    nc = (int)c;
    index = 0;
    temp = nc - 48;
    pot = 1;
    //read the COEF
    for (c = getc(fp); c!=EOF && index<numVars; c = getc(fp)){
        nc = (int)c;
        if (c == ',' || c == '\n') {
            //save & restart
            Z[index] = (double)temp;
            temp = 0;
            pot = 0;
            index++;
        }else if(nc >= 48 && nc <= 57){
            //add other decimal
            temp = (temp * (int)pow(10,pot)) + (nc - 48);
            if(pot == 0){
                pot = 1;
            }
        }
    }

    rewind(fp);
    return;
}
void print_matrix(double *data,int mRr, int mRc){
    int row, col;
    for (row=0; row<mRr; row++)
    {
        for(col=0; col<mRc; col++)
            {
             printf("%g", data[row * mRc + col]);
	     if(col != mRc - 1){
		printf(",");
	     }
            }
        if(row != mRr - 1){
            //if isn't the last line print return
            printf("\n");
        }
    }
    printf("\n");
    return;
}
void print_array(double *data,int len){
    int i;
    for (i=0; i<len; i++)
    {
        printf("%g", data[i]);
        if(i != len - 1){
            printf(",");
        }
    }
    printf("\n");
    return;
}
