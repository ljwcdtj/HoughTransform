#define STB_IMAGE_RESIZE_IMPLEMENTATION



#include "ImageIO.h"
#include "time.h"
#include "HoughTransform.h"

#include <time.h>
#include <stdio.h>
#define N 8



int main()
{
	

	//************* hough transform ***************** 
	char *readpath = "src/cameraman.bmp";
	int width = 256;
	int height = 256;
	clock_t start, end;

	start = clock();

	UCHAR *img_src_data = read_bmp(readpath, width, height);
	HoughCheckLines hcl(img_src_data, width,height, 2);
	hcl.GenerateHough();
	hcl.GenerateHoughPeaks(50);
	hcl.GenerateHoughLines(5, 5);
	end = clock();

	printf("time: %d ms.\n", end - start);
	write_bmp(img_src_data, "testnew.bmp", width, height);

	getchar();
	free(img_src_data);
	img_src_data = NULL;

}