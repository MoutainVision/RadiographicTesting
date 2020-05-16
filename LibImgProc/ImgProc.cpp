// LibImgProc.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"


#include "ImgProc.h"

#include<iostream>
#include<fstream>

using namespace std;

void GetHist(unsigned aF[65536], unsigned short *pImg, int nW, int nH)
{
	for (int n = 0; n < 65536; n++)
	{
		aF[n] = 0;
	}

	for (int y = 0; y < nH; y++)
	{
		for (int x = 0; x < nW; x++)
		{
			aF[pImg[y*nW + x]]++;
		}
	}

	return;
}


void GetHist(unsigned aF[256], unsigned char *pImg, int nW, int nP, int nH)
{
	for (int n = 0; n < 256; n++)
	{
		aF[n] = 0;
	}

	for (int y = 0; y < nH; y++)
	{
		for (int x = 0; x < nW; x++)
		{
			aF[pImg[y*nP + x]]++;
		}
	}

	return;
}


//RAW转灰度图像
bool Convert(unsigned char *pDst, unsigned short *pSrc, int nW, int nP, int nH, long &maxIntensity, long &minIntensity)
{
	if (NULL == pDst || NULL == pSrc || nW < 0 || nP < 0 || nH < 0)
		return false;

	int i;

	maxIntensity = pSrc[0];
	minIntensity = pSrc[0];
	int N = nH * nW;
	for (i = 0; i < N; i++)
	{
		if (pSrc[i] > maxIntensity)
			maxIntensity = pSrc[i];
		if (pSrc[i] < minIntensity)
			minIntensity = pSrc[i];
	}

	if (maxIntensity == minIntensity)
	{
		memset(pDst, 255, nH*nP * sizeof(unsigned char));
	}
	else
	{
		const double dRatio = 255.0 / (maxIntensity - minIntensity);

		for (int y = 0; y < nH; y++)
			for (int x = 0; x < nW; x++)
			{
				unsigned char g = (unsigned char)((pSrc[y*nW + x] - minIntensity)*dRatio);
				//unsigned char g = (unsigned char)((pSrc[y*nW+x]-minIntensity)*255/(maxIntensity-minIntensity)); 
				pDst[y*nP + x] = g;
				//if (g != 0)
				//{
				//	int dbg = 10;
				//}
			}

		//unsigned aFS[65536];
		//GetHist(aFS, pSrc, nW, nH);
		//ofstream ofs("shortFreq.txt");
		//if (ofs)
		//{
		//	for (int n = 0; n < 65536; n++)
		//	{
		//		if (aFS[n] > 0)
		//			ofs << n << "\t" << aFS[n] << endl;
		//	}
		//}

		//unsigned aFC[256];
		//GetHist(aFC, pDst, nW, nP, nH);
		//ofstream ofs1("ucFreq.txt");
		//if (ofs1)
		//{
		//	for (int n = 0; n < 255; n++)
		//	{
		//		if (aFC[n] > 0)
		//			ofs1 << n << "\t" << aFC[n] << endl;
		//	}
		//}

	}

	return true;
}

bool Convert(unsigned short *pDst, unsigned char *pSrc, int nW, int nP, int nH)
{
	const double dRatio = 65535.0 / 255;

	for (int y = 0; y < nH; y++)
		for (int x = 0; x < nW; x++)
		{
			pDst[y*nW + x] = (unsigned short)(pSrc[y*nP + x] * dRatio);
		}

	return true;
}


void GetKernel(int pKernel[9], EEmbOp eop)
{
	switch (eop)
	{
		//梯度检测, 南（垂直）
	case EMB_IMAGE_GRADIENT_SOUTH_DETECT:
	{
		pKernel[0] = pKernel[2] = -1;  pKernel[1] = -2;
		pKernel[3] = pKernel[4] = pKernel[5] = 0;
		pKernel[6] = pKernel[8] = 1;   pKernel[7] = 2;
		break;
	}
	//梯度检测, 东(水平)
	case EMB_IMAGE_GRADIENT_EAST_DETECT:
	{
		pKernel[0] = pKernel[6] = -1;  pKernel[3] = -2;
		pKernel[1] = pKernel[4] = pKernel[7] = 0;
		pKernel[2] = pKernel[8] = 1;   pKernel[5] = 2;
		break;
	}
	//梯度检测, 东北-西南
	case EMB_IMAGE_GRADIENT_NORTHEAST_DETECT:
	{
		pKernel[1] = pKernel[5] = 1;    pKernel[2] = 2;
		pKernel[0] = pKernel[4] = pKernel[8] = 0;
		pKernel[3] = pKernel[7] = -1;   pKernel[6] = -2;
		break;
	}
	//梯度检测, 西北-东南
	case EMB_IMAGE_GRADIENT_SOUTHEAST_DETECT:
	{
		pKernel[0] = -2;  pKernel[1] = pKernel[3] = -1;
		pKernel[2] = pKernel[4] = pKernel[6] = 0;
		pKernel[5] = pKernel[7] = 1;   pKernel[8] = 2;
		break;
	}


	//平移和差分边缘增强, 东（水平）
	case EMB_IMAGE_DIFFERENCE_EAST_DETECT:
	{
		pKernel[0] = pKernel[1] = pKernel[2] = 0;
		pKernel[3] = -1; pKernel[4] = 1; pKernel[5] = 0;
		pKernel[6] = pKernel[7] = pKernel[8] = 0;
		break;
	}
	//平移和差分边缘增强, 南（垂直）
	case EMB_IMAGE_DIFFERENCE_SOUTH_DETECT:
	{
		pKernel[0] = pKernel[2] = 0; pKernel[1] = -1;
		pKernel[3] = 0; pKernel[4] = 1; pKernel[5] = 0;
		pKernel[6] = pKernel[7] = pKernel[8] = 0;
		break;
	}
	//平移和差分边缘增强, 东北-西南
	case EMB_IMAGE_DIFFERENCE_NORTHEAST_DETECT:
	{
		pKernel[0] = pKernel[1] = pKernel[2] = 0;
		pKernel[3] = 0; pKernel[4] = 1; pKernel[5] = 0;
		pKernel[6] = -1; pKernel[7] = pKernel[8] = 0;
		break;
	}
	//平移和差分边缘增强, 西北-东南
	case EMB_IMAGE_DIFFERENCE_SOUTHEAST_DETECT:
	{
		pKernel[0] = -1; pKernel[1] = pKernel[2] = 0;
		pKernel[3] = 0; pKernel[4] = 1; pKernel[5] = 0;
		pKernel[6] = pKernel[7] = pKernel[8] = 0;
		break;
	}


	//Prewitt边缘增强, 东（水平）
	case EMB_IMAGE_PREWITT_EAST_DETECT:
	{
		pKernel[0] = -1; pKernel[1] = pKernel[2] = 1;
		pKernel[3] = -1; pKernel[4] = -2; pKernel[5] = 1;
		pKernel[6] = -1; pKernel[7] = pKernel[8] = 1;
		break;
	}
	//Prewitt边缘增强, 南（垂直）
	case EMB_IMAGE_PREWITT_SOUTH_DETECT:
	{
		pKernel[0] = pKernel[1] = pKernel[2] = -1;
		pKernel[3] = 1; pKernel[4] = -2; pKernel[5] = 1;
		pKernel[6] = pKernel[7] = pKernel[8] = 1;
		break;
	}
	//Prewitt边缘增强, 东北-西南
	case EMB_IMAGE_PREWITT_NORTHEAST_DETECT:
	{
		pKernel[0] = pKernel[1] = pKernel[2] = 1;
		pKernel[3] = -1; pKernel[4] = -2; pKernel[5] = 1;
		pKernel[6] = pKernel[7] = -1; pKernel[8] = 1;
		break;
	}
	//Prewitt边缘增强, 西北-东南
	case EMB_IMAGE_PREWITT_SOUTHEAST_DETECT:
	{
		pKernel[0] = pKernel[1] = -1; pKernel[2] = 1;
		pKernel[3] = -1; pKernel[4] = -2; pKernel[5] = 1;
		pKernel[6] = pKernel[7] = pKernel[8] = 1;
		break;
	}

	default:
		break;
	}

	return;
}

