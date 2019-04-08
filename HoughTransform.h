#pragma once 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef HOUGHTRANSFORM_H
#define HOUGHTRANSFORM_H
#endif

#ifndef max
#define max(a,b)  (((a)>(b))?(a):(b))
#endif

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif



#ifndef UCHAR
typedef unsigned char UCHAR;
#endif

typedef struct location
{
	int height;
	int width;
}HoughPoint;

typedef struct linedef
{
	HoughPoint point1;
	HoughPoint point2;
	int theta;
	int rho;
}HoughLine;

//theta range
#define THETA_BOTTOM -90
#define THETA_TOP 89

#define PI 3.14159265358979


static double round(double r)
{
	return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

class HoughCheckLines
{
public:
	HoughCheckLines(UCHAR *ucData, int nWidth, int nHeight, int PeaksNum);
	~HoughCheckLines();

	//under - construction

	// reference matlab hough function
	void GenerateHough(); 

	//reference matlab huoghpeaks function
	void GenerateHoughPeaks(int Threshold); 

	// reference matlab houghlines funciton
	void GenerateHoughLines(int GapFillNum, int MinLength); 

	void Houghpixels(int *x, int *y, int peakflag, int *r, int *c);

	int *GetH(){return H;};
	int *GetTheta(){return Theta;};
	int *GetRho(){return Rho;};
	HoughPoint *GetPeak(){return Peaks;};
	HoughLine *GetLines(){return Lines;};


private:
	UCHAR *ImageData;
	int ImageWidth;
	int ImageHeight;
	int PeakNumber;
	int *H;
	int *Theta;//Ω«∂»÷∆
	int ThetaLength;
	int *Rho; 
	int RhoLength;
	HoughPoint *Peaks;
	HoughLine *Lines;
};


typedef struct node
{
	int val;
	int rank;
}pnode;

void QuickSortPnode(pnode *src, int low, int high);
void ConstructNode(pnode *dst, int *src, int length);
void ConstructArray(pnode *src, int *dst, int length);
void ConstructPairArray(pnode *src, int *dst0, int *dst1, int length);