#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if( 3 != argc ){
		printf("Incorrect number of params: %i\n",argc - 1)
		printf("Please include the filename of the two files\n");
		return 1;
	}

	printf("first file: %s\n", argv[1]);
	printf("second file: %s\n", argv[2]);
 
}