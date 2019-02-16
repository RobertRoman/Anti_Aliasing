#include "anti_aliasing.h"
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int num_threads;
int resize_factor;

int main(int argc, char * argv[]) {
	//agrv[1] -> input file
	//argv[2] -> output file name
	//argv[3] -> resize factor
	//argv[4] -> number of threads
	if(argc < 5) {
		printf("Incorrect number of arguments!\n");
		exit(-1);
	}

	resize_factor = atoi(argv[3]);
	num_threads = atoi(argv[4]);

	image input;
	image output;

	readInput(argv[1], &input);

	resize(&input, &output);

	writeData(argv[2], &output);

	return 0;
}
