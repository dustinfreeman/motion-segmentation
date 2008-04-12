/* globals.h */
#ifndef GLOBAL_H
#define GLOBAL_H

extern const int A;
extern const int B;
extern const float LAMBDA1;
extern const float LAMBDA2;
extern const float DELTAT;
extern const int BINS;
extern float	ycbcrCoeffs[3];
extern unsigned int w, h;
extern int npixels;
extern int *imagePtr;
extern float *motionPtr;
extern float *lsetPtr;
extern float maxFlow;
extern float term1, term2;
extern int NumInR[];
extern int NumInRC[];

#endif
