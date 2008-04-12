/* math.c */
/*
 * Various math functions used
 * All the derivatives are approximated using finite differences
 */

#include <stdio.h>
#include <math.h>

#include "math.h"
#include "globals.h"
#include "motion.h"

#define EPSILON 0.00000001
/*
 * This is the I function.  You pass it the number of the image,
 * as well as an x and y value, and it returns the intensity 
 * value between 0 and 1 for that point.
 */
int I(int imageCount, int x, int y)
{
	int intensity;
	// The image array is arranged in memory in an order that allows
	// us to use this pointer arithmetic to acces x and y points.
	intensity = *(imagePtr + imageCount*npixels + y*w + x);
	return (intensity);
}
/*
 * This is the temporal derivative of the I function.  From t to t+1.
 */
int I_t(int t, int x, int y)
{
	if ( x<=0 || x>=w-1 || y<=0 || y>=h-1) {
		return (0);
	}
    return ((int)fabs(I(t+1,x,y)-I(t,x,y)));
}
/*
 * First derivative of I wrt x
 */
int Ix(int t, int x, int y)
{
	int value;
	if (x<=0 || x>=w-1) {
	    value = 0;
	}
	else {
	    value = I(t, x+1, y) - I(t, x-1, y);
	}
	return (value/2);
}
/*
 * First derivative of u wrt y
 */
int Iy(int t, int x, int y)
{
	int value;
	if (y<=0 || y>=h-1) {
	    value = 0;
	}
	else {
	    value = I(t, x, y+1) - I(t, x, y-1);
	}
	return (value/2);
}
/*
 * F Function for contrast based recognition
 */
float F(int t, int x, int y)
{
    float value;
    int i;
    i = I(t,x,y);
    value = (float)(pow((i-A),2)-pow((i-B),2));
    return(value);
}
/*
 * G function for motion recognition
 */
float G(int t, int x, int y)
{
	float z, value;
	float NR, NRC;
	int Zval;
	
	z = v(x,y);
	Zval =  (int)(z*BINS/maxFlow);
	NR= (float)NumInR[Zval];
	NRC = (float)NumInRC[Zval];
	
	value = logn(NR,NRC);
	
	value = (term1 + term2 * value);
	return value;
}
/*
 * Calculates the part of our descent equation that is constant
 * per iteration
 */
void newGIter()
{
	int AR, ARC;
	float integral;
	integral = flowInt();
	AR = A_R();
	ARC = npixels- AR;

	//term1 is the constant terms in the formula
	term1 = 2 - (float)(npixels)/(float)(ARC) + (float)(integral)/(float)(AR);
	term1 = term1 / (float)(AR);
	//term2 is the term that the value calculated per point is multiplied with
	term2 = 1 / (float)(AR);
}
/*
 * A normalized log of a fraction function
 */
float logn(float a, float b)
{
	float value;
	if (a == 0) {
		value = log(EPSILON);
	}
	else if (b == 0) {
		value = log(a/EPSILON);
	}
	else {
		value = log(a/b);
	}
	return (value);
}
/*
 * Returns the value of the level set for a location
 */
float u(int t, int x, int y)
{
    int value;
    value = *(lsetPtr + x + y*w + t*npixels);
    return (value);
}
/*
 * Get motion characteristic for a point
 */
float v(int x, int y)
{
	float v;
	v = *(motionPtr+x+y*w);
	return(v);
}
/*
 * Gradient magnitude of intensity
 */
int delI(int t, int x, int y)
{
	int magnitude;
	magnitude = (int)sqrt(pow(Ix(t,x,y),2)+pow(Iy(t,x,y),2));
	return (magnitude);
}
/*
 * Gradient magnitude using ux and uy
 */
float del(int t, int x, int y)
{
	int magnitude;
	magnitude = (int)sqrt(pow(ux(t,x,y),2)+pow(uy(t,x,y),2));
	return (magnitude);
}
/*
 * Gradient magnitude from minus side
 */
