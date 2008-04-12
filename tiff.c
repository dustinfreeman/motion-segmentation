/* tiff.c */
/*
 * Contains functions to read and write TIFF images
 * to files using libTIFF
 */

#include <tiffio.h>
#include <stdio.h>
#include <math.h>

#include "math.h"
#include "globals.h"

/*
 * Sets the width and height global variables
 */
void setSizes(char* filename)
{
    //Get image dimensions from first image passed
    TIFF* tif;
    tif = TIFFOpen(filename, "r");
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
    npixels = w*h;
    TIFFClose(tif);
}
/*
 * Writes a file with the level set highlighted
 * in red to the Frames directory
 */
int writeFrame(int frameCount)
{
	char filename[23];
	sprintf(filename, "Frames/Frame%d.tif", frameCount);
	TIFF* out = TIFFOpen(filename,"w");
	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, w);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, h);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
	TIFFSetField(out, TIFFTAG_ROWSPERSTRIP,
	    TIFFDefaultStripSize(out, 1));

	uint32* output;
	output = (uint32*)_TIFFmalloc(sizeof(uint32)*w);
	int row,x;
	int intensity;
	float uxr, uxr1, ux1r;
	for (row = 0; row < h; row++) {
	    for (x = 0; x < w; x++) {
		    uxr = u(0,x,row);
		    uxr1 = u(0,x,row+1);
		    ux1r = u(0,x+1,row);
			if (u(0,x,row) <= 0.0) {
				//if ((uxr < 0 && ux1r >= 0) || (uxr > 0 && ux1r <=0) ||
				//	(uxr < 0 && uxr1 >=0) || (uxr > 0 && uxr1 <=0)) {
				    output[x] = (uint32) (0xff0000ff);
				}
				else {
				    intensity = (int) (I(0,x,row));
				    output[x] = (uint32) (0xff*pow(2,24) + 
				    	intensity*pow(2,16) + intensity*pow(2,8) + intensity);
				}	
	    }
	    TIFFWriteScanline(out, output, h-1-row, 0);
	}

	_TIFFfree(output);
	TIFFClose(out);
	return(0);
}   
/*
 * We're going to read in the requested TIFF file
 * into an array of intensity values.  To do this, we use
 * libtiff to read in an array of ARGB values, and 
 * convert this to Y values.
 */
int readTiff(char* filename, int imageCount)
{
    uint32* raster;
    int i;
    
    TIFF* tif;
    tif = TIFFOpen(filename, "r");

    uint32 width, height;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    if (width != w || height != h) {
	printf("ERROR::You have read in multiple TIFF files \
		with different dimensions!\n");
	return (1);
    }
    raster = (uint32*) _TIFFmalloc(npixels * sizeof (uint32));
    if (raster != NULL) {
        if (TIFFReadRGBAImage(tif, w, h, raster, 0)) {
            for (i = 0 ; i < npixels ; i++) {
	        *(imagePtr+imageCount*npixels+i) = 
				(int)(ycbcrCoeffs[0]*(int)(TIFFGetR(raster[i]))) + 
				(int)(ycbcrCoeffs[1]*(int)(TIFFGetG(raster[i]))) + 
				(int)(ycbcrCoeffs[2]*(int)(TIFFGetB(raster[i])));
	    }
	}
	else {
	    printf("ERROR::TIFFReadRGBAImage Failure!\n");
	}
	TIFFClose(tif);
	_TIFFfree(raster);
    }
    return (0);
}
/*
 * Writes a file with the level set highlighted
 * in red to the Frames directory
 */
int outputMotionTif(int i)
{
	char filename[30];
	sprintf(filename, "Motion/Motion%04d.tif", i);
	TIFF* out = TIFFOpen(filename,"w");
	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, w);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, h);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
	TIFFSetField(out, TIFFTAG_ROWSPERSTRIP,
	    TIFFDefaultStripSize(out, 1));

	uint32* output;
	output = (uint32*)_TIFFmalloc(sizeof(uint32)*w);
	int row,x,pix;
	float val;
	for (row = 0; row < h; row++) {
	    for (x = 0; x < w; x++) {
			val = v(x,row);
			if (val > 0) {
				val = val + 0;
			}
			pix = (int)(val*(255/maxFlow));
		    output[x] = (uint32) (0xff*pow(2,24) + 
		    	pix*pow(2,16) + pix*pow(2,8) + pix);
	    }
	    TIFFWriteScanline(out, output, h-1-row, 0);
	}

	_TIFFfree(output);
	TIFFClose(out);
	return(0);
}   
/*
 * We're going to read in the requested TIFF file
 * into an array of intensity values.  To do this, we use
 * libtiff to read in an array of ARGB values, and 
 * convert this to Y values.
 */
int genLevelSet(char* filename)
{
    uint32* raster;
    int i;
    
    TIFF* tif;
    tif = TIFFOpen(filename, "r");

    uint32 width, height;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    raster = (uint32*) _TIFFmalloc(width * height * sizeof (uint32));
	for (i = 0; i < npixels ; i++) {
		*(lsetPtr+i) = -10;
	}
    if (raster != NULL) {
        if (TIFFReadRGBAImage(tif, w, h, raster, 0)) {
            for (i = 0 ; i < width*height ; i++) {
				if ((int)(ycbcrCoeffs[0]*(int)(TIFFGetR(raster[i]))) + 
					(int)(ycbcrCoeffs[1]*(int)(TIFFGetG(raster[i]))) + 
					(int)(ycbcrCoeffs[2]*(int)(TIFFGetB(raster[i]))) > 0) {
					*(lsetPtr+i) = 10;
				}
				else *(lsetPtr+i) = -10;
			}
		}
	}
	else {
	    printf("ERROR::TIFFReadRGBAImage Failure!\n");
	}
	TIFFClose(tif);
	_TIFFfree(raster);
    return (0);
}