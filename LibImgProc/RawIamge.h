#pragma once

#ifdef LIBIMGPROC_EXPORTS
#define LIBIMGPROC_API __declspec(dllexport)
#else
#define LIBIMGPROC_API __declspec(dllimport)
#endif

#include "LibImgProc.h"

class LIBIMGPROC_API RawImage
{
private:
	//圖像寬度和高度
	int m_nWidth, m_nHeight;

	//圖像數據起始地址
	unsigned short *m_pData;

public:
	//構造函數
	RawImage();
	RawImage(int nW, int nH);
	RawImage(const unsigned short *pBuf, int nW, int nH);

	//析構函數
	virtual ~RawImage(void);

	//創建
	bool Create(int nW, int nH);

	//創建并賦值
	bool Create(const unsigned short *pBuf, int nW, int nH);

	//判斷是否有效
	bool IsValid();

	////加載
	//bool Load(const CString& sFile);

	////保存
	//bool Save(const CString &sFile);

	//拷貝
	bool CopyFrom(RawImage &imgSrc);

	//獲取圖像數據起始地址
	unsigned short* GetData();

	//獲取圖像寬度
	int GetWidth();

	//獲取圖像高度
	int GetHeight();

	//獲取圖像的最小和最大亮度值
	bool GetMinMaxIntensity(unsigned short &minInt, unsigned short &maxInt);

	//獲取給定像素點的亮度值
	bool GetIntensity(unsigned short &nInt, unsigned x, unsigned y);

	//獲取給定像素行的灰度曲線
	bool GetRowIntensityCurve(vector<unsigned short> &aIntensity, int iRow);

	//獲取給定像素列的灰度曲線
	bool GetColumnIntensityCurve(vector<unsigned short> &aIntensity, int iCol);

	//縮放
	bool Zoom(double dFactor = 2);

	//上下翻轉
	bool MirrorV();

	//左右翻轉
	bool MirrorH();

	//剪切
	bool Clip(int xs, int xe, int ys, int ye);

	//反相
	bool Reverse();

	//Laplace變換
	bool Laplacian_Transform();

	//窗寬窗位變換
	bool Window_Level_Transform(int nWinCentre, int nWinWidth);

	//浮雕變換
	bool Emboss_Transform(EEmbOp eop = EMB_IMAGE_GRADIENT_SOUTH_DETECT);

	//16位灰度圖轉換為8位灰度圖
	bool ConvertToMono8(unsigned char *pDst, int nW, int nP, int nH);

	//中值濾波
	bool Median(int nFilterRadius = 3, ImageRect *pROI=NULL);

	//高斯濾波
	bool Gaussian(ImageRect *aoi=NULL);

};
