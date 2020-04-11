#pragma once

#ifdef LIBIMGIO_EXPORTS
#define LIBIMGIO_API __declspec(dllexport)
#else
#define LIBIMGIO_API __declspec(dllimport)
#endif

#include "ximage.h"

//#include "RawImageFile.h"

bool LIBIMGIO_API ReadDCMFile(const char *szDCMFile, unsigned short *pImgBuf, unsigned &nImgWidth, unsigned &nImgHeight);

bool LIBIMGIO_API WriteDCMFile(const char *szDCMFile, unsigned short *pImgBuf, unsigned nImgWidth, unsigned nImgHeight);

struct LIBIMGIO_API DCMFileFeat
{
	unsigned short nW;
	unsigned short nH;
	unsigned short nBpp;
	unsigned short winC;
	unsigned short winW;
	unsigned short nAveG;
	unsigned short nMinG;
	unsigned short nMaxG;
};

class LIBIMGIO_API DCMFile
{
private:
	unsigned short *m_pImgBuf;
	unsigned short m_nImgWidth;
	unsigned short m_nImgHeight;
	unsigned short m_nImgBPP;
	unsigned short m_nWinCentre;
	unsigned short m_nWinWidth;

public:
	DCMFile()
		: m_pImgBuf(NULL)
		, m_nImgWidth(0)
		, m_nImgHeight(0)
		, m_nImgBPP(0)
		, m_nWinCentre(0)
		, m_nWinWidth(0)
	{
	}

	void Release()
	{
		if (NULL != m_pImgBuf)
		{
			delete[]m_pImgBuf;
			m_pImgBuf = NULL;
		}
		m_nImgWidth = 0;
		m_nImgHeight = 0;
		m_nImgBPP = 0;
		m_nWinCentre = 0;
		m_nWinWidth = 0;

		return;
	}

	~DCMFile()
	{
		Release();
	}

	bool IsValid()
	{
		return (NULL != m_pImgBuf && 0 < m_nImgWidth && 0 < m_nImgHeight);
	}

	bool Load(const char *szSrcFileName);

	bool Save(const char *szDstFileName);

	unsigned short* GetBuffer()
	{
		return m_pImgBuf;
	}

	unsigned short GetWidth()
	{
		return m_nImgWidth;
	}

	unsigned short GetHeight()
	{
		return m_nImgHeight;
	}

	unsigned short GetBPP()
	{
		return m_nImgBPP;
	}

	unsigned short GetWindowCenter()
	{
		return m_nWinCentre;
	}

	unsigned short GetWindowWidth()
	{
		return m_nWinWidth;
	}

	bool Convert(CxImage &dstImg);

	DCMFileFeat getFileFeature();
};