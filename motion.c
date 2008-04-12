/* motion.c */
/*
 * Functions related to our motion characteristics
 */

#include <math.h>
#include <stdio.h>

#include "motion.h"
#include "math.h"
#include "globals.h"

#define BIN_SIZE 2
#define BIN_SIZE2 4
#define NU2 0.5
#define EPSILON 0.0000001
/*
 * Motion related characteristic from 
 * Fablet and Bouthemy
 */
float calcV(int x, int y)
{
    int i, j;
    float num = 0;
    float den = 0;
	float val;
    for (i=x-2; i<x+3; i++) {
        for (j=y-2; j<y+3; j++) {
            val = (float)(delI(0, i, j)) * (float)(I_t(0, i, j));
			num = num + val;
            den = den + (float)(pow(delI(0, i, j), 2));
        }
    }
    if (den > NU2) {
        return (num / den);
    }
    else {
        return (num / NU2);
    }
}
/*
 * Generate all the motion characteristics
 */
void genMotionChar()
{
	int i, j;
	float max, val;
	max = 0;
	for (i=0; i<w; i++) {
		for (j=0; j<h; j++) {
			val = calcV(i, j);
			*(motionPtr+i+j*w) = val;
			if (val > max) {
				max = val;
			}
		}
	}
	//set the maximum in order to create the histograms
	maxFlow = max; 
}
/*
 * Build arrays of counts for each bin
 */
void buildHistograms() 
{
	int i,j,Vindex;
	float val;

	for (i=0; i<BINS; i++) {
		NumInR[i] = 0;
		NumInRC[i] = 0;
	}
	for (i=0; i<w; i++) {
		for (j=0; j<h; j++) {
			val = *(motionPtr+j*w+i);
			Vindex = (int) (val*(BINS-1)/maxFlow);
			if ( u(0,i,j) >0 ) { //if we're inside the region R
				NumInR[Vindex] += 1;
			}
			else { //we're in R^C
				NumInRC[Vindex] += 1;
			}
		}
	}
}
/*
 * Print the histograms to a .csv file that can be opened in
 * Excel
 */
int printHistograms(int num) {
	int i;
	float AR, ARC;
	FILE* outFile;
	char filename[30];
	sprintf(filename, "Histograms/histograms%04d.csv",num);
	outFile = fopen(filename, "w");
	fprintf(outFile, "Z value,NumInR,NumInRC\n");
	AR = (float)A_R();
	ARC = (float)(npixels-AR);
	for (i=0;i<BINS;i++) {
		fprintf(outFile, "%d,%f,%f\n",i,((float)NumInR[i]/AR)*100,((float)NumInRC[i]/ARC)*100);
	}
	fclose(outFile);
	return 0;
}
/*
 * Count the size of the region
 */
int A_R()
{
    int i, j, count=0;
    for (i=0; i<w; i++) {
        for (j=0; j<h; j++) {
            if (u(0,i,j) > 0) {
				count++;
            }
        }
    }
    return count;
}

/*
 * Integral term in flow equation
 */
float flowInt()
{
	int i;
	float NR, NRC;
	float sum = 0;
	for (i=0; i<BINS; i++)
	{
		NR = (float)NumInR[i];
		NRC = (float)NumInRC[i];
		sum = sum + (float)(NR * logn(NRC,NR));
	}
	return sum;
}
