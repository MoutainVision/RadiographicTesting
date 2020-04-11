#include "stdafx.h"
#include "RawIamge.h"


RawImage::RawImage()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_pData = NULL;
}

RawImage::RawImage(int nW, int nH)
{
	m_nWidth = nW;
	m_nHeight = nH;
	m_pData = new unsigned short[nW*nH];
}

RawImage::RawImage(const unsigned short *pBuf, int nW, int nH)
{
	m_nWidth = nW;
	m_nHeight = nH;
	m_pData = new unsigned short[nW*nH];
	memcpy(m_pData, pBuf, nW*nH * sizeof(unsigned short));
}

bool RawImage::Create(int nW, int nH)
{
	if (NULL != m_pData)
		delete[]m_pData;

	m_nWidth = nW;
	m_nHeight = nH;
	m_pData = new unsigned short[nW*nH];

	return true;
}

bool RawImage::Create(const unsigned short *pBuf, int nW, int nH)
{
	if (NULL != m_pData)
		delete[]m_pData;

	m_nWidth = nW;
	m_nHeight = nH;
	m_pData = new unsigned short[nW*nH];
	memcpy(m_pData, pBuf, nW*nH * sizeof(unsigned short));

	return true;
}

RawImage::~RawImage(void)
{
	if (NULL != m_pData)
	{
		delete[]m_pData;
		m_pData = NULL;
	}
}
//
//bool RawImage::Load(const CString& sFile)
//{
//	if (!IsValid())
//		return false;
//
//	CString strExt = sFile.Right(3);
//	strExt.MakeLower();
//	// 如果是RAW格式的图像，则以下打开处理
//	if (strExt == "raw")
//	{
//		CFile rawfile;
//		if (rawfile.Open(sFile, CFile::modeRead))
//		{//打开文件
//			unsigned short *pBuf = new unsigned short[m_nHeight*m_nWidth];
//			UINT nByte = rawfile.Read(pBuf, m_nHeight*m_nWidth * sizeof(unsigned short)); // 读取文件
//			rawfile.Close();
//
//			if (nByte == m_nHeight * m_nWidth * sizeof(unsigned short))
//			{
//				memcpy(m_pData, pBuf, nByte);
//
//				delete[]pBuf;
//
//				return true;
//			}
//
//			delete[]pBuf;
//		}
//	}
//
//	return false;
//}
//
//bool RawImage::Save(const CString &sFile)
//{
//	if (!IsValid())
//		return false;
//
//	CString strExt = sFile.Right(3);
//	strExt.MakeLower();
//	// 如果是RAW格式的图像，则以下打开处理
//	if (strExt == "raw")
//	{
//		CFile rawfile;
//		if (rawfile.Open(sFile, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
//		{// 打开文件
//			rawfile.Write(m_pData, m_nHeight*m_nWidth * sizeof(unsigned short)); // 寫文件
//			rawfile.Close();
//
//			return true;
//		}
//	}
//
//	return false;
//}

bool RawImage::IsValid()
{
	return (m_pData != NULL && m_nWidth > 0 && m_nHeight > 0);
}

bool RawImage::CopyFrom(RawImage &imgSrc)
{
	if (!imgSrc.IsValid())
		return false;

	if (m_pData == NULL)
	{
		m_nWidth = imgSrc.GetWidth();
		m_nHeight = imgSrc.GetHeight();
		m_pData = new unsigned short[m_nWidth*m_nHeight];
	}
	else if (m_nWidth != imgSrc.GetWidth() || m_nHeight != imgSrc.GetHeight())
	{
		delete[]m_pData;
		m_nWidth = imgSrc.GetWidth();
		m_nHeight = imgSrc.GetHeight();
		m_pData = new unsigned short[m_nWidth*m_nHeight];
	}

	memcpy(m_pData, imgSrc.GetData(), m_nWidth*m_nHeight * sizeof(unsigned short));

	return true;
}

inline unsigned short* RawImage::GetData()
{
	return m_pData;
}

inline int RawImage::GetWidth()
{
	return m_nWidth;
}

inline int RawImage::GetHeight()
{
	return m_nHeight;
}

bool RawImage::ConvertToMono8(unsigned char *pDst, int nW, int nP, int nH)
{
	long minIntensity, maxIntensity;
	return Convert(pDst, m_pData, nW, nP, nH, maxIntensity, minIntensity);
}

bool RawImage::Zoom(double dFactor)
{
	return Resize(m_pData, m_nWidth, m_nHeight, (int)(dFactor*m_nWidth), (int)(dFactor*m_nHeight));
}

bool RawImage::MirrorV()
{
	return Flip(m_pData, m_nWidth, m_nHeight);
}

bool RawImage::MirrorH()
{
	return Mirror(m_pData, m_nWidth, m_nHeight);
}

bool RawImage::Clip(int xs, int xe, int ys, int ye)
{
	return Crop(m_pData, m_nWidth, m_nHeight, xs, xe, ys, ye);
}

bool RawImage::Reverse()
{
	return Invert(m_pData, m_nWidth, m_nHeight);
}

bool RawImage::Laplacian_Transform()
{
	return Laplacian(m_pData, m_nWidth, m_nHeight);
}

bool RawImage::Window_Level_Transform(int nWinCentre, int nWinWidth)
{
	return WindowLevelTransform(m_pData, m_nWidth, m_nHeight, nWinCentre, nWinWidth);
}

bool RawImage::Median(int nFilterRadius, ImageRect *pROI)
{
	return MedianFiltering(m_pData, m_nWidth, m_nHeight, nFilterRadius, pROI);
}

bool RawImage::Gaussian(ImageRect *aoi)
{
	return GaussianFiltering(m_pData, m_nWidth, m_nHeight, aoi);
}

bool RawImage::Emboss_Transform(EEmbOp eop)
{
	return Emboss(m_pData, m_nHeight, m_nWidth, eop);
}

bool RawImage::GetMinMaxIntensity(unsigned short &minInt, unsigned short &maxInt)
{
	if (!IsValid())
		return false;

	minInt = m_pData[0];
	maxInt = m_pData[0];

	unsigned N = m_nHeight * m_nWidth;

	for (unsigned i = 1; i < N; i++)
	{
		if (m_pData[i] > maxInt)
			maxInt = m_pData[i];

		if (m_pData[i] < minInt)
			minInt = m_pData[i];
	}

	return true;
}

bool RawImage::GetIntensity(unsigned short &nInt, unsigned x, unsigned y)
{
	if (!IsValid())
		return false;

	if (x >= m_nWidth || y >= m_nHeight)
		return false;

	nInt = m_pData[y*m_nWidth + x];

	return true;
}

bool RawImage::GetRowIntensityCurve(vector<unsigned short> &aIntensity, int iRow)
{
	if (iRow < 0 || iRow >= m_nHeight)
	{
		return false;
	}

	if (!IsValid())
	{
		return false;
	}

	aIntensity.clear();

	unsigned short *pRow = m_pData + iRow * m_nWidth;

	for (int x = 0; x < m_nWidth; x++)
	{
		aIntensity.push_back(pRow[0]);
	}

	return true;
}

bool RawImage::GetColumnIntensityCurve(vector<unsigned short> &aIntensity, int iCol)
{
	if (iCol < 0 || iCol >= m_nWidth)
	{
		return false;
	}

	if (!IsValid())
	{
		return false;
	}

	aIntensity.clear();

	int pos = iCol;

	for (int x = 0; x < m_nHeight; x++)
	{
		aIntensity.push_back(m_pData[pos]);

		pos += m_nWidth;
	}

	return true;

}
