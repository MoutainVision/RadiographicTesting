#include "stdafx.h"

#include "RawImageFile.h"
#include<iostream>
#include<fstream>
#include<string>

#include<algorithm>

using namespace std;

RawImageFile::RawImageFile()
	: m_nWidth(0)
	, m_nHeight(0)
	, m_pBuffer(NULL)
{
}

RawImageFile::RawImageFile(int nW, int nH)
{
	m_nWidth = nW;
	m_nHeight = nH;
	m_pBuffer = new unsigned short[nW*nH];
}

RawImageFile::~RawImageFile(void)
{
	if (NULL != m_pBuffer)
	{
		delete[]m_pBuffer;
		m_pBuffer = NULL;
	}
}

bool RawImageFile::Load(const char *szFile, int nW, int nH)
{
	if (nW <= 0 || nH <= 0)
	{
		return false;
	}

	string strFile = szFile;
	if (strFile.length() < 5)
	{
		return false;
	}

	string strExt = strFile.substr(strFile.length() - 3);
	std::transform(strExt.begin(), strExt.end(), strExt.begin(), ::tolower);
		// 如果是RAW格式的图像，则以下打开处理
	if (strExt == "raw")
	{
		ifstream ifs(szFile, ios::in | ios::binary);
		if (ifs)
		{// 打开文件
			size_t nPx = nW * (size_t)(nH);

			unsigned short *pBuf = nullptr;
			if (nullptr == (pBuf = new unsigned short[nPx]))
			{
				return false;
			}

			if (!ifs.read((char*)pBuf, nPx * sizeof(unsigned short)))
			{
				delete[]pBuf;
				return false;
			}

			if (nullptr != m_pBuffer)
			{
				delete[]m_pBuffer;
			}
			m_pBuffer = pBuf;
			m_nWidth = nW;
			m_nHeight = nH;

			return true;
		}
	}

	return false;
}

bool RawImageFile::Save(const char *szFile)
{
	if (nullptr == m_pBuffer)
	{
		return false;
	}

	string strFile = szFile;

	if (!strFile.empty())
	{
		ofstream ofs(szFile, ios::binary);
		if (ofs)
		{
			if (ofs.write((char*)m_pBuffer, m_nHeight*m_nWidth * sizeof(unsigned short)))
			{
				return true;
			}
		}
	}

	return false;
}

inline unsigned short* RawImageFile::GetBuffer()
{
	return m_pBuffer;
}

inline int RawImageFile::GetWidth()
{
	return m_nWidth;
}

inline int RawImageFile::GetHeight()
{
	return m_nHeight;
}