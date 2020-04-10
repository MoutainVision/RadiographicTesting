#include "stdafx.h"

#include "RawImageFile.h"

RawImageFile::RawImageFile()
	: m_nWidth(0)
	, m_nHeight(0)
	, m_pData(NULL)
{
}

RawImageFile::RawImageFile(int nW, int nH)
{
	m_nWidth = nW;
	m_nHeight = nH;
	m_pData = new unsigned short[nW*nH];
}

RawImageFile::~RawImageFile(void)
{
	if (NULL != m_pData)
	{
		delete[]m_pData;
		m_pData = NULL;
	}
}

bool RawImageFile::Load(const CString& sFile)
{
	CString strExt = sFile.Right(3);
	strExt.MakeLower();
	// 如果是RAW格式的图像，则以下打开处理
	if (strExt == "raw")
	{
		CFile rawfile;
		if (rawfile.Open(sFile, CFile::modeRead))
		{// 打开文件
			UINT nByte = rawfile.Read(m_pData, m_nHeight*m_nWidth * sizeof(unsigned short)); // 读取文件
			rawfile.Close();

			if (nByte == m_nHeight * m_nWidth * sizeof(unsigned short))
			{
				return true;
			}
		}
	}

	return false;
}

bool RawImageFile::Save(const CString &sFile)
{
	CString strExt = sFile.Right(3);
	strExt.MakeLower();
	// 如果是RAW格式的图像，则以下打开处理
	if (strExt == "raw")
	{
		CFile rawfile;
		if (rawfile.Open(sFile, CFile::modeWrite))
		{// 打开文件
			rawfile.Write(m_pData, m_nHeight*m_nWidth * sizeof(unsigned short)); // 寫文件
			rawfile.Close();

			return true;
		}
	}

	return false;
}

inline unsigned short* RawImageFile::GetData()
{
	return m_pData;
}

inline int RawImageFile::GetWidth()
{
	return m_nWidth;
}

inline int RawImageFile::GetHeight()
{
	return m_nHeight;
}