float delneg(int t, int x, int y)
{
    float one, two, three, four;
    one = uxneg(t,x,y);
    two = uxpos(t,x,y);
    three = uyneg(t,x,y);
    four = uypos(t,x,y);
    if (one < 0)
	one = 0;
    if (two > 0)
	two = 0;
    if (three < 0)
	three = 0;
    if (four > 0)
	four = 0;
    return (sqrt(pow(one,2)+pow(two,2)+pow(three,2)+pow(four,2)));
}
/*
 * Gradient magnitude from plus side
 */
float delpos(int t, int x, int y)
{
    float one, two, three, four;
    one = uxneg(t,x,y);
    two = uxpos(t,x,y);
    three = uyneg(t,x,y);
    four = uypos(t,x,y);
    if (one > 0)
	one = 0;
    if (two < 0)
	two = 0;
    if (three > 0)
	three = 0;
    if (four < 0)
	four = 0;
    return (sqrt(pow(one,2)+pow(two,2)+pow(three,2)+pow(four,2)));
}
/*
 * First derivative of u wrt x
 */
float ux(int t, int x, int y)
{
	float value;
	if (x==0 || x==w-1) {
	    value = 0;
	}
	else {
	    value = u(t, x+1, y) - u(t, x-1, y);
	}
	return (value/2);
}
/*
 * First derivative of u wrt y
 */
float uy(int t, int x, int y)
{
	float value;
	if (y==0 || y==h-1) {
	    value = 0;
	}
	else {
	    value = u(t, x, y+1) - u(t, x, y-1);
	}
	return (value/2);
}
/*
 * First derivative of u wrt x (negative method)
 */
float uxneg(int t, int x, int y)
{
	float value;
	if (x==0 || x==w-1) {
	    value = 0;
	}
	else {
	    value = u(t, x, y) - u(t, x-1, y);
	}
	return (value);
}
/*
 * First derivative of u wrt y (negative method)
 */
float uyneg(int t, int x, int y)
{
	float value;
	if (y==0 || y==h-1) {
	    value = 0;
	}
	else {
	    value = u(t, x, y) - u(t, x, y-1);
	}
	return (value);
}
/*
 * First derivative of u wrt x (positive method)
 */
float uxpos(int t, int x, int y)
{
	float value;
	if (x==0 || x==w-1) {
	    value = 0;
	}
	else {
	    value = u(t, x+1, y) - u(t, x, y);
	}
	return (value);
}
/*
 * First derivative of u wrt y (positive method)
 */
float uypos(int t, int x, int y)
{
	float value;
	if (y==0 || y==h-1) {
	    value = 0;
	}
	else {
	    value = u(t, x, y+1) - u(t, x, y);
	}
	return (value);
}
/*
 * Second derivative of u wrt x
 */
float uxx(int t, int x, int y)
{
	float value;
	if (x==0 || x==w-1) {
	    value = 0;
	}
	else {
	    value = u(t, x+1, y) - 2*u(t, x, y) + u(t, x-1, y);
	}
	return (value);
}
/*
 * Second derivative of u wrt y
 */
float uyy(int t, int x, int y)
{
	float value;
	if (y==0 || y==h-1) {
	    value = 0;
	}
	else {
	    value = u(t, x, y+1) - 2*u(t, x, y) + u(t, x, y-1);
	}
	return (value);
}
/*
 * Second derivative cross
 */
float uxy(int t, int x, int y)
{
	float value;
	if (y==0 || y==h-1 || x==0 || x==w-1) {
	    value = 0;
	}
	else {
	    value = u(t, x+1, y+1) - u(t, x+1, y-1) - 
	    	u(t, x-1, y+1) + u(t, x-1, y-1);
	}
	return (value/4);
}
/*
 * Curvature term from IEEE paper
 */
float K(int t, int x, int y)
{
     float value;
     float UY = uy(t,x,y);
     float UX = ux(t,x,y);
     value = uxx(t,x,y)*pow(UY,2) - 2*UY*UX*uxy(t,x,y) +
		uyy(t,x,y)*pow(UX,2);
     value /= pow(del(t,x,y),2) + EPSILON;
     return(value);
}
