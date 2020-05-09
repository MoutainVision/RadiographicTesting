#pragma once

#ifdef LIBIMGIO_EXPORTS
#define LIBIMGIO_API __declspec(dllexport)
#else
#define LIBIMGIO_API __declspec(dllimport)
#endif

#include "ximage.h"

#include"dcmtk\dcmdata\dctk.h"

#include<string>
#include<iostream>

using std::string;

#pragma pack(1)
//DCM�ļ�����
struct LIBIMGIO_API DCMFileFeat
{
	unsigned short nW;//ͼ�����ؿ��
	unsigned short nH;//ͼ�����ظ߶�
	unsigned short nBpp;//ͼ��λ��
	unsigned short winC;//ͼ��Ĵ�λ
	unsigned short winW;//ͼ��Ĵ���
	unsigned short nSpp;//ͼ����ÿ��������λ��
	unsigned nLen;
	//string strPI;
	unsigned short nAveG;//ͼ���ƽ������
	unsigned short nMinG;//ͼ�����С����ֵ
	unsigned short nNonZeroMinG;//ͼ�����С��������ֵ
	unsigned short nMaxG;//ͼ����������ֵ

	DCMFileFeat()
		: nW(0)
		, nH(0)
		, nBpp(0)
		, winC(0)
		, winW(0)
		, nSpp(0)
		, nLen(0)
		//, strPI("")
		, nAveG(0)
		, nMinG(0)
		, nNonZeroMinG(0)
		, nMaxG(0)
	{
	}

	void Output()
	{
		std::cout << "ImgW=" << nW << "\t"
				  << "ImgH=" << nH << "\t"
				  << "BPP=" << nBpp << "\t"
				  << "WinC=" << winC << "\t"
				  << "WinW=" << winW << "\t"
				  << "SPP=" << nSpp << "\t"
				  << "Lens=" << nLen << "\t"
				  //<< "PhtI=" << strPI << "\t"
				  << "AveG=" << nAveG << "\t"
				  << "MinG=" << nMinG << "\t"
				  << "NZMinG=" << nNonZeroMinG << "\t"
				  << "MaxG=" << nMaxG << std::endl;
	}

	void Output2()
	{
		std::cout << nW << "\t"
				  << nH << "\t"
				  << nBpp << "\t"
				  << winC << "\t"
				  << winW << "\t"
				  << nSpp << "\t"
				  << nLen << "\t"
				  //<< strPI << "\t"
				  << nAveG << "\t"
				  << nMinG << "\t"
				  << nNonZeroMinG << "\t"
				  << nMaxG << std::endl;
	}

	bool operator==(const DCMFileFeat &rhs)
	{
		if (this == &rhs)
		{
			return true;
		}

		if (nW != rhs.nW)		return false;
		if (nH != rhs.nH)		return false;
		if (nBpp != rhs.nBpp)	return false;
		if (winC != rhs.winC)	return false;
		if (winW != rhs.winW)	return false;
		if (nSpp != rhs.nSpp)	return false;
		if (nLen != rhs.nLen)	return false;
		if (nAveG != rhs.nAveG)	return false;
		if (nMinG != rhs.nMinG)	return false;
		if (nNonZeroMinG != rhs.nNonZeroMinG)	return false;
		if (nMaxG != rhs.nMaxG)	return false;
		
		return true;
	}
};
#pragma pack()

//DCM�ļ���
class LIBIMGIO_API DCMFile
{
private:
	unsigned short *m_pImgBuf;
	unsigned short m_nImgWidth;
	unsigned short m_nImgHeight;
	unsigned short m_nImgBPP;
	unsigned short m_nWinCentre;
	unsigned short m_nWinWidth;
	unsigned short m_nSamplesPerPixel;
	unsigned m_nDataLength;
	string m_strPhotoInterp;

	DcmFileFormat m_dcmFileFmt;

public:
	DCMFile()
		: m_pImgBuf(NULL)
		, m_nImgWidth(0)
		, m_nImgHeight(0)
		, m_nImgBPP(0)
		, m_nWinCentre(0)
		, m_nWinWidth(0)
		, m_nSamplesPerPixel(0)
		, m_nDataLength(0)
		, m_strPhotoInterp("")
	{
	}

	DCMFile(const char *szDCMFile)
	{
		m_pImgBuf = NULL;
		m_nImgWidth = 0;
		m_nImgHeight = 0;
		m_nImgBPP = 0;
		m_nWinCentre = 0;
		m_nWinWidth = 0;
		m_nSamplesPerPixel = 0;
		m_nDataLength = 0;
		m_strPhotoInterp = "";

		Load(szDCMFile);
	}

	bool CopyFrom(DCMFile &srcFile)
	{
		if (this == &srcFile)
		{
			return true;
		}

		if (srcFile.IsValid())
		{
			if (IsValid())
			{
				Release();
			}

			m_nImgWidth = srcFile.GetWidth();
			m_nImgHeight = srcFile.GetHeight();
			m_nImgBPP = srcFile.GetBPP();
			m_nWinCentre = srcFile.GetWindowCenter();
			m_nWinWidth = srcFile.GetWindowWidth();
			m_nSamplesPerPixel = srcFile.GetSamplePerPixel();
			m_nDataLength = srcFile.GetDataLength();
			m_strPhotoInterp = srcFile.GetPhotometricInterpretation();

			m_pImgBuf = new unsigned short[m_nImgWidth*m_nImgHeight];
			memcpy(m_pImgBuf, srcFile.GetBuffer(), m_nImgWidth*m_nImgHeight * sizeof(unsigned short));

			m_dcmFileFmt = srcFile.getDCMFileFormat();

			return true;
		}

		return false;
	}

	virtual void Release()
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
		m_nSamplesPerPixel = 0;
		m_nDataLength = 0;
		m_strPhotoInterp = "";

		m_dcmFileFmt.clear();

		return;
	}

	virtual ~DCMFile()
	{
		Release();
	}

	virtual bool IsValid()
	{
		return (NULL != m_pImgBuf && 0 < m_nImgWidth && 0 < m_nImgHeight);
	}

	//���ⲿ�ļ�����
	virtual bool Load(const char *szSrcFileName);

	//���浽�ⲿ�ļ�
	virtual bool Save(const char *szDstFileName);

	virtual DcmFileFormat& getDCMFileFormat()
	{
		return m_dcmFileFmt;
	}

	virtual unsigned short* GetBuffer()
	{
		return m_pImgBuf;
	}

	virtual unsigned short GetWidth()
	{
		return m_nImgWidth;
	}

	virtual unsigned short GetHeight()
	{
		return m_nImgHeight;
	}

	virtual unsigned short GetBPP()
	{
		return m_nImgBPP;
	}

	virtual unsigned short GetWindowCenter()
	{
		return m_nWinCentre;
	}

	virtual unsigned short GetWindowWidth()
	{
		return m_nWinWidth;
	}

	virtual string GetPhotometricInterpretation()
	{
		return m_strPhotoInterp;
	}

	virtual unsigned short GetSamplePerPixel()
	{
		return m_nSamplesPerPixel;
	}

	virtual unsigned GetDataLength()
	{
		return m_nDataLength;
	}

	//ת��ΪCxImage���󣬱�����ʾ
	virtual bool Convert(CxImage &dstImg);

	//����ļ�����
	virtual DCMFileFeat getFileFeature();

	//�����DCM�ļ�����ƥ�䣬����ƥ������ƶ�
	virtual bool Match(double &dSimilarity, DCMFile &file);
};