/* tiff.h */

#ifndef TIFF_H
#define TIFF_H

int setSizes(char*);
int readTiff(char*, int);
int writeFrame(int);
int outputMotionTif(int);
void genLevelSet(char*);

#endif
