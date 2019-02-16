#ifndef ANTI_ALIASING_H
#define ANTI_ALIASING_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

extern int num_threads;
extern int resize_factor;

typedef struct {
	unsigned char r, g, b;
}pixel;

// adjuvant structure for images
typedef struct {
	char P[3];
	int width;
	int height;
	unsigned char maxval;
	pixel **pic;				// color
	unsigned char **grey;		// greyscale
}image;

// adjuvant structure for threads
typedef struct {
	int id, start, stop;
}thread;

void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image * out);

void free_mem(image *img);
void free_mem_grey(image *img);

void aloc_mem(image *img);
void aloc_mem_grey(image *img);
#endif