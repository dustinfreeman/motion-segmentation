/* globals.c */
/*
 * This file simply defines globals variables used
 * throughout the program
 */
#include "globals.h"
 
#define BINSARR 1000
//Constants
const int A = 5;
const int B = 250;
//LAMBDA2 affects curvature, 
//while LAMBDA1 affects the functional
const float LAMBDA1 = 10000000.0;
const float LAMBDA2 = 0.00001;
//const float LAMBDA2 = 0.0;
const float DELTAT = 0.1;
const int BINS = BINSARR;
// Standard definition television standard for Y'CbCr
float	ycbcrCoeffs[3] = { .299, .587, .114 };
// Variables to hold the image size
unsigned int w = 0, h = 0;
int npixels = 0;
// Pointer to our arrays
int *imagePtr;
float *motionPtr;
float *lsetPtr;
// Maximum value of Optic Flow
float maxFlow;
// Items constant per iteration
float term1, term2;
// Histogram arrays
int NumInR[BINSARR];
int NumInRC[BINSARR];