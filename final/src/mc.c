#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void read_header(FILE* fp,int *numVars,int *numRest);
void read_data(FILE* fp,double *COEF,double *P,double *Z);
void print_matrix(double *data,int mr, int mc);
void print_array(double *data,int len);

/*
__global__ void matrix_multiplication(double *COEF,double P,double Z,int numRest, int numVars)
{ 
    int row = blockIdx.y * blockDim.y + threadIdx.y; 
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;
    int i = 0;
    if( col < m2c && row < m1r) 
    {
        for(i = 0; i < m1c; i++) 
        {
            sum += m1[row * m1c + i] * m2[i * m2c + col];
        }
        mR[row * m2c + col] = sum;
    }
} 
*/
int main(int argc, char **argv)
{
    if( 2 != argc ){
        printf("Incorrect number of params: %d\n",argc - 1);
        return 1;
    }
    //Vars Declaration
    int numRest,numVars;
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
    cpu_COEF = (double*)malloc(numVars*numRest*sizeof(double));
    cpu_P = (double*)malloc(numRest*sizeof(double));
    cpu_Z = (double*)malloc(numVars*sizeof(double));
    cpu_C = (double*)calloc(numVars,sizeof(double));
    cpu_CANS = (double*)calloc(numVars,sizeof(double));


/*
    err = cudaMalloc((void**)&gpu_m1,m1s);
    if(err != cudaSuccess){printf("Error with Matrix 1\n");exit(1);}
    err = cudaMalloc((void**)&gpu_m2,m2s);
    if(err != cudaSuccess){printf("Error with Matrix 2\n");exit(1);}
    err = cudaMalloc((void**)&gpu_mR,mRs);
    if(err != cudaSuccess){printf("Error with Matrix R\n");exit(1);}
*/
    //Read the Files to the CPU memory
    read_data(fp,cpu_COEF,cpu_P,cpu_Z);
    //Now the files can be closed
    fclose(fp);
/*
    //Copy each matrix to the device
    err = cudaMemcpy(gpu_m1, cpu_m1, m1s, cudaMemcpyHostToDevice);
    if(err != cudaSuccess){printf("Error Coping Matrix 1\n");exit(1);}
    err = cudaMemcpy(gpu_m2, cpu_m2, m2s, cudaMemcpyHostToDevice);
    if(err != cudaSuccess){printf("Error Coping Matrix 2\n");exit(1);}

    //Execute the Kernel in the Device
    int tile_dim = 32;
    dim3 block_dim(tile_dim,tile_dim,1);
    dim3 grid_dim(ceil(mRc/float(tile_dim)),ceil(mRr/float(tile_dim)),1);

    matrix_multiplication<<<grid_dim,block_dim>>>(gpu_m1, gpu_m2, gpu_mR, m1r, m1c, m2c);

    //copy the result to Host mem
    err = cudaMemcpy(cpu_mR,gpu_mR,mRs, cudaMemcpyDeviceToHost);
    if(err != cudaSuccess){printf("Error Coping Matrix R\n");exit(1);}

    */
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
