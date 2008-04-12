/* main.c */
/* 
 * The main loop that controls the program
 * This program accepts an input of a series of TIFF files
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "tiff.h"
#include "globals.h"
#include "levelset.h"
#include "motion.h"
#include "math.h"

#define TIMESTEPS  500
#define OUTFREQ  2

int main(int argc, char* argv[])
{
    int i;
    //Read the size of the first image
    setSizes(argv[1]);

    //Create directories to hold the set of images
    mkdir("Frames",S_IRWXU);
	mkdir("Motion",S_IRWXU);
	mkdir("Histograms",S_IRWXU);

    //Allocate needed memory for image arrays
    imagePtr = malloc(npixels*(argc-2)*sizeof(unsigned int));
    //Allocate memory to hold the level set of the curve
    lsetPtr = malloc(npixels*sizeof(float)*2);
	//Allocate memory to hold motion characteristic
	motionPtr = malloc(npixels*sizeof(float));
	//Make sure they were properly allocated
    if (imagePtr == NULL || motionPtr == NULL || lsetPtr == NULL) {
	printf("ERROR::Not enough memory!\n");
	return(0);
    }
	
	//Generate the initial level set from the first argument passed
	printf("Generating level set from %s\n\n", argv[1]);
	genLevelSet(argv[1]);
	
	//Read each image passed into the imagePtr array
    for (i = 0; i < argc-2; i++) {
		printf("Reading in image %s\n", argv[i+2]);
	    if (readTiff(argv[i+2],i) == 1) {
			return (0);
	    }
    }
	printf("\n");

	//Calculate motion characteristic
	genMotionChar();
	outputMotionTif(0);
	
    //Timesteps loop
    int k;
    i = 0;
    for (k = 0; k < TIMESTEPS; k++) {
		//Build the histograms
		buildHistograms();
		//Calculate the constant per iteration terms
		newGIter();
		//Update level set
		updateU();
		
		if (k%OUTFREQ == 0) {
			i++; //This is the count used in the image filename
			//Output an image
			if (writeFrame(i) != 0 ) {
				printf("Error writing file to disk, program aborted.\n");
				return(-1);
			}
			//Write the histogram to disk
			printHistograms(i);
			//Write progress to terminal
			printf("\r \r");
			printf("%3d Percent Remaining",100 - 100*k/TIMESTEPS);
			fflush(stdout);
		}
		//Copy back
		copyU();
    }
	printf("\n");
    //Make sure the memory is freed
    free(imagePtr);
    free(lsetPtr);
	free(motionPtr);
    return (0);
}

