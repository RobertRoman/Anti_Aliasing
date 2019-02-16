#include "anti_aliasing.h"

int num_threads;
int resize_factor;

/* global pointers for the input and output image used
   for accessing the same memory from different threads */
image *input, *output;

// gaussian matrix for resize factor 3
int gauss[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};

void readInput(const char * fileName, image *img) {
	FILE *in;
	int i;

	if ((in = fopen(fileName, "rb")) == NULL) {
		return;
	}

	// reading a ".pnm" image
	fscanf(in, "%s\n", img->P);
	fscanf(in, "%d", &(img->width));
	fscanf(in, "%d\n", &(img->height));
	fscanf(in, "%hhu\n", &(img->maxval));

	// allocating memory for greyscale images ".pgm"
	if(!strcmp(img->P, "P5")) {
		aloc_mem_grey(img);
		for(i = 0; i < img->height; i++) {
			fread((img->grey[i]), 1, img->width, in);
		}
	// allocating memory for color imagages ".png"
	} else {
		aloc_mem(img);
		for(i = 0; i < img->height; i++) {
			fread((img->pic[i]), 1, 3*img->width, in);			
		}		
	}

	fclose(in);
}

void writeData(const char * fileName, image *img) {
	FILE *out;
	int i;

	out = fopen(fileName, "wb");
	if(!out)
		return;

	// writing a ".pnm" image
	fprintf(out, "%s\n", img->P);
	fprintf(out, "%d %d\n%d\n", img->width, img->height, img->maxval);

	// writing a greyscale image ".pgm" 
	if(!strcmp(img->P, "P5")) {
		for(i = 0; i < img->height; i++) {
			fwrite((img->grey[i]), 1, img->width, out);
		}
		free_mem_grey(img);
	// writing a color image ".png"
	} else {
		for(i = 0; i < img->height; i++) {
			fwrite((img->pic[i]), 1, 3*img->width, out);
		}
		free_mem(img);		
	}

	fclose(out);
}


void* threadFunction(void *var) {
	int i, j, k, l, sum_r, sum_g, sum_b, sum_grey, m1, m2;
	thread th = *(thread *)var;

	// greyscale image
	if(!strcmp(input->P, "P5")) {
		// even resize factor
		if(resize_factor != 3) {
			// iterate output image
			for(i = th.start; i <= th.stop; i++) {
				for(j = 0; j < output->width; j++) {
					sum_grey = 0;
					// iterate input matrix 
					for(k = i * resize_factor; k < (i + 1) * resize_factor; k++) {
						for(l = j * resize_factor; l < (j + 1) * resize_factor; l++) {
							sum_grey += input->grey[k][l];
						}
					}
					sum_grey /= (resize_factor * resize_factor);
					output->grey[i][j] = sum_grey;
				}
			}
		//resize factor 3
		} else {
			// iterate output image
			for(i = th.start; i <= th.stop; i++) {
				for(j = 0; j < output->width; j++) {
					sum_grey = 0;
					// iterate input matrix 
					for(k = i * resize_factor; k < (i + 1) * resize_factor; k++) {
						for(l = j * resize_factor; l < (j + 1) * resize_factor; l++) {
							m1 = k % resize_factor;
							m2 = l % resize_factor;
							sum_grey += input->grey[k][l] * gauss[m1][m2];
						}
					}
					sum_grey /= 16;
					output->grey[i][j] = sum_grey;
				}
			}					
		}
	// color image
	} else {
		// even resize factor
		if(resize_factor != 3) {
			// iterate output image
			for(i = th.start; i <= th.stop; i++) {
				for(j = 0; j < output->width; j++) {
					sum_r = sum_g = sum_b = 0;
					// iterate input matrix 
					for(k = i * resize_factor; k < (i + 1) * resize_factor; k++) {
						for(l = j * resize_factor; l < (j + 1) * resize_factor; l++) {
							sum_r += input->pic[k][l].r;
							sum_g += input->pic[k][l].g;
							sum_b += input->pic[k][l].b;
						}
					}
					sum_r /= (resize_factor * resize_factor);
					sum_g /= (resize_factor * resize_factor);
					sum_b /= (resize_factor * resize_factor);
					output->pic[i][j].r = sum_r;
					output->pic[i][j].g = sum_g;
					output->pic[i][j].b = sum_b;
				}
			}
		// resize factor 3
		} else {
			// iterate output image
			for(i = th.start; i <= th.stop; i++) {
				for(j = 0; j < output->width; j++) {
					sum_r = sum_g = sum_b = 0;
					// iterate input matrix 
					for(k = i * resize_factor; k < (i + 1) * resize_factor; k++) {
						for(l = j * resize_factor; l < (j + 1) * resize_factor; l++) {
							m1 = k % resize_factor;
							m2 = l % resize_factor;
							sum_r += input->pic[k][l].r * gauss[m1][m2];
							sum_g += input->pic[k][l].g * gauss[m1][m2];
							sum_b += input->pic[k][l].b * gauss[m1][m2];
						}
					}
					sum_r /= 16;
					sum_g /= 16;
					sum_b /= 16;
					output->pic[i][j].r = sum_r;
					output->pic[i][j].g = sum_g;
					output->pic[i][j].b = sum_b;
				}
			}	
		}
	}

	return NULL;
}

void resize(image *in, image * out) {
	int i;
	
	// resolving the header of the ".pnm" image
	memcpy(out->P, in->P, 3);
	out->width = in->width / resize_factor;
	out->height = in->height / resize_factor;
	out->maxval = in->maxval;

	if(!strcmp(in->P, "P5")) 
		aloc_mem_grey(out);
	else
		aloc_mem(out);

	pthread_t tid[num_threads];
	thread th[num_threads];

	// settling the rows each thread processes
	for(i = 0;i < num_threads; i++) {
		th[i].id = i;
		th[i].start = (out->height / num_threads) * i;
		if(i != num_threads - 1) {
			th[i].stop = (out->height / num_threads) * (i+1) - 1;
		} else {
			th[i].stop = out->height - 1;
		}
	}

	input = in;
	output = out;

	for(i = 0; i < num_threads; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, &(th[i]));
	}

	for(i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}

	if(!strcmp(input->P, "P5")) 
		free_mem_grey(in);
	else
		free_mem(in);
}

void aloc_mem(image *img) {
	int i;
	img->pic = (pixel **) malloc(img->height * sizeof(pixel *));
	for(i = 0; i < img->height; i++) {
		img->pic[i] = (pixel *) malloc(img->width*sizeof(pixel));
	}
}

void aloc_mem_grey(image *img) {
	int i;
	img->grey = (unsigned char **) malloc(img->height * sizeof(unsigned char *));
	for(i = 0; i < img->height; i++) {
		img->grey[i] = (unsigned char *) malloc(img->width*sizeof(unsigned char));
	}
}

void free_mem(image *img) {
	int i;
	for(i = 0; i < img->height; i++) {
		free(img->pic[i]);
	}
	free(img->pic);
}

void free_mem_grey(image *img) {
	int i;
	for(i = 0; i < img->height; i++) {
		free(img->grey[i]);
	}
	free(img->grey);
}