bool Emboss(unsigned short *pData, int nRow, int nCol, EEmbOp eop)
{
	unsigned short *pTmp = new unsigned short[nRow*nCol];
	memcpy(pTmp, pData, nRow*nCol * sizeof(unsigned short));

	int pKernel[9];
	GetKernel(pKernel, eop);

	int nWeightSum = 0;
	for (int k = 0; k < 9; k++)
	{
		nWeightSum += pKernel[k];
	}

	if (nWeightSum == 0 || nWeightSum == 1)
	{
		for (int r = 1; r < nRow - 1; r++)
			for (int c = 1; c < nCol - 1; c++)
			{
				int pos = r * nCol + c;

				int sum = pKernel[0] * pTmp[pos - nCol - 1] + pKernel[1] * pTmp[pos - nCol] + pKernel[2] * pTmp[pos - nCol + 1] +
					pKernel[3] * pTmp[pos - 1] + pKernel[4] * pTmp[pos] + pKernel[5] * pTmp[pos + 1] +
					pKernel[6] * pTmp[pos + nCol - 1] + pKernel[7] * pTmp[pos + nCol] + pKernel[8] * pTmp[pos + nCol + 1] + 32768;

				pData[pos] = sum < 0 ? 0 : (sum > 65535 ? 65535 : sum);
			}
	}
	else
	{
		for (int r = 1; r < nRow - 1; r++)
			for (int c = 1; c < nCol - 1; c++)
			{
				int pos = r * nCol + c;

				int sum = pKernel[0] * pTmp[pos - nCol - 1] + pKernel[1] * pTmp[pos - nCol] + pKernel[2] * pTmp[pos - nCol + 1] +
					pKernel[3] * pTmp[pos - 1] + pKernel[4] * pTmp[pos] + pKernel[5] * pTmp[pos + 1] +
					pKernel[6] * pTmp[pos + nCol - 1] + pKernel[7] * pTmp[pos + nCol] + pKernel[8] * pTmp[pos + nCol + 1] + 32768;

				sum /= nWeightSum;

				pData[pos] = sum < 0 ? 0 : (sum > 65535 ? 65535 : sum);
			}
	}

	delete[]pTmp;

	return true;
}

bool Emboss(unsigned char *pData, int nRow, int nCol, EEmbOp eop)
{
	unsigned char *pTmp = new unsigned char[nRow*nCol];
	memcpy(pTmp, pData, nRow*nCol * sizeof(unsigned char));

	int pKernel[9];
	GetKernel(pKernel, eop);

	int nWeightSum = 0;
	for (int k = 0; k < 9; k++)
	{
		nWeightSum += pKernel[k];
	}

	if (nWeightSum == 0 || nWeightSum == 1)
	{
		for (int r = 1; r < nRow - 1; r++)
			for (int c = 1; c < nCol - 1; c++)
			{
				int pos = r * nCol + c;

				int sum = pKernel[0] * pTmp[pos - nCol - 1] + pKernel[1] * pTmp[pos - nCol] + pKernel[2] * pTmp[pos - nCol + 1] +
					pKernel[3] * pTmp[pos - 1] + pKernel[4] * pTmp[pos] + pKernel[5] * pTmp[pos + 1] +
					pKernel[6] * pTmp[pos + nCol - 1] + pKernel[7] * pTmp[pos + nCol] + pKernel[8] * pTmp[pos + nCol + 1];

				pData[pos] = sum < 0 ? 0 : (sum > 255 ? 255 : sum);
			}
	}
	else
	{
		for (int r = 1; r < nRow - 1; r++)
			for (int c = 1; c < nCol - 1; c++)
			{
				int pos = r * nCol + c;

				int sum = pKernel[0] * pTmp[pos - nCol - 1] + pKernel[1] * pTmp[pos - nCol] + pKernel[2] * pTmp[pos - nCol + 1] +
					pKernel[3] * pTmp[pos - 1] + pKernel[4] * pTmp[pos] + pKernel[5] * pTmp[pos + 1] +
					pKernel[6] * pTmp[pos + nCol - 1] + pKernel[7] * pTmp[pos + nCol] + pKernel[8] * pTmp[pos + nCol + 1];

				sum /= nWeightSum;

				pData[pos] = sum < 0 ? 0 : (sum > 255 ? 255 : sum);
			}
	}

	delete[]pTmp;

	return true;
}


template<class T>
T compare(const void *a, const void *b)
{
	return *(T*)a - *(T*)b;
}

template<class T>
T Median(T *pArray, int begin, int end)
{
	T *pA = new T[end - begin + 1];
	memcpy(pA, pArray + begin, (end - begin + 1) * sizeof(T));
	qsort(pA, (end - begin + 1), sizeof(T), compare);
	T m = *(pA + (end - begin + 1) / 2);
	delete[]pA;

	return m;
}

