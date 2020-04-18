#pragma once

#ifdef LIBIMGPROC_EXPORTS
#define LIBIMGPROC_API __declspec(dllexport)
#else
#define LIBIMGPROC_API __declspec(dllimport)
#endif

#include<vector>

using std::vector;


struct LIBIMGPROC_API ImageRect
{
	int xs, xe, ys, ye;

	ImageRect()
		: xs(0)
		, xe(0)
		, ys(0)
		, ye(0)
	{
	}

	ImageRect(int xxs, int xxe, int yys, int yye)
		: xs(xxs)
		, xe(xxe)
		, ys(yys)
		, ye(yye)
	{
	}
};

typedef enum LIBIMGPROC_API EEmbossOperator
{
	EMB_IMAGE_GRADIENT_SOUTH_DETECT,	//梯度检测, 南（垂直）
	EMB_IMAGE_GRADIENT_EAST_DETECT,		//梯度检测, 东(水平)
	EMB_IMAGE_GRADIENT_NORTHEAST_DETECT,//梯度检测, 东北-西南
	EMB_IMAGE_GRADIENT_SOUTHEAST_DETECT,//梯度检测, 西北-东南

	EMB_IMAGE_DIFFERENCE_EAST_DETECT,		//平移和差分边缘增强, 东（水平）
	EMB_IMAGE_DIFFERENCE_SOUTH_DETECT,		//平移和差分边缘增强, 南（垂直）
	EMB_IMAGE_DIFFERENCE_NORTHEAST_DETECT,	//平移和差分边缘增强, 东北-西南
	EMB_IMAGE_DIFFERENCE_SOUTHEAST_DETECT,	//平移和差分边缘增强, 西北-东南

	EMB_IMAGE_PREWITT_EAST_DETECT,		//Prewitt边缘增强, 东（水平）
	EMB_IMAGE_PREWITT_SOUTH_DETECT,		//Prewitt边缘增强, 南（垂直）
	EMB_IMAGE_PREWITT_NORTHEAST_DETECT, //Prewitt边缘增强, 东北-西南
	EMB_IMAGE_PREWITT_SOUTHEAST_DETECT	//Prewitt边缘增强, 西北-东南
}EEmbOp;

//獲取卷積核
LIBIMGPROC_API void GetKernel(int pKernel[9],	//卷積核，輸出參數
	EEmbOp eop			//浮雕算子類型
);

//浮雕效果
LIBIMGPROC_API bool Emboss(unsigned short *pImg, //圖像數據指針，既為輸入亦為輸出
	int nRow,				//輸入圖像行數即高度
	int nCol,				//輸入圖像列數即寬度
	EEmbOp eop			//浮雕算子類型
);
LIBIMGPROC_API bool Emboss(unsigned char *pImg,	//圖像數據指針，既為輸入亦為輸出
	int nRow,				//輸入圖像行數即高度
	int nCol,				//輸入圖像列數即寬度
	EEmbOp eop			//浮雕算子類型
);

//16位灰度圖轉換為8位灰度圖，并返回16位圖像的最小和最大灰度值
LIBIMGPROC_API bool Convert(unsigned char *pDst,		//輸出的8位灰度圖像
	unsigned short *pSrc,	//輸入的16位灰度圖像
	int nW,					//輸入圖像的寬度
	int nP,					//待輸出圖像的字節寬度
	int nH,					//輸入圖像的高度
	long &maxIntensity,		//輸入圖像的的最大灰度值
	long &minIntensity		//輸入圖像的最小灰度值
);

//8位灰度圖轉換為16位灰度圖
LIBIMGPROC_API bool Convert(unsigned short *pDst,	//輸出的16位灰度圖像
	unsigned char *pSrc,		//輸入的8位灰度圖像
	int nW,					//輸入圖像的寬度
	int nP,					//輸入圖像的字節寬度
	int nH					//輸入圖像的高度
);

//中值濾波
LIBIMGPROC_API bool MedianFiltering(unsigned short *pImg,	//圖像數據指針，既為輸入亦為輸出
	int nW,					//輸入圖像的寬度
	int nH,					//輸入圖像的高度
	int nFilterRadius,		//濾波半徑
	ImageRect *aoi = NULL
);

//高斯濾波
LIBIMGPROC_API bool GaussianFiltering(unsigned short *pImg, //圖像數據指針，既為輸入亦為輸出
	int nWidth, 	//輸入圖像的寬度
	int nHeight, //輸入圖像的高度
	ImageRect *aoi = NULL); //濾波範圍


//窗寬窗位變換
LIBIMGPROC_API bool WindowLevelTransform(unsigned short *pImg,	//圖像數據指針，既為輸入亦為輸出
	int nW,					//輸入圖像的寬度
	int nH,					//輸入圖像的高度
	int nWinCentre,			//窗位
	int nWindWidth			//窗寬
);

