#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void array_addition(float *destiny,
                    float *summand_a, float *summand_b, int size);
void matrix_multiplication(float **destiny,
                           float **multiplicand, float **multiplier,
                           int rows_multiplicand, int cols_multiplicand,
                           int rows_multiplier, int cols_multiplier);
void matrix_print( float **matrix ,int rows ,int cols  );
void matrix_allocate( float ***matrix, int rows ,int cols );
void matrix_free( float **matrix, int rows);
void matrix_fill(float **matrix ,int rows ,int cols);
void array_fill(float *array,int lenght);
void array_print( float *array ,int lenght);
void array_allocate( float **array, int lenght );
void array_free( float *array);
void matrix_write( float **matrix ,int rows ,int cols, char *filename);

/**
 * This function make the addition and multiplication of each value in two
 * arrays of the determinated size.
 *
 * @param {integer}        lenght                The lenght of the arrays
 * @param {integer}        rows_multiplicand    The number of rows
 * @param {integer}        cols_multiplicand    The nember of columns
 * @param {integer}        rows_multiplier        The number of rows
 * @param {integer}        cols_multiplier        The nember of columns
 *
 * @return {integer}    Status code of the execution
 */

int main(int argc, char *argv[]){
    if(6 != argc){
        printf("incorrect number of parameters\n");
        printf("insert: lenght,rows_multiplicand,cols_multiplicand,rows_multiplier,cols_multiplier");
        return 0;
    }
    int lenght = atoi(argv[1]);
    int rows_multiplicand = atoi(argv[2]);
    int cols_multiplicand = atoi(argv[3]);
    int rows_multiplier = atoi(argv[4]);
    int cols_multiplier = atoi(argv[5]);

    float *array_destiny;
    float *summand_a;
    float *summand_b;

    float **matrix_destiny;
    float **multiplicand;
    float **multiplier;

    matrix_allocate( &matrix_destiny, rows_multiplicand, cols_multiplier );
    matrix_allocate( &multiplicand, rows_multiplicand, cols_multiplicand );
    matrix_allocate( &multiplier, rows_multiplier, cols_multiplier );

    array_allocate( &array_destiny, lenght );
    array_allocate( &summand_a, lenght );
    array_allocate( &summand_b, lenght );

    matrix_fill(multiplicand ,rows_multiplicand ,cols_multiplicand);
    matrix_fill(multiplier ,rows_multiplier ,cols_multiplier);
    array_fill(summand_a, lenght);
    array_fill(summand_b, lenght);

    array_addition(array_destiny,summand_a, summand_b, lenght);
    matrix_multiplication(matrix_destiny,multiplicand, multiplier,rows_multiplicand, cols_multiplicand,rows_multiplier, cols_multiplier);
    
    array_print( array_destiny , lenght);
    
    matrix_write( matrix_destiny , rows_multiplicand , cols_multiplier, "matrix_destiny.csv");
    matrix_write( multiplicand , rows_multiplicand , cols_multiplicand, "multiplicand.csv");
    matrix_write( multiplier , rows_multiplier , cols_multiplier, "multiplier.csv");

    matrix_free( matrix_destiny, rows_multiplicand);
    matrix_free( multiplicand, rows_multiplicand);
    matrix_free( multiplier, rows_multiplier);

    array_free( array_destiny);
    array_free( summand_a);
    array_free( summand_b);
}

void matrix_print( float **matrix ,int rows ,int cols  )
{
    int i,j;
    for(i=0; i<rows; ++i)
        for(j=0; j<cols; ++j)
        {
            printf("%f ", matrix[i][j]);
            if(j == cols-1)
                printf("\n");
        }
}

void matrix_allocate( float ***matrix, int rows ,int cols )
{
int i;
*matrix = (float **)malloc(rows * sizeof(float*));
for(i = 0; i < rows; i++) (*matrix)[i] = (float *)malloc(cols* sizeof(float));
}

void matrix_free( float **matrix, int rows)
{
int i;
for(i = 0; i < rows; i++) free(matrix[i]);
free(matrix);
}

void array_allocate( float **array, int lenght )
{
    (*array) = (float *)malloc(lenght* sizeof(float));
}

void array_free( float *array)
{
free(array);
}

void matrix_fill(float **matrix ,int rows ,int cols)
{
    int i,j;
    for (i=0; i<rows; i++)
    {
        for (j=0; j<cols; j++)
        {
            matrix[i][j]= rand()/(float) RAND_MAX;
        }
    }
}

void array_fill(float *array,int lenght)
{
    int i;
    for (i=0; i<lenght; i++)
    {
        array[i]= rand()/(float) RAND_MAX;
    }
}

void array_print( float *array ,int lenght)
{
    int i;
    for(i=0; i<lenght; ++i)
    {
        printf("%f ", array[i]);
    }
    printf("\n");
}

void matrix_multiplication(float **destiny,
                           float **multiplicand, float **multiplier,
                           int rows_multiplicand, int cols_multiplicand,
                           int rows_multiplier, int cols_multiplier)
{
    int i,j,k;
    if (cols_multiplicand != rows_multiplier){
        printf("The matrices can't be multiplied.\n");
        return;
    }
    for(i=0; i<rows_multiplicand; ++i)
            for(j=0; j<cols_multiplier; ++j)
                for(k=0; k<cols_multiplicand; ++k)
                    destiny[i][j]+=multiplicand[i][k]*multiplier[k][j];
    return;
}

void array_addition(float *destiny,
                    float *summand_a, float *summand_b, int size)
{
    int i;
    for (i = 0; i < size; ++i)
    {
        destiny[i] = summand_a[i] + summand_b[i];
    }
}

void matrix_write( float **matrix ,int rows ,int cols, char *filename )
{
    int i,j;
    FILE *f;
    f = fopen(filename, "w+");

    for(i=0; i<rows; ++i)
        for(j=0; j<cols; ++j)
        {
            fprintf(f,"%f", matrix[i][j]);
            if(j == cols-1)
                fprintf(f,"\n");
            else
                fprintf(f,",");
        }
    fclose(f); 
}