#include "HoughTransform.h"

HoughCheckLines::HoughCheckLines(UCHAR *ucData, int nWidth, int nHeight, int PeaksNum)
{
	//内部变量赋值
	ImageHeight = nHeight;
	ImageWidth = nWidth;
	PeakNumber = PeaksNum;

	ImageData = (UCHAR *)malloc(ImageWidth * ImageHeight * sizeof(UCHAR));
	memcpy(ImageData, ucData, nWidth * nHeight * sizeof(UCHAR));

	int thetanum = (THETA_TOP - THETA_BOTTOM) + 1;
	int dis = (int)ceil(sqrt((double)((nWidth - 1) * (nWidth - 1) + (nHeight - 1) * (nHeight - 1))));
	int rhonum = 2 * dis + 1;
	ThetaLength = thetanum;
	RhoLength = rhonum;

	H = (int *)calloc(ThetaLength * RhoLength, sizeof(int));
	Theta = (int *)malloc(ThetaLength * sizeof(int));
	Theta[0] = THETA_BOTTOM;
	Rho = (int *)malloc(RhoLength * sizeof(int));
	Rho[0] = -dis;

	for(int i = 1; i < ThetaLength; i++)
	{
		Theta[i] = Theta[i - 1] + 1; 
	}

	for(int i = 1; i < RhoLength; i++)
	{
		Rho[i] = Rho[i - 1] + 1; 
	}
 
	Peaks = (HoughPoint *)calloc(PeakNumber, sizeof(HoughPoint));
	Lines = (HoughLine *)calloc(PeakNumber, sizeof(HoughLine));
}

HoughCheckLines::~HoughCheckLines()
{
	free(ImageData);
	ImageData = NULL;

	free(Theta);
	Theta = NULL;

	free(Rho);
	Rho = NULL;

	free(Peaks);
	Peaks = NULL;

	free(Lines);
	Lines = NULL;
}

void HoughCheckLines::GenerateHough()
{
	//生成正弦，余弦查找表
	double *CosTheta = (double *)malloc(ThetaLength * sizeof(double));
	double *SinTheta = (double *)malloc(ThetaLength * sizeof(double));

	for(int i = 0; i < ThetaLength; i++)
	{
		CosTheta[i] = cos((double)(Theta[i]) / 180.0 * PI);
		SinTheta[i] = sin((double)(Theta[i]) / 180.0 * PI);
	}

	//计算H矩阵，按行排列
	int dis = (int)ceil(sqrt((double)((ImageWidth - 1) * (ImageWidth - 1) + (ImageHeight - 1) * (ImageHeight - 1))));
	for(int i = 0; i < ImageHeight; i++)
	{
		for(int j = 0; j < ImageWidth; j++)
		{
			if(ImageData[i * ImageWidth + j] != 0)
			//映射到H
			{
				for(int k = 0; k < ThetaLength; k++)
				{
					int rho_cal = (int)(round(j * CosTheta[k] + i * SinTheta[k]));
					H[(rho_cal + dis) * ThetaLength + k] = H[(rho_cal + dis) * ThetaLength + k] + 1;
				}
			}
		}
	}

	free(CosTheta);
	free(SinTheta);
	CosTheta = NULL;
	SinTheta = NULL;

	//debug codes
	//for(int i = 0; i < 10; i++)
	//printf("H = %d\n", H[981 * ThetaLength + i]);
}

void HoughCheckLines::GenerateHoughPeaks(int Threshold)
{
	//参数设置
	bool done = false;
    int nhood_width  = 2 * (ThetaLength / 100 + 1) + 1;
	int nhood_height = 2 * (RhoLength / 100 + 1) + 1;
	int nhood_center_x = (nhood_width - 1) / 2;
	int nhood_center_y = (nhood_height - 1) / 2;
    int max_h = H[0];
	int flag = 0;

	if(Threshold <= 0)
	{
		//max(H(:))
		for(int i = 0; i < ThetaLength * RhoLength; i++)
		{
			if(H[i] > max_h)
				max_h = H[i];
		}

		Threshold = max_h / 2;
	}

	int *Hnew = (int *)calloc(ThetaLength * RhoLength, sizeof(int));
	memcpy(Hnew, H, ThetaLength * RhoLength * sizeof(int));

	//循环
	while(!done)
	{
		int p, q;//height_width
		p = 0; q = 0;
		max_h = Hnew[0];
		for(int i = 0; i < ThetaLength * RhoLength; i++)
		{
			if(H[i] > max_h)
			{
				max_h = Hnew[i];
				p = i / ThetaLength;
				q = i % ThetaLength;
			}
		}

		if(Hnew[p * ThetaLength + q] >= Threshold)
		{
			Peaks[flag].height = p;
			Peaks[flag].width = q;
			flag++;
		
			int p1 = p - nhood_center_y;
			int p2 = p + nhood_center_y;
			int q1 = q - nhood_center_x;
			int q2 = q + nhood_center_x;

			p1 = max(p1, 0);
			p2 = min(p2, RhoLength - 1);

			//construct pp qq
			int meshwidth = q2 - q1 + 1;
			int meshheight = p2 - p1 + 1;

			int *pp = (int *)malloc(meshheight * meshwidth * sizeof(int));
			int *qq = (int *)malloc(meshheight * meshwidth * sizeof(int));

			int ii,jj;
			ii = 0;
			for(int i = p1; i <= p2; i++)
			{
				jj = 0;
				for(int j = q1; j <= q2; j++)
				{
					
					pp[ii * meshwidth + jj] = i;
					qq[ii * meshwidth + jj] = j;
					jj++;
				}
				ii++;
			}

			//90 degree
			for(int i = 0; i < meshheight * meshwidth; i++)
			{
				if(qq[i] < 0)
				{
					qq[i] += ThetaLength;
					pp[i] = RhoLength - pp[i] + 1; 
				}

				if(qq[i] > ThetaLength - 1)
				{
					qq[i] -= ThetaLength;
					pp[i] = RhoLength - pp[i] + 1;
				}
				Hnew[pp[i] * ThetaLength + qq[i]] = 0;
			}

			done = (flag == PeakNumber);
			
			free(pp);
			free(qq);
			pp = NULL;
			qq = NULL;
		}
		else
		{
			done = true;
		}
	}

	free(Hnew);
	Hnew = NULL;
}

