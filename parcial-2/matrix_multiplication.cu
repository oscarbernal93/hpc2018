#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int row_counter(FILE* fp);
int col_counter(FILE* fp);
void read_matrix(FILE* fp,unsigned int *data);

int main(int argc, char **argv)
{
    if( 3 != argc ){
        printf("Incorrect number of params: %d\n",argc - 1);
        return 1;
    }
    //Vars Declaration
    int m1r,m1c,m2r,m2c,mRr,mRc;
    unsigned int *gpu_m1, *cpu_m1;
    unsigned int *gpu_m2, *cpu_m2;
    unsigned int *gpu_mR, *cpu_mR;
    /// cudaError_t err = cudaSuccess;
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
    printf("Matrix 1: %dx%d\n", m1r,m1c);
    printf("Matrix 2: %dx%d\n", m2r,m2c);
 	
    //condition: the number of columns in A must equal the number of rows in B
    if(m1c != m2r){
        printf("Incorrect Matrix Size, cols of Matrix 1 (%d) are different of the rows of Matrix 2 (%d)\n",m1c,m2r );
        return 1;
    }

    //reserve memory to each matrix
    cpu_m1 = (unsigned int*)malloc(m1r*m1c*sizeof(unsigned int));
    cpu_m2 = (unsigned int*)malloc(m2r*m2c*sizeof(unsigned int));
    cpu_mR = (unsigned int*)malloc(mRr*mRc*sizeof(unsigned int));
    /// err = cudaMalloc((void**)&gpu_m1,m1r*m1c*sizeof(unsigned int));
    /// if(err != cudaSuccess){printf("Error with Matrix 1\n");exit(1);}
    /// err = cudaMalloc((void**)&gpu_m2,m2r*m2c*sizeof(unsigned int));
    /// if(err != cudaSuccess){printf("Error with Matrix 2\n");exit(1);}
    /// err = cudaMalloc((void**)&gpu_mR,mRr*mRc*sizeof(unsigned int));
    /// if(err != cudaSuccess){printf("Error with Matrix R\n");exit(1);}

    //Read the Files to the CPU memory
    read_matrix(fp1,cpu_m1);
    read_matrix(fp2,cpu_m2);
    //Now the files can be closed
    fclose(fp1);
 	fclose(fp2);

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
void read_matrix(FILE* fp,unsigned int *data){
    char c;
    int nc;
    unsigned int temp = 0;
    int pot = 0;
    int index = 0;
    for (c = getc(fp); c != EOF; c = getc(fp)){
        nc = (int)c;
        if (c == ',' || c == '\n') {
            //save & restart
            data[index] = (unsigned int)temp;
            temp = 0;
            pot = 0;
            index++;
        }else if(nc >= 48 && nc <= 57){
            //add other decimal
            temp = (temp * (int)pow(10,pot)) + (nc - 48);
            if(pot == 0){
                pot = 1;
            }
            printf("=%d\n",temp );
        }
    }
    rewind(fp);
    return;
}