bool MedianFiltering(unsigned short *pImg, int nW, int nH, int nFilterRadius, ImageRect *aoi)
{
	int xmin, xmax, ymin, ymax;
	if (nullptr == aoi)
	{
		xmin = 0;
		xmax = nW - 1;
		ymin = 0;
		ymax = nH - 1;
	}
	else
	{
		xmin = aoi->xs;
		xmax = aoi->xe;
		ymin = aoi->ys;
		ymax = aoi->ye;
	}

	if (nullptr == pImg ||
		xmin < 0 || xmin > xmax || xmax > nW - 1 ||
		ymin < 0 || ymin > ymax || ymax > nH - 1 ||
		nFilterRadius < 0)
	{
		return false;
	}

	if (0 == nFilterRadius)
		return true;

	//备份源图像
	unsigned short *pBuf = new unsigned short[nW*nH];
	memcpy(pBuf, pImg, nW*nH * sizeof(unsigned short));

	//对图像中的矩形区域进行中值滤波
	for (int y = ymin; y < ymax; y++)
		for (int x = xmin; x < xmax; x++)
		{
			int ys = 0 > (y - nFilterRadius) ? 0 : (y - nFilterRadius);
			int ye = nH < (y + nFilterRadius) ? nH : (y + nFilterRadius);
			int xs = 0 > (x - nFilterRadius) ? 0 : (x - nFilterRadius);
			int xe = nW < (x + nFilterRadius) ? nW : (x + nFilterRadius);

			int w = xe - xs;
			int h = ye - ys;
			int *pTemp = new int[(xe - xs)*(ye - ys)];
			for (int i = ys; i < ye; i++)
				for (int j = xs; j < xe; j++)
					pTemp[(i - ys)*w + (j - xs)] = pImg[i*nW + j];

			pBuf[y*nW + x] = (unsigned short)Median(pTemp, 0, w*h);

			delete []pTemp;
		}

	//将滤波后的结果拷贝至源图像中
	memcpy(pImg, pBuf, nW*nH * sizeof(unsigned short));

	delete[]pBuf;

	return true;
}


bool GaussianFiltering(unsigned short *pImg, int nWidth, int nHeight, ImageRect *aoi)
{
	//3*3 kernel
	//1  2  1
	//2  4  2  
	//1  2  1
	//
	int xs, xe, ys, ye;
	if (nullptr == aoi)
	{
		xs = 0;
		xe = nWidth - 1;
		ys = 0;
		ye = nHeight - 1;
	}
	else
	{
		xs = aoi->xs;
		xe = aoi->xe;
		ys = aoi->ys;
		ye = aoi->ye;
	}

	if (nullptr == pImg ||
		xs < 0 || xs > xe || xe > nWidth - 1 ||
		ys < 0 || ys > ye || ye > nHeight - 1)
	{
		return false;
	}

	unsigned short *pTmp = new unsigned short[nWidth*nHeight];
	memcpy(pTmp, pImg, nWidth*nHeight * sizeof(unsigned short));

	for (int y = ys + 1; y <= ye - 1; y++)
	{
		for (int x = xs + 1; x <= xe - 1; x++)
		{
			int pos = y * nWidth + x;

			int sum = (pTmp[pos] << 2)
				+ (pTmp[pos - 1] << 1) + (pTmp[pos + 1] << 1) + (pTmp[pos - nWidth] << 1) + (pTmp[pos + nWidth] << 1)
				+ pTmp[pos + nWidth + 1] + pTmp[pos + nWidth - 1] + pTmp[pos - nWidth + 1] + pTmp[pos - nWidth - 1];

			pImg[pos] = sum >> 4;
		}
	}

	delete[]pTmp;

	return true;
}

bool WindowLevelTransform(unsigned short *pImg, int nW, int nH, int nWinCentre, int nWindWidth)
{
	// TODO: Add your command handler code here
	if (NULL == pImg || nW < 0 || nH < 0 ||
		nWinCentre < 0 || nWindWidth < 0)
	{
		return false;
	}

	int window_center = nWinCentre;      //窗位,窗位是指窗宽上下限的平均数,或称窗中心
	int window_width = nWindWidth;      //窗宽,窗宽即是指CT图像上所包含的CT值范围,宽的宽窄直接影响到图像的对比度和清晰度

	float f_min = (float)((2 * window_center - window_width) / 2.0 + 0.5);
	float f_max = (float)((2 * window_center + window_width) / 2.0 + 0.5);

	float dFactor;
	dFactor = (float)65535.0 / (f_max - f_min);

	//循环变量
	long i;

	long N = nH * nW;

	for (i = 0; i < N; i++)
	{
		if (pImg[i] < f_min)
			pImg[i] = 0;
		else if (pImg[i] > f_max)
			pImg[i] = 65535;
		else
			pImg[i] = (unsigned short)((pImg[i] - f_min)*dFactor);

		if (pImg[i] < 0)
			pImg[i] = 0;
		if (pImg[i] > 65535)
			pImg[i] = 65535;
	}

	return true;
}

bool Laplacian(unsigned short *pImg, int nW, int nH)
{
	if (NULL == pImg || nW < 0 || nH < 0)
	{
		return false;
	}

	unsigned short *pTmp = new unsigned short[nW*nH];
	memcpy(pTmp, pImg, nW*nH * sizeof(unsigned short));

	for (int y = 1; y < nH - 1; y++)
		for (int x = 1; x < nW - 1; x++)
		{
			int pos = y * nW + x;
			int g = pTmp[pos] * 9 -
				(pTmp[pos - nW - 1] + pTmp[pos - nW] + pTmp[pos - nW + 1] +
					pTmp[pos - 1] + pTmp[pos + 1] +
					pTmp[pos + nW - 1] + pTmp[pos + nW] + pTmp[pos + nW + 1]);
			pImg[pos] = g > 65535 ? 65535 : (g < 0 ? 0 : g);
		}

	delete[]pTmp;

	return true;
}

bool Laplacian(unsigned char *pImg, int nW, int nH)
{
	if (NULL == pImg || nW < 0 || nH < 0)
	{
		return false;
	}

	unsigned char *pTmp = new unsigned char[nW*nH];
	memcpy(pTmp, pImg, nW*nH * sizeof(unsigned char));

	for (int y = 1; y < nH - 1; y++)
		for (int x = 1; x < nW - 1; x++)
		{
			int pos = y * nW + x;
			int g = pTmp[pos] * 9 -
				(pTmp[pos - nW - 1] + pTmp[pos - nW] + pTmp[pos - nW + 1] +
					pTmp[pos - 1] + pTmp[pos + 1] +
					pTmp[pos + nW - 1] + pTmp[pos + nW] + pTmp[pos + nW + 1]);
			pImg[pos] = g > 255 ? 255 : (g < 0 ? 0 : g);
		}

	delete[]pTmp;

	return true;
}