void HoughCheckLines::GenerateHoughLines(int GapFillNum, int MinLength)
{

	//parameters usage under construction...

	//默认数据
	if(GapFillNum < 0)
		GapFillNum = 20;
	if(MinLength < 0)
		MinLength = 40;

	//PASCAL风格数组，第0元素存储实际所用的像素数目
	int *nonzeropix_x = (int *)calloc(ImageWidth * ImageHeight + 1, sizeof(int));
	int *nonzeropix_y = (int *)calloc(ImageWidth * ImageHeight + 1, sizeof(int));

	//构造x, y
	int i, j, k;
	k = 1;
	for(i = 0; i < ImageHeight; i++)
	{
		for(j = 0; j < ImageWidth; j++)
		{
			if(ImageData[i * ImageWidth + j] != 0)
			{
				nonzeropix_x[k] = j;
				nonzeropix_y[k] = i;
				nonzeropix_x[0] += 1;
				nonzeropix_y[0] += 1;
				k++;
			}
		}
	}

	int minlength_sq = MinLength * MinLength;
	int fillgap_sq = GapFillNum * GapFillNum;
	int numlines = 0;
	int nonzeropixlen = nonzeropix_y[0];
	
	for(int k = 0; k < PeakNumber; k++)
	{
		int *r = (int *)calloc(nonzeropixlen + 1, sizeof(int));
		int *c = (int *)calloc(nonzeropixlen + 1, sizeof(int));

		Houghpixels(nonzeropix_x, nonzeropix_y, k, r, c);

		if(r[0] == 0)
		{
			free(r);
			r = NULL;
			free(c);
			c = NULL;
		}
		else
		{
			//int rlength = r[0];
			//int *dist_sq = (int *)malloc((rlength - 1) * sizeof(int));

			//for(i = 0, j = 1; i < rlength - 1; i++, j++)
			//{
			//	dist_sq[i] = (r[j + 1] - r[j])*(r[j + 1] - r[j])+ (c[j + 1] - c[j])*(c[j + 1] - c[j]);
			//}

			//printf("Testing...\n");

			////find 'proper' gap
			//int *idx = (int *)malloc(rlength + 2) * sizeof(int));

			int p1_x = c[1];
			int p2_x = c[c[0]];
			int p1_y = r[1];
			int p2_y = r[r[0]];

			Lines[k].point1.width = p1_x;
			Lines[k].point1.height = p1_y;
			Lines[k].point2.width = p2_x;
			Lines[k].point2.height = p2_y;

			Lines[k].theta = Theta[Peaks[k].width];
			Lines[k].rho = Rho[Peaks[k].height];

			free(r);
			free(c);
			r = NULL;
			c = NULL;
		}

		//printf("Testing...\n");
	}
	free(nonzeropix_x);
	nonzeropix_x = NULL;
	free(nonzeropix_y);
	nonzeropix_y = NULL;
}


