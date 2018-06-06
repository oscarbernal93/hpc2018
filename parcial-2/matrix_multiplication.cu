#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int row_counter(FILE* fp);
int col_counter(FILE* fp);
void read_matrix(FILE* fp,int *data);
void print_matrix(int *data,int mRr, int mRc);

__global__ void matrix_multiplication(int *m1,int *m2, int *mR, int m1r, int m1c, int m2c)
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

int main(int argc, char **argv)
{
    if( 3 != argc ){
        printf("Incorrect number of params: %d\n",argc - 1);
        return 1;
    }
    //Vars Declaration
    int m1r,m1c,m2r,m2c,mRr,mRc; //rows & cols
    int m1s,m2s,mRs;  //size
    int *gpu_m1, *cpu_m1;
    int *gpu_m2, *cpu_m2;
    int *gpu_mR, *cpu_mR;
    cudaError_t err = cudaSuccess;
	// Open the files
    FILE* fp1 = fopen(argv[1], "r");
    FILE* fp2 = fopen(argv[2], "r");
    // Check if files exists
    if (fp1 == NULL)
    {
        printf("Could not open file %s",argv[1] );
        return 1;
    }
    if (fp2 == NULL)
    {
        printf("Could not open file %s",argv[2] );
        return 1;
    }
    //Read the number of rows and cols of each matrix
    m1r = row_counter(fp1);
    m1c = col_counter(fp1);
    m2r = row_counter(fp2);
    m2c = col_counter(fp2);
    mRr = m1r;
    mRc = m2c;
    //printf("Matrix 1: %dx%d\n", m1r,m1c);
    //printf("Matrix 2: %dx%d\n", m2r,m2c);
	
    //condition: the number of columns in A must equal the number of rows in B
    if(m1c != m2r){
        printf("Incorrect Matrix Size, cols of Matrix 1 (%d) are different of the rows of Matrix 2 (%d)\n",m1c,m2r );
        return 1;
    }

    //reserve memory to each matrix
    m1s = m1r*m1c*sizeof(int);
    m2s = m2r*m2c*sizeof(int);
    mRs = mRr*mRc*sizeof(int);
    cpu_m1 = (int*)malloc(m1s);
    cpu_m2 = (int*)malloc(m2s);
    cpu_mR = (int*)malloc(mRs);
    err = cudaMalloc((void**)&gpu_m1,m1s);
    if(err != cudaSuccess){printf("Error with Matrix 1\n");exit(1);}
    err = cudaMalloc((void**)&gpu_m2,m2s);
    if(err != cudaSuccess){printf("Error with Matrix 2\n");exit(1);}
    err = cudaMalloc((void**)&gpu_mR,mRs);
    if(err != cudaSuccess){printf("Error with Matrix R\n");exit(1);}

    //Read the Files to the CPU memory
    read_matrix(fp1,cpu_m1);
    read_matrix(fp2,cpu_m2);
    //Now the files can be closed
    fclose(fp1);
	fclose(fp2);

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

    print_matrix(cpu_mR, mRr, mRc);

}

int row_counter(FILE* fp){
    char c;
	int count = 1;
	for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n') 
            count = count + 1;
	rewind(fp);
	return count;
}
int col_counter(FILE* fp){
    char c;
    int count = 1;
    for (c = getc(fp); c != '\n'; c = getc(fp))
        if (c == ',') 
            count = count + 1;
    rewind(fp);
    return count;
}
void read_matrix(FILE* fp,int *data){
    char c;
    int nc;
    int temp = 0;
    int pot = 0;
    int index = 0;
    for (c = getc(fp); c != EOF; c = getc(fp)){
        nc = (int)c;
        if (c == ',' || c == '\n') {
            //save & restart
            data[index] = (int)temp;
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
void print_matrix(int *data,int mRr, int mRc){
    int row, col;
    for (row=0; row<mRr; row++)
    {
        for(col=0; col<mRc; col++)
            {
             printf("%d", data[row * mRc + col]);
	     if(col != mRc - 1){
		printf(",");
	     }
            }
        if(row != mRr - 1){
            //if isn't the last line print return
            printf("\n");
        }
    }
    return;
}