bool Resize(unsigned short *&pImg, int &nW, int &nH, int nNewW, int nNewH)
{
	if (NULL == pImg || nW < 0 || nH < 0)
		return false;

	int w0 = nW;
	int h0 = nH;
	int p0 = w0;

	int w1 = nNewW;
	int h1 = nNewH;
	int p1 = w1;

	float fw = float(w0) / w1;
	float fh = float(h0) / h1;

	float *arr_x = new float[w1];
	float *arr_y = new float[h1];
	for (int y = 0; y < h1; y++)
	{
		arr_y[y] = y * fh;
	}
	for (int x = 0; x < w1; x++)
	{
		arr_x[x] = x * fw;
	}

	unsigned short *pSrc = pImg;

	unsigned short *pDst = new unsigned short[p1*h1];

	for (int y = 0; y < h1; y++)
	{
		int ySrc = (int)(arr_y[y]);
		float v = arr_y[y] - ySrc;

		for (int x = 0; x < w1; x++)
		{
			int xSrc = (int)(arr_x[x]);

			float u = arr_x[x] - xSrc;

			//coefficients
			float c11 = u * v;
			float c10 = v - c11;
			float c01 = u - c11;
			float c00 = 1 - u - v + c11;

			int pos = ySrc * p0 + xSrc;
			unsigned short g = pSrc[pos];
			if (xSrc < w0 - 1)
			{
				if (ySrc < h0 - 1)
				{
					g = (unsigned short)(c00*pSrc[pos] + c01 * pSrc[pos + 1] + c10 * pSrc[pos + p0] + c11 * pSrc[pos + p0 + 1]);
				}
				else
				{
					g = (unsigned short)((1 - u)*pSrc[pos] + u * pSrc[pos + 1]);
				}
			}
			else
			{
				if (ySrc < h0 - 1)
				{
					g = (unsigned short)((1 - v)*pSrc[pos] + v * pSrc[pos + p0]);
				}
				else
				{
					g = pSrc[pos];
				}
			}

			pDst[y*p1 + x] = g;
		}
	}

	delete[]arr_x;
	delete[]arr_y;

	delete[]pImg;
	pImg = new unsigned short[h1*p1];
	memcpy(pImg, pDst, h1*p1 * sizeof(unsigned short));
	delete[]pDst;

	nW = w1;
	nH = h1;

	return true;
}

bool ZoomIn2(unsigned short *&pImg, int &nW, int &nH)
{
	if (NULL == pImg || nW < 0 || nH < 0)
		return false;

	int w0 = nW;
	int h0 = nH;
	int p0 = w0;

	int w1 = 2 * nW;
	int h1 = 2 * nH;
	int p1 = w1;

	float fw = float(w0) / w1;
	float fh = float(h0) / h1;

	float *arr_x = new float[w1];
	float *arr_y = new float[h1];
	for (int y = 0; y < h1; y++)
	{
		arr_y[y] = y * fh;
	}
	for (int x = 0; x < w1; x++)
	{
		arr_x[x] = x * fw;
	}

	unsigned short *pSrc = pImg;

	unsigned short *pDst = new unsigned short[p1*h1];

	for (int y = 0; y < h1; y++)
	{
		int ySrc = (int)(arr_y[y]);
		float v = arr_y[y] - ySrc;

		for (int x = 0; x < w1; x++)
		{
			int xSrc = (int)(arr_x[x]);

			float u = arr_x[x] - xSrc;

			//coefficients
			float c11 = u * v;
			float c10 = v - c11;
			float c01 = u - c11;
			float c00 = 1 - u - v + c11;

			int pos = ySrc * p0 + xSrc;
			unsigned short g = pSrc[pos];
			if (xSrc < w0 - 1)
			{
				if (ySrc < h0 - 1)
				{
					g = (unsigned short)(c00*pSrc[pos] + c01 * pSrc[pos + 1] + c10 * pSrc[pos + p0] + c11 * pSrc[pos + p0 + 1]);
				}
				else
				{
					g = (unsigned short)((1 - u)*pSrc[pos] + u * pSrc[pos + 1]);
				}
			}
			else
			{
				if (ySrc < h0 - 1)
				{
					g = (unsigned short)((1 - v)*pSrc[pos] + v * pSrc[pos + p0]);
				}
				else
				{
					g = pSrc[pos];
				}
			}

			pDst[y*p1 + x] = g;
		}
	}

	delete[]arr_x;
	delete[]arr_y;

	delete[]pImg;
	pImg = new unsigned short[h1*p1];
	memcpy(pImg, pDst, h1*p1 * sizeof(unsigned short));
	delete[]pDst;

	nW = w1;
	nH = h1;

	return true;
}

bool ZoomOut2(unsigned short *&pImg, int &nWidth, int &nHeight)
{
	if (NULL == pImg || nWidth < 0 || nHeight < 0)
		return false;

	int nW = nWidth / 2;
	int nH = nHeight / 2;

	unsigned short *pTemp = new unsigned short[nW*nH];
	for (int y = 0; y < nHeight; y += 2)
	{
		for (int x = 0; x < nWidth; x += 2)
		{
			int pos = y * nWidth + x;
			int ave = (pImg[pos] + pImg[pos + 1] + pImg[pos + nWidth] + pImg[pos + 1 + nWidth]) / 4;
			pTemp[y / 2 * nW + x / 2] = (unsigned short)ave;
		}
	}

	delete[]pImg;

	pImg = new unsigned short[nW*nH];
	memcpy(pImg, pTemp, nW*nH * sizeof(unsigned short));
	nWidth = nW;
	nHeight = nH;

	delete[]pTemp;

	return true;
}

bool Rotate90(unsigned short *&pImg, int &nWidth, int &nHeight)
{
	if (NULL == pImg || nWidth < 0 || nHeight < 0)
		return false;

	int nW = nHeight;
	int nH = nWidth;

	unsigned short *pTemp = new unsigned short[nW*nH];

	for (int y = 0; y < nHeight; y++)
	{
		for (int x = 0; x < nWidth; x++)
		{
			pTemp[(nH - 1 - x)*nW + y] = pImg[y*nWidth + x];
		}
	}

	delete[]pImg;
	pImg = new unsigned short[nW*nH];
	memcpy(pImg, pTemp, nW*nH * sizeof(unsigned short));
	nWidth = nW;
	nHeight = nH;

	delete[]pTemp;

	return true;
}

bool Rotate180(unsigned short *pImg, int nWidth, int nHeight)
{
	if (nullptr == pImg || 0 >= nWidth || 0 >= nHeight)
		return false;

	
	unsigned short *pStride = new unsigned short[nWidth];
	unsigned nStride = nWidth * sizeof(unsigned short);
	unsigned nHalfH = nHeight / 2;
	unsigned nHalfW = nWidth / 2;
	int nOffset0 = 0;
	int nOffset1 = (nHeight - 1) * nWidth;

	for (unsigned y = 0; y < nHalfH; y++)
	{
		unsigned short *pL0 = pImg + nOffset0;
		unsigned short *pL1 = pImg + nOffset1;

		for (unsigned x = 0; x < nHalfW; x++)
		{
			unsigned short t = *(pL0 + x);
			*(pL0+x) = *(pL0 + nWidth - 1 - x);
			*(pL0 + nWidth - 1 - x) = t;

			t = *(pL1 + x);
			*(pL1+x) = *(pL1 + nWidth - 1 - x);
			*(pL1 + nWidth - 1 - x) = t;
		}

		memcpy(pStride, pL0, nStride);
		memcpy(pL0, pL1, nStride);
		memcpy(pL1, pStride, nStride);

		nOffset0 += nWidth;
		nOffset1 -= nWidth;
	}

	delete[]pStride;

	return true;
}