void HoughCheckLines::Houghpixels(int *x, int *y, int peakflag, int *yr, int *xc)
{
	HoughPoint peak = Peaks[peakflag];

	double Theta_c = (double)(Theta[peak.width]) * PI / 180.0;
	double Cos_Theta_c = cos(Theta_c);
	double Sin_Theta_c = sin(Theta_c);

	//theta_c
	int x_length = x[0];
	double *rho_xy = (double *)malloc(x_length * sizeof(double));
	int i, j;

	//rho_xy
	//int neg = 0;
	for(i = 1, j = 0; i <= x_length; i++, j++)
	{
		rho_xy[j] = x[i] * Cos_Theta_c + y[i] * Sin_Theta_c;
		//if(rho_xy[j] < 0)
		//	neg++;
	}
	//printf("%d\n", neg);

	//nrho slope rho_bin_index
	int nrho = RhoLength;
	double slope = (double)(nrho - 1) / (double)(Rho[nrho - 1] - Rho[0]);

	//rho_bin_index
	int *rho_bin_index = (int *)malloc(x_length * sizeof(int));
	for(i = 0; i < x_length; i++, j++)
	{
		rho_bin_index[i] = (int)(round(slope * (rho_xy[i] - Rho[0])));
	}

	for(i = 0, j = 1; i < x_length; i++)
	{
		if(rho_bin_index[i] == peak.height)
		{
			yr[j] = y[i + 1];
			xc[j] = x[i + 1];//注意x, y索引值
			yr[0] += 1;
			xc[0] += 1;
			j++;
		}
	}

	//reSortHoughPixels realize

	if(yr[0] != 0)
	{
		int max_r, min_r, range_r;
		int max_c, min_c, range_c;

		max_r = yr[1];
		min_r = yr[1];
		max_c = xc[1];
		min_c = xc[1];
		for(i = 1; i <= yr[0]; i++)
		{
			if(yr[i] > max_r)
				max_r = yr[i];
			if(yr[i] < min_r)
				min_r = yr[i];
			if(xc[i] > max_c)
				max_c = xc[i];
			if(xc[i] < min_c)
				min_c = xc[i];
		}
		range_r = max_r - min_r;
		range_c = max_c - min_c;

		if(range_r > range_c)
		{
			//按照r为主排序
			pnode *rtemp = (pnode *)malloc(yr[0] * sizeof(pnode));
			int *rranked = (int *)malloc(yr[0] * sizeof(int));
			int *cranked = (int *)malloc(xc[0] * sizeof(int));
			memcpy(cranked, &(xc[1]), xc[0] * sizeof(int));

			ConstructNode(rtemp, &(yr[1]), yr[0]);
			QuickSortPnode(rtemp, 0, yr[0] - 1);
			ConstructPairArray(rtemp, rranked, cranked, yr[0]);

			memcpy(&(yr[1]), rranked, yr[0] * sizeof(int));
			memcpy(&(xc[1]), cranked, xc[0] * sizeof(int));
			
			free(rtemp);
			rtemp = NULL;
			free(rranked);
			free(cranked);
			rranked = NULL;
			cranked = NULL;

		}
		else
		{
			//按照c为主排序
			pnode *ctemp = (pnode *)malloc(xc[0] * sizeof(pnode));
			int *rranked = (int *)malloc(yr[0] * sizeof(int));
			int *cranked = (int *)malloc(xc[0] * sizeof(int));
			memcpy(rranked, &(yr[1]), yr[0] * sizeof(int));

			ConstructNode(ctemp, &(xc[1]), xc[0]);
			QuickSortPnode(ctemp, 0, xc[0] - 1);
			ConstructPairArray(ctemp, cranked, rranked, xc[0]);

			free(ctemp);
			ctemp = NULL;
			memcpy(&(yr[1]), rranked, yr[0] * sizeof(int));
			memcpy(&(xc[1]), cranked, xc[0] * sizeof(int));

			free(ctemp);
			ctemp = NULL;
			free(rranked);
			free(cranked);
			rranked = NULL;
			cranked = NULL;
		}
	}

	free(rho_bin_index);
	rho_bin_index = NULL;
	free(rho_xy);
	rho_xy = NULL;
}




void QuickSortPnode(pnode *src, int low, int high)
{
	int i = low;
	int j = high;
	pnode key = src[low];
	if (low >= high)  //如果low >= high说明排序结束了
	{
		return ;
	}
	while (low < high)  //该while循环结束一次表示比较了一轮
	{
		while (low < high && key.val <= src[high].val)
		{
			--high;  //向前寻找
		}
		if (key.val > src[high].val)
		{
			src[low] = src[high];  //直接赋值, 不用交换
			++low;
		}
		while (low < high && key.val >= src[low].val)
		{
			++low;  //向后寻找
		}
		if (key.val < src[low].val)
		{
			src[high] = src[low];  //直接赋值, 不用交换
			--high;
		}
	}
	src[low] = key;  //查找完一轮后key值归位, 不用比较一次就互换一次。此时key值将序列分成左右两部分
	QuickSortPnode(src, i, low-1);  //用同样的方式对分出来的左边的部分进行同上的做法
	QuickSortPnode(src, low+1, j);  //用同样的方式对分出来的右边的部分进行同上的做法
}

void ConstructNode(pnode *dst, int *src, int length)
{
	int i;
	for(i = 0; i < length; i++)
	{
		dst[i].val = src[i];
		dst[i].rank = i;
	}
}

void ConstructArray(pnode *src, int *dst, int length)
{
	int i;
	for(i = 0; i < length; i++)
	{
		dst[i] = src[i].val;
	}
}



void ConstructPairArray(pnode *src, int *dst0, int *dst1, int length)
{
	int i;
	int *dsttemp = (int *)malloc(length * sizeof(int));
	memcpy(dsttemp, dst1, length * sizeof(int));

	for(i = 0; i < length; i++)
	{
		dst0[i] = src[i].val;
		dst1[i] = dsttemp[src[i].rank];
	}
	free(dsttemp);
	dsttemp = NULL;
}