//拉普拉斯變換
LIBIMGPROC_API bool Laplacian(unsigned short *pImg,	//圖像數據指針，既為輸入亦為輸出
	int nW,				//輸入圖像的寬度		
	int nH					//輸入圖像的高度	
);
LIBIMGPROC_API bool Laplacian(unsigned char *pImg,	//圖像數據指針，既為輸入亦為輸出
	int nW,				//輸入圖像的寬度
	int nH					//輸入圖像的高度
);

//縮放
LIBIMGPROC_API bool Resize(unsigned short *&pImg,	//圖像數據指針，既為輸入也為輸出
	int &nW,					//圖像寬度，既為輸入也為輸出
	int &nH,					//圖像高度，既為輸入也為輸出
	int nNewW,				//輸出圖像寬度
	int nNewH);				//輸出圖像高度

//放大2倍
LIBIMGPROC_API bool ZoomIn2(unsigned short *&pImg,	//圖像數據指針，既為輸入也為輸出
	int &nW,				//圖像寬度，既為輸入也為輸出
	int &nH					//圖像高度，既為輸入也為輸出
);

//縮小2倍
LIBIMGPROC_API bool ZoomOut2(unsigned short *&pImg,	//圖像數據指針，既為輸入也為輸出
	int &nW,				//圖像寬度，既為輸入也為輸出
	int &nH					//圖像高度，既為輸入亦為輸出
);

//順時針旋轉90度
LIBIMGPROC_API bool Rotate90(unsigned short *&pImg,	//圖像數據指針，既為輸入也為輸出
	int &nW,				//圖像寬度，既為輸入也為輸出
	int &nH					//圖像高度，既為輸入亦為輸出
);

//順時針旋轉180度
LIBIMGPROC_API bool Rotate180(unsigned short *pImg,	//圖像數據指針，既為輸入也為輸出
	int nW,				//圖像寬度，既為輸入也為輸出
	int nH					//圖像高度，既為輸入亦為輸出
);

//順時針旋轉270度
LIBIMGPROC_API bool Rotate270(unsigned short *&pImg,	//圖像數據指針，既為輸入也為輸出
	int &nW,				//圖像寬度，既為輸入也為輸出
	int &nH					//圖像高度，既為輸入亦為輸出
);

//上下翻轉
LIBIMGPROC_API bool Flip(unsigned short *pImg,	//圖像數據指針，既為輸入也為輸出
	int nW,					//輸入圖像寬度
	int nH					//輸入圖像高度
);

//左右翻轉
LIBIMGPROC_API bool Mirror(unsigned short *pImg,	//圖像數據指針，既為輸入也為輸出
	int nW,					//輸入圖像寬度
	int nH					//輸入圖像高度
);

//反相
LIBIMGPROC_API bool Invert(unsigned short *pImg,	//圖像數據指針，既為輸入也為輸出
	int nW,					//輸入圖像寬度
	int nH					//輸入圖像高度
);

//剪切
LIBIMGPROC_API bool Crop(unsigned short *&pImg,	//圖像數據指針，既為輸入也為輸出
	int &nW,				//圖像寬度，既為輸入也為輸出
	int &nH,				//圖像數據指針，既為輸入亦為輸出
	int xs, int xe,			//剪切区域的左右边界
	int ys, int ye			//剪切区域的上下边界
);

//获取灰度曲线
LIBIMGPROC_API bool GetIntensityCurve(vector<unsigned short> &aIntensity,
	unsigned short *pImg,	//輸入圖像數據指針
	int nW,				//輸入圖像寬度
	int nH,				//輸入圖像高度
	int xs, int ys,		//起点图像坐标
	int xe, int ye			//终点图像坐标
);

//计算信噪比
LIBIMGPROC_API bool GetSNR(double &dMean,	//均值
	double &dStd,	//方差
	double &dSNR,	//信噪比
	unsigned short *pImg,	//輸入圖像數據指針
	int nW,					//輸入圖像寬度
	int nH,					//輸入圖像高度
	int xs, int ys,			//起点图像坐标
	int xe, int ye			//终点图像坐标
);

//計算16位灰度圖像的直方圖
LIBIMGPROC_API void GetHist(unsigned aF[65536],		//直方圖
	unsigned short *pImg,	//輸入圖像數據指針
	int nW,					//輸入圖像寬度
	int nH					//輸入圖像高度
);

//計算8位灰度圖像的直方圖
LIBIMGPROC_API void GetHist(unsigned aF[256],		//直方圖
	unsigned char *pImg,	//輸入圖像數據指針
	int nW,					//圖像寬度
	int nP,					//輸入圖像字節寬度
	int nH					//輸入圖像高度
);

//伽馬校正
LIBIMGPROC_API bool GammaCorrection(unsigned short *pImg, int nW, int nH, double dGamma,
	unsigned short nDark = 0, unsigned short nBright = 65535);
