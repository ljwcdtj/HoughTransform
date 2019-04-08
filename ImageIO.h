#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

typedef unsigned char UCHAR;

//bmp相关
typedef struct  
{  
	//unsigned short    bfType;  
	unsigned long    bfSize;  
	unsigned short    bfReserved1;  
	unsigned short    bfReserved2;  
	unsigned long    bfOffBits;  
} ClBitMapFileHeader;  

typedef struct  
{  
	unsigned long  biSize;   
	long   biWidth;   
	long   biHeight;   
	unsigned short   biPlanes;   
	unsigned short   biBitCount;  
	unsigned long  biCompression;   
	unsigned long  biSizeImage;   
	long   biXPelsPerMeter;   
	long   biYPelsPerMeter;   
	unsigned long   biClrUsed;   
	unsigned long   biClrImportant;   
} ClBitMapInfoHeader;  

typedef struct   
{  
	unsigned char rgbBlue; //该颜色的蓝色分量  
	unsigned char rgbGreen; //该颜色的绿色分量  
	unsigned char rgbRed; //该颜色的红色分量  
	unsigned char rgbReserved; //保留值  
} ClRgbQuad;  

//函数声明

UCHAR *cpy_image(UCHAR *, int, int);
UCHAR *read_bmp(const char* , int , int);
bool read_image(const char *,  UCHAR *, int, int);
bool write_image(UCHAR *, int, int, const char * );
bool write_image(short *, int, int, const char * );
bool write_bmp(UCHAR *, char *,  int, int);
bool write_data(double *data, int width, int height, const char * name);