bool Rotate270(unsigned short *&pImg, int &nWidth, int &nHeight)
{
	if (NULL == pImg || nWidth < 0 || nHeight < 0)
		return false;

	int nW = nHeight;
	int nH = nWidth;

	unsigned short *pTemp = new unsigned short[nW*nH];

	for (int y = 0; y < nHeight; y++)
	{
		for (int x = 0; x < nWidth; x++)
		{
			pTemp[x*nW +(nW - 1 - y)] = pImg[y*nWidth + x];
		}
	}

	delete[]pImg;
	pImg = pTemp;
	nWidth = nW;
	nHeight = nH;

	return true;
}

bool Flip(unsigned short *pImg, int nW, int nH)
{
	if (NULL == pImg || nW < 0 || nH < 0)
	{
		return false;
	}

	int nHalfH = nH / 2;

	for (int y = 0; y < nHalfH; y++)
	{
		for (int x = 0; x < nW; x++)
		{
			unsigned short t = pImg[y*nW + x];
			pImg[y*nW + x] = pImg[(nH - 1 - y)*nW + x];
			pImg[(nH - 1 - y)*nW + x] = t;
		}
	}

	return true;
}

bool Mirror(unsigned short *pImg, int nW, int nH)
{
	if (NULL == pImg || nW < 0 || nH < 0)
	{
		return false;
	}

	int nHalfW = nW / 2;

	for (int y = 0; y < nH; y++)
	{
		for (int x = 0; x < nHalfW; x++)
		{
			int pos = y * nW;

			unsigned short t = pImg[pos + x];
			pImg[pos + x] = pImg[pos + nW - 1 - x];
			pImg[pos + nW - 1 - x] = t;
		}
	}

	return true;
}

bool ContrastEnhancement(unsigned short *pImg, int nW, int nH, unsigned nContrast)
{
	if (NULL == pImg || nW < 0 || nH < 0 || nContrast > 100)
		return false;

	double dRatio = nContrast * 0.09 + 1.0;

	for (int y = 0; y < nH; y++)
		for (int x = 0; x < nW; x++)
		{
			double g = dRatio * pImg[y*nW + x];

			pImg[y*nW + x] = (unsigned short)(g > 65535 ? 65535 : g);
		}

	return true;
}

bool Invert(unsigned short *pImg, int nW, int nH)
{
	if (NULL == pImg || nW < 0 || nH < 0)
		return false;

	for (int y = 0; y < nH; y++)
		for (int x = 0; x < nW; x++)
		{
			pImg[y*nW + x] = 65535 - pImg[y*nW + x];
		}

	return true;
}

bool Crop(unsigned short *&pImg, int &nW, int &nH, int xs, int xe, int ys, int ye)
{
	if (NULL == pImg ||
		xs < 0 || xs > xe || xe >= nW ||
		ys < 0 || ys > ye || ye >= nH)
	{
		return false;
	}

	int w = xe - xs + 1;
	int h = ye - ys + 1;

	unsigned short *pTmp = new unsigned short[w*h];
	for (int y = ys; y <= ye; y++)
		for (int x = xs; x <= xe; x++)
		{
			pTmp[(y - ys)*w + (x - xs)] = pImg[y*nW + x];
		}

	delete[]pImg;
	pImg = new unsigned short[w*h];
	memcpy(pImg, pTmp, w*h * sizeof(unsigned short));
	delete[]pTmp;

	nW = w;
	nH = h;

	return true;
}

bool GetIntensity(unsigned short &nIntensity, unsigned short *pImg, int nW, int nH, int x, int y)
{
	if (NULL == pImg ||
		x < 0 || x >= nW ||
		y < 0 || y >= nH)
	{
		return false;
	}

	nIntensity = pImg[y*nW + x];

	return true;
}

//获取指定區域平均灰度值
bool GetIntensity(unsigned short &nIntensity, unsigned short *pImg,	int nW,	int nH,	int xs, int ys,	int xe, int ye)
{
	if (NULL == pImg ||
		0 > xs || xs > xe || xe>= nW ||
		0 > ys || ys > ye || ye>= nH)
	{
		return false;
	}

	double dIntensity = 0.0;
	for (int y = ys; y <= ye; y++)
	{
		for (int x = xs; x <= xe; x++)
		{
			dIntensity += pImg[y*nW + x];
		}
	}

	nIntensity = (unsigned short)(dIntensity / (xe - xs + 1) / (ye - ys + 1));

	return true;
}

bool GetIntensityCurve(vector<unsigned short> &aIntensity, unsigned short *pImg, int nW, int nH, int xs, int ys, int xe, int ye)
{
	if (NULL == pImg ||
		xs < 0 || xs >= nW ||
		ys < 0 || ys >= nH ||
		xe < 0 || xe >= nW ||
		ye < 0 || ye >= nH)
	{
		return false;
	}

	if (xs == xe && ys == ye)
	{
		return false;
	}

	double dx = xe - xs;
	double dy = ye - ys;
	double dLen = sqrt(dx*dx + dy * dy);
	double nx = dx / dLen;
	double ny = dy / dLen;

	aIntensity.push_back(pImg[ys*nW + xs]);

	int nLen = (int)(dLen);

	for (int i = 1; i < nLen; i++)
	{
		double x = xs + i * nx;
		double y = ys + i * ny;

		int x0 = (int)x;
		int y0 = (int)y;
		int p0 = y0 * nW + x0;

		unsigned short g;

		if (x < nW - 1)
		{
			if (y < nH - 1)
			{
				g = (unsigned short)((y0 + 1 - y)*((x0 + 1 - x)*pImg[p0] + (x - x0)*pImg[p0 + 1]) +
					(y - y0)*((x0 + 1 - x)*pImg[p0 + nW] + (x - x0)*pImg[p0 + nW + 1]));
			}
			else
			{
				g = (unsigned short)((x0 + 1 - x)*pImg[p0] + (x - x0)*pImg[p0 + 1]);
			}
		}
		else
		{
			if (y < nH - 1)
			{
				g = (unsigned short)((y0 + 1 - y)*pImg[p0] + (y - y0)*pImg[p0 + nW]);
			}
			else
			{
				g = pImg[p0];
			}
		}

		aIntensity.push_back(g);
	}

	//aIntensity.push_back(pImg[ye*nW+xe]);

	return true;
}

//计算信噪比
bool GetSNR(double &dMean, double &dStd, double &dSNR, unsigned short *pImg, int nW, int nH, int xs, int ys, int xe, int ye)
{
	if (NULL == pImg ||
		xs < 0 || xs >= nW ||
		ys < 0 || ys >= nH ||
		xe < 0 || xe >= nW ||
		ye < 0 || ye >= nH)
	{
		return false;
	}

	if (xs == xe && ys == ye)
	{
		return false;
	}

	unsigned nPx = (xe - xs + 1)*(ye - ys + 1);

	dMean = 0.0;
	dStd = 0.0;
	for (int y = ys; y <= ye; y++)
		for (int x = xs; x <= xe; x++)
		{
			dMean += pImg[y*nW + x];
		}
	dMean /= nPx;

	dStd = 0.0;
	for (int y = ys; y <= ye; y++)
		for (int x = xs; x <= xe; x++)
		{
			dStd += (pImg[y*nW + x] - dMean) * (pImg[y*nW + x] - dMean);
		}
	dStd = sqrt(dStd / nPx);

	if (dStd < 1.0e-10)
	{
		dSNR = 0.0;
	}
	else
	{
		dSNR = dMean / dStd;
	}

	return true;
}


bool GammaCorrection(unsigned short *pImg, int nW, int nH, double dGamma,
	unsigned short nDark, unsigned short nBright)
{
	if (NULL == pImg || nW < 0 || nH < 0)
		return false;

	long i;

	long N = nW * nH;

	double *pGammaVal = new double[65536];
	for (int k = 0; k < 65536; k++)
	{
		pGammaVal[k] = pow((double)(k) / 65535.0, dGamma);
	}

	for (i = 0; i < N; i++)
	{
		pImg[i] = (unsigned short)(pGammaVal[pImg[i]] * (nBright - nDark) + nDark);

		if (pImg[i] < 0)
			pImg[i] = 0;
		if (pImg[i] > 65535)
			pImg[i] = 65535;
	}

	delete[]pGammaVal;

	return true;
}

bool CalcWinLevelWidth(unsigned short &nWinLevel, unsigned short &nWinWidth, unsigned short *pImg, int nW, int nH, double dLowCP, double dHighCP)
{
	if (NULL == pImg || 0 >= nW || 0 >= nH)
	{
		return false;
	}

	unsigned hist[65536];
	GetHist(hist, pImg, nW, nH);

	bool used[65536];
	memset(used, false, 65536 * sizeof(bool));

	int ks = 0, ke = 65535;
	const unsigned short nMinWidth = 500;

	unsigned nPx = nW * nH;

	bool bValidWinFound = false;
	nWinLevel = 0;
	nWinWidth = 0;

	while (true)
	{
		unsigned nCnt = 0;//累积点数
		double dCP = 0.0;//累积概率
		unsigned short nLowerBound=ks, nUpperBound=ke;
		bool bLowerBoundFound = false;
		bool bUpperBoundFound = false;
		for (int k = ks; k <= ke; k++)
		{
			nCnt += hist[k];

			dCP = (double)(nCnt) / nPx;

			if (!bLowerBoundFound && dCP >= dLowCP)
			{
				nLowerBound = k;
				nUpperBound = k;

				bLowerBoundFound = true;
			}

			if (dCP >= dHighCP)
			{
				nUpperBound = k;
				bUpperBoundFound = true;

				break;
			}
		}

		if (bLowerBoundFound && bUpperBoundFound)
		{
			nWinWidth = (nUpperBound - nLowerBound + 1);
			nWinLevel = (nUpperBound + nLowerBound) / 2;
		}
		
		//判断找到的窗宽和窗位是否满足要求
		if (nWinWidth >= nMinWidth)
		{
			bValidWinFound = true;

			break;
		}

		//标记已用的灰度段
		for (int k = nLowerBound; k <= nUpperBound; k++)
		{
			used[k] = true;
		}

		//从未选用的灰度段中找出跨度最大的一段
		bool newSegFound = false;
		vector<int> aIdxSeg;
		for (int k = 0; k < 65536; k++)
		{
			if (!used[k])
			{
				if (!newSegFound)
				{
					ks = k;
					ke = k;
					newSegFound = true;
				}
				else
				{
					ke = k;
				}
			}
			else
			{
				if (newSegFound)
				{
					aIdxSeg.push_back(ks);
					aIdxSeg.push_back(ke);

					newSegFound = false;
				}
			}
		}
		if (newSegFound)
		{
			aIdxSeg.push_back(ks);
			aIdxSeg.push_back(ke);
		}

		int iMaxSpan = -1;
		unsigned nMaxSpan = 0;
		for (size_t k = 0; k < aIdxSeg.size(); k += 2)
		{
			if (nMaxSpan < (aIdxSeg[k + 1] - aIdxSeg[k]))
			{
				nMaxSpan = aIdxSeg[k + 1] - aIdxSeg[k];
				iMaxSpan = k;
			}			
		}
		if (-1 == iMaxSpan)
		{
			break;
		}

		ks = aIdxSeg[iMaxSpan];
		ke = aIdxSeg[iMaxSpan + 1];

		nPx = 0;
		for (int k = ks; k <= ke; k++)
		{
			nPx += hist[k];
		}
	}
		
	return bValidWinFound;
}


void ImageFilter8(double *fliter, int windowWidth, unsigned short *pixels, int w, int h, int stride)   //对buffer进行高斯平滑滤波
{
	int i;
	int ww = windowWidth * windowWidth;
	int hw = windowWidth / 2;
	int pixelByte = 1; ////////////////////////////////////

	unsigned short *_pixels0 = pixels;
	unsigned short *_pixels1 = new unsigned short[stride*h];
	unsigned short **_pixels = new unsigned short*[windowWidth*windowWidth];

	int _strideoff = stride - pixelByte * w + 2 * hw*pixelByte;
	unsigned short _letf = 0;
	unsigned short _top = 0;
	unsigned short _right = w;
	unsigned short _bottom = h;

	///////////////////////////////////////////////////////////////
	for (i = 0; i < stride*h; i++)
	{
		_pixels1[i] = pixels[i];
		_pixels0[i] = pixels[i];
	}

	int x, y;
	int m, n;
	int xx, yy;

	for (y = _top + hw; y < _bottom - hw; y++)   //图像卷积过程
		for (x = _letf + hw; x < _right - hw; x++)
		{
			double g = 0;

			for (i = 0; i < ww; i++)
			{
				m = i % windowWidth;
				n = i / windowWidth;

				yy = y + n - 2;
				xx = x + m - 2;
				g = g + _pixels1[yy*w + xx] * fliter[i];
			}

			_pixels0[y*stride + x] = (unsigned short)g;
		}

	delete[] _pixels;
	delete _pixels1;
	///////////////////////////////////////////////////////////////
	return;
}


BOOL MUSICA_ImageSubsample(unsigned short *X, int X_w, int X_h, int X_stride, unsigned short *X_1, int X_1_w, int X_1_h, int X_1_stride)   //buffer隔行抽样得到下一级的近似图X+1
{
	if (2 * (X_1_w - 1) > X_w - 1 || 2 * (X_1_h - 1) > X_h - 1)
	{
		return FALSE;
	}

	int pixelByte8 = 1;             ///////////////////////////////////////////
	unsigned short *_pixels_X = X;
	unsigned short *_pixels_X_1 = X_1;

	int _strideoff_X = X_stride - pixelByte8 * 2 * X_1_w;
	int _strideoff_X_1 = X_1_stride - pixelByte8 * X_1_w;

	unsigned short _letf = 0;
	unsigned short _top = 0;
	unsigned short _right = X_1_w;
	unsigned short _bottom = X_1_h;

	int x, y;
	for (y = _top; y < _bottom; y++)
	{
		for (x = _letf; x < _right; x++)
		{
			*_pixels_X_1 = *_pixels_X;
			_pixels_X_1++;
			_pixels_X += 2;
		}
		_pixels_X_1 += _strideoff_X_1;
		_pixels_X += _strideoff_X;
		_pixels_X += X_stride;
	}

	return TRUE;
}


BOOL MUSICA_ImageInterpolator(unsigned short *X_1, int X_1_w, int X_1_h, int X_1_stride, unsigned short *X, int X_w, int X_h, int X_stride)   //近似图X+1隔行插值0到buffer
{
	if (2 * (X_1_w - 1) > X_w - 1 || 2 * (X_1_h - 1) > X_h - 1)
	{
		return FALSE;
	}

	int pixelByte8 = 1;          ///////////////////////////////////////////
	unsigned short *_pixels_X = X;
	unsigned short *_pixels_X_1 = X_1;

	int _strideoff_X = X_stride - pixelByte8 * 2 * X_1_w;
	int _strideoff_X_1 = X_1_stride - pixelByte8 * X_1_w;

	unsigned short _letf = 0;
	unsigned short _top = 0;
	unsigned short _right = X_1_w;
	unsigned short _bottom = X_1_h;
	int i;
	//memset(_pixels_X,0,X_h*X_stride);
	for (i = 0; i < X_w*X_h; i++)
		_pixels_X[i] = 0;

	int x, y;
	for (y = 0; y < X_1_h; y++)
		for (x = 0; x < X_1_w; x++)
		{
			_pixels_X[y * 2 * X_w + x * 2] = X_1[y*X_1_w + x];
		}

	for (i = 0; i < X_w*X_h; i++)
		X[i] = _pixels_X[i];


	return TRUE;
}


void MUSICA_ImageSub8(unsigned short *pMinuend, unsigned short *pSubtrahend, unsigned short *pResult, int w, int h, int stride)   //X减去平滑后的buffer得到残差图像保存于X
{
	int pixelByte8 = 1;   /////////////////////////////////
	unsigned short *_pixels_M = pMinuend;
	unsigned short *_pixels_S = pSubtrahend;
	unsigned short *_pixels_R = pResult;
	int _strideoff = stride - pixelByte8 * w;

	unsigned short _letf = 0;
	unsigned short _top = 0;
	unsigned short _right = w;
	unsigned short _bottom = h;

	int x, y;
	for (y = _top; y < _bottom; y++)
	{
		for (x = _letf; x < _right; x++)
		{
			//*_pixels_X = *_pixels_X_1;
			int nResult = *_pixels_M - *_pixels_S + 32768;
			nResult = max(nResult, 0);
			nResult = min(nResult, 65535);
			*_pixels_R = (unsigned short)nResult;

			_pixels_M++;
			_pixels_S++;
			_pixels_R++;
		}
		_pixels_M += _strideoff;
		_pixels_S += _strideoff;
		_pixels_R += _strideoff;
	}

	return;
}

BOOL MUSICA_Decomposition(unsigned short *pPixelsGrey, unsigned short *pDecomposition, int *matrixInfo)
{
	//1) 对X进行一级分解，X拷贝到buffer
	//2) 对buffer进行平滑滤波
	//3) buffer隔行抽样得到下一级的近似图X+1
	//4) 近似图X+1隔行插值0到buffer
	//5) 进行滤波参数乘4（对置0行列的补偿）的平滑滤波
	//6) X减去平滑后的buffer得到残差图像保存于X

	int i;
	int nLevel = *matrixInfo;
	if (nLevel < 2)
	{
		return FALSE;
	}

	//函数原型为void *memcpy(void *destin, void *source, unsigned n)
	//从源内存地址的起始位置开始拷贝若干个字节到目标内存地址中，即从源source中拷贝n个字节到目标destin中
	memcpy(pDecomposition, pPixelsGrey, matrixInfo[2] * matrixInfo[3] * 2);    //unsigned short*2

	int windowWidth = 5;
	double filter[5 * 5] = {
		0.0025,	0.0125,	0.02,	0.0125,	0.0025,
			0.0125,	0.0625,	0.1,	0.0625,	0.0125,
			0.02,	0.1,	0.16,	0.1,	0.02,
			0.0125,	0.0625,	0.1,	0.0625,	0.0125,
			0.0025,	0.0125,	0.02,	0.0125,	0.0025
	};

	double filterMul4[5 * 5];
	for (i = 0; i < 5 * 5; i++)
	{
		filterMul4[i] = filter[i] * 4.0;
	}

	unsigned short *buffer = new unsigned short[matrixInfo[2] * matrixInfo[3]];
	unsigned short *X = pDecomposition;
	unsigned short *X_1 = NULL;

	for (i = 0; i < nLevel; i++)
	{
		int X_w = matrixInfo[i * 3 + 1];
		int X_h = matrixInfo[i * 3 + 2];
		int X_stride = matrixInfo[i * 3 + 3];
		int X_1_w = matrixInfo[(i + 1) * 3 + 1];
		int X_1_h = matrixInfo[(i + 1) * 3 + 2];
		int X_1_stride = matrixInfo[(i + 1) * 3 + 3];
		int X_size = X_h * X_stride;

		X_1 = X + X_size;
		//X -> buffer
		memcpy(buffer, X, X_size * 2);
		//X filter
		ImageFilter8(filter, 5, X, X_w, X_h, X_stride);

		//X Subsample by a factor of 2 -> X+1
		MUSICA_ImageSubsample(X, X_w, X_h, X_stride, X_1, X_1_w, X_1_h, X_1_stride);
		//X+1 Interpolate zero -> X
		MUSICA_ImageInterpolator(X_1, X_1_w, X_1_h, X_1_stride, X, X_w, X_h, X_stride);
		//X filter by 4 compensate
		ImageFilter8(filterMul4, 5, X, X_w, X_h, X_stride);
		//buffer-X -> X
		MUSICA_ImageSub8(buffer, X, X, X_w, X_h, X_stride);
		//(X+1) -> (X)
		X = X_1;
	}

	delete buffer;
	//MessageBox("分解完成。");

	return TRUE;
}

void MUSICA_Mapping(unsigned short *pDecomposition, int *matrixInfo, double power=0.6)
{
	int i;
	int nLevel = *matrixInfo;
	if (nLevel < 2)
	{
		return;
	}

	int m = 32767;
	//double power = 0.6;

	//double power = m_musicaPower;
	unsigned short *map;
	map = new unsigned short[65536];

	for (i = 0; i < 65536; i++)
	{
		double detail = i - 32768;

		detail = m * (detail / m);
		if (detail >= 0)
		{
			detail = m * pow(detail / m, power);
		}
		else {
			detail = -1 * m*pow(-1 * detail / m, power);
		}

		detail = detail + 32768;
		detail = max(detail, 0);
		detail = min(detail, 65535);

		map[i] = (unsigned short)detail;
	}

	int totleLength = 0;
	for (i = 0; i < nLevel; i++)
	{
		totleLength += matrixInfo[2 + i * 3] * matrixInfo[3 + i * 3];
	}

	unsigned short *_pixels = pDecomposition;
	for (i = 0; i < totleLength; i++)
	{
		*_pixels = map[*_pixels];
		_pixels++;
	}

	//MessageBox("映射完成。");
}


void MUSICA_ImageAdd8(unsigned short *pOriginal, unsigned short *pDetail, unsigned short *pResult, int w, int h, int stride)    //原始图像+细节
{
	unsigned short *_pixels_O;
	_pixels_O = new unsigned short[w*h];

	unsigned short *_pixels_D;
	_pixels_D = new unsigned short[w*h];

	unsigned short *_pixels_R;
	_pixels_R = new unsigned short[w*h];
	long int i;
	for (i = 0; i < w*h; i++)
	{
		_pixels_O[i] = pOriginal[i];
		_pixels_D[i] = pDetail[i];
	}

	long int _letf = 0;
	long int _top = 0;
	long int _right = w;
	long int _bottom = h;

	int x, y;
	for (y = _top; y < _bottom; y++)
	{
		for (x = _letf; x < _right; x++)
		{
			int nResult;
			nResult = _pixels_O[y*w + x] + _pixels_D[y*w + x] - 32768;
			nResult = max(nResult, 0);
			nResult = min(nResult, 65535);
			_pixels_R[y*w + x] = (unsigned short)nResult;
		}
	}

	for (i = 0; i < w*h; i++)
	{
		pResult[i] = _pixels_R[i];
	}

	delete _pixels_O;
	delete _pixels_D;
	delete _pixels_R;
	return;
}


BOOL MUSICA_Reconstruction(unsigned short *pPixelsGrey, unsigned short *pDecomposition, int *matrixInfo)   //重构图像
{
	int i;
	int nLevel = *matrixInfo;
	if (nLevel < 2)
	{
		return FALSE;
	}

	int windowWidth = 5;
	double filter[5 * 5] = {
		0.0025,	0.0125,	0.02,	0.0125,	0.0025,
			0.0125,	0.0625,	0.1,	0.0625,	0.0125,
			0.02,	0.1,	0.16,	0.1,	0.02,
			0.0125,	0.0625,	0.1,	0.0625,	0.0125,
			0.0025,	0.0125,	0.02,	0.0125,	0.0025
	};
	double filterMul4[5 * 5];
	for (i = 0; i < 5 * 5; i++)
	{
		filterMul4[i] = filter[i] * 4.0;
	}

	int totleLength = 0;
	for (i = 0; i < nLevel; i++)
	{
		totleLength += matrixInfo[2 + i * 3] * matrixInfo[3 + i * 3];
	}

	unsigned short *buffer = new unsigned short[matrixInfo[2] * matrixInfo[3]];
	unsigned short *X = NULL;
	unsigned short *X_1 = pDecomposition + totleLength;

	for (i = nLevel - 1; i >= 0; i--)
	{
		int X_w = matrixInfo[i * 3 + 1];
		int X_h = matrixInfo[i * 3 + 2];
		int X_stride = matrixInfo[i * 3 + 3];
		int X_1_w = matrixInfo[(i + 1) * 3 + 1];
		int X_1_h = matrixInfo[(i + 1) * 3 + 2];
		int X_1_stride = matrixInfo[(i + 1) * 3 + 3];
		int X_size = X_h * X_stride;
		int X_1_size = X_1_h * X_1_stride;

		//X
		X = X_1 - X_size;

		//X+1 Interpolate zero -> buffer
		MUSICA_ImageInterpolator(X_1, X_1_w, X_1_h, X_1_stride, buffer, X_w, X_h, X_stride);

		//buffer filter
		ImageFilter8(filterMul4, 5, buffer, X_w, X_h, X_stride);

		//X + buffer -> X
		MUSICA_ImageAdd8(buffer, X, X, X_w, X_h, X_stride);

		//next
		X_1 = X;
	}

	delete buffer;

	memcpy(pPixelsGrey, pDecomposition, matrixInfo[2] * matrixInfo[3] * 2);

//	MessageBox("重构完成。");
	return TRUE;
}



void IPFuncMUSICA(unsigned short *pImage, int nWidth, int nHeight, int nLevel, double dPower)
{
	int i;

	//分解级数
	//int nLevel = 6;

	if (nLevel < 2)
	{
		return;
	}

	//各级长宽
//	int* pnMatrixInfo = new int[1+nLevel*3];
	int pnMatrixInfo[200];

	int totleLength = 0;
	pnMatrixInfo[0] = nLevel;

	pnMatrixInfo[1] = nWidth;	//w

	pnMatrixInfo[2] = nHeight;//h

	pnMatrixInfo[3] = nWidth;	//Stride
	totleLength = pnMatrixInfo[2] * pnMatrixInfo[3];
	for (i = 1; i < nLevel + 1; i++)
	{
		pnMatrixInfo[1 + i * 3] = (pnMatrixInfo[1 + i * 3 - 3] + 1) / 2;//w
		pnMatrixInfo[2 + i * 3] = (pnMatrixInfo[2 + i * 3 - 3] + 1) / 2;//h
		pnMatrixInfo[3 + i * 3] = pnMatrixInfo[1 + i * 3];         //Stride
		totleLength += pnMatrixInfo[2 + i * 3] * pnMatrixInfo[3 + i * 3];
	}

	unsigned short *pPixelsGrey;
	pPixelsGrey = new unsigned short[pnMatrixInfo[2] * pnMatrixInfo[3]];

	unsigned short *pDecomposition;
	pDecomposition = new unsigned short[totleLength];


	long j;
	for (j = 0; j < nWidth*nHeight; j++)
		pPixelsGrey[j] = pImage[j];

	//分解
	MUSICA_Decomposition(pPixelsGrey, pDecomposition, pnMatrixInfo);     //金字塔分解

	//变换
	MUSICA_Mapping(pDecomposition, pnMatrixInfo, dPower);

	//重构
	MUSICA_Reconstruction(pPixelsGrey, pDecomposition, pnMatrixInfo);    //金字塔重构

	for (j = 0; j < nWidth*nHeight; j++)
		pImage[j] = pPixelsGrey[j];

	delete pPixelsGrey;
	delete pDecomposition;

	return;
}
