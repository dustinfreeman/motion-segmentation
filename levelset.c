/* levelset.c */
/*
 * This file contains methods for creating and
 * updating the level set of the curve representing
 * the image segmentation
 */
#include <pthread.h>
#include <stdio.h>

#include "math.h"
#include "tiff.h"
#include "globals.h"

#define NUM_THREADS  2

//Prototype for thread
void *updateUThread(void *arg);
/*
 * Thread_data structure, contains information 
 * that each thread needs. The thread_data_array
 * contains multiple thread_data structs.
 */
struct thread_data{
    int row_start;
    int row_end;
};
struct thread_data thread_data_array[NUM_THREADS];
/*
 * Multithreaded function to update the u array
 */
int updateU()
{
	// Now create the threads and do the real computation
    pthread_t threads[NUM_THREADS];
    int t;
    //Create threads
    for (t = 0; t < NUM_THREADS-1; t++) {
	thread_data_array[t].row_start = t*(h/NUM_THREADS);
	thread_data_array[t].row_end = (t+1)*(h/NUM_THREADS);
	pthread_create(&threads[t],NULL,updateUThread,\
		(void *)&thread_data_array[t]);
    }
    //The last thread must go all the way to last row
    t=NUM_THREADS-1;
    thread_data_array[t].row_start = t*(h/NUM_THREADS);
    thread_data_array[t].row_end = h;
    pthread_create(&threads[t],NULL,updateUThread,\
    	(void *)&thread_data_array[t]);

    //Wait for threads to finish before continuing
    for (t = 0; t < NUM_THREADS; t++) {
	pthread_join(threads[t],NULL);
    }
    return(0);
}
/*
 * Each thread runs this function
 * needs to be passed an array containing the
 * start line, and the end line
 */
void *updateUThread(void *arg)
{	
	int start, end;
    struct thread_data *my_data;
    my_data = (struct thread_data *) arg;
    start = my_data->row_start;
    end = my_data->row_end;
    float f,max,min;
    int i,j;
    for (j = start; j < end; j++) {
		for (i = 0; i < w; i++) {
			//Here we selecst the max and min values
			//max represents max(F,0)
			//min represents min(F,0)
			f = G(0,i,j);
			if (f<0) {
			max = 0;
			min = f;
			}
			else {
			min = 0;
			max = f;
			}
			/* This is the formula from Mansouri's IEEE paper,
			 * it depends on the curvature, so all that
			 * needs to be changed for different
			 * functionals is F */
			*(lsetPtr+j*w+i+npixels) = (u(0,i,j) +
					DELTAT*(LAMBDA1*(max*delpos(0,i,j) + min*delneg(0,i,j)) + 
					LAMBDA2*K(0,i,j)*del(0,i,j)));
			//Prevent overflow, as values quickly become too large
			if (*(lsetPtr+j*w+i+npixels) < -10000)
			*(lsetPtr+j*w+i+npixels) = -10000;
			if (*(lsetPtr+j*w+i+npixels) > 10000)
			*(lsetPtr+j*w+i+npixels) = 10000;
		}
    }
    return(0);
}
/*
 * Move U1 back to U0 so we can calculate a new
 * U1 based on the last iteration
 */
int copyU()
{
    int i,j;
    for (j = 0; j < h; j++) {
	for (i = 0; i < w; i++) {
	    *(lsetPtr+j*w+i) = *(lsetPtr+j*w+i+npixels);
	}
    }
    return(0);
}