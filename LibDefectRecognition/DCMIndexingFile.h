#pragma once


#ifdef LIBDEFECTRECOGNITION_EXPORTS
#define LIBDEFECTRECOGNITION_API __declspec(dllexport)
#else
#define LIBDEFECTRECOGNITION_API __declspec(dllimport)
#endif

#include "..\LibImgIO\LibImgIO.h"

#include "DefectRecognition.h"


#include<iostream>
#include<fstream>

using std::fstream;
using std::iostream;
using std::ios;

struct LIBDEFECTRECOGNITION_API DCMFileIndex
{
	string strFullPath;

	unsigned nOffset;

	unsigned nLength;

	DCMFileIndex()
		: strFullPath("")
		, nOffset(0)
		, nLength(0)
	{
	}

	DCMFileIndex(string strPath, unsigned nOff, unsigned nLen)
		: strFullPath(strPath)
		, nOffset(nOff)
		, nLength(nLen)
	{
	}
};

struct LIBDEFECTRECOGNITION_API DCMFileIndexingData
{
	string strFullPath;

	DCMFileFeat fileFeat;

	vector<DefectFeat> aDefectList;
};


class LIBDEFECTRECOGNITION_API DCMIndexingFile
{
public:
	DCMIndexingFile();
	virtual ~DCMIndexingFile();

	static bool Create(const char *szNewFile="DCMIndexData.dat")
	{
		fstream fs;
		fs.open(szNewFile, ios::out | ios::binary);
		if (!fs)
		{
			return false;
		}
		fs.close();

		return true;
	}

	static bool GetFileSize(const char *szFileName, size_t &nFileSize)
	{
		fstream f(szFileName, std::ios_base::binary | std::ios_base::in);
		if (!f.good() || f.eof() || !f.is_open())
		{
			return false;
		}

		f.seekg(0, std::ios_base::beg);

		fstream::pos_type begin_pos = f.tellg();

		f.seekg(0, std::ios_base::end);

		nFileSize = f.tellg() - begin_pos;

		return true;
	}

	static bool Write(DCMFileIndex &index, const char *szFile, const DCMFileIndexingData &data)
	{
		unsigned nSz;
		if (!GetFileSize(szFile, nSz))
		{
			return false;
		}

		fstream fs;
		fs.open(szFile, ios::app | ios::binary);
		if (!fs)
		{
			return false;
		}

		unsigned nLength = 0;

		unsigned nFilePathLength = data.strFullPath.size();
		fs.write((char*)&nFilePathLength, sizeof(nFilePathLength));
		nLength += sizeof(nFilePathLength);

		fs.write(data.strFullPath.c_str(), data.strFullPath.size() * sizeof(char));
		nLength += data.strFullPath.size() * sizeof(char);

		fs.write((char*)&data.fileFeat, sizeof(data.fileFeat));
		nLength += sizeof(data.fileFeat);

		unsigned nDefect = data.aDefectList.size();
		fs.write((char*)&nDefect, sizeof(nDefect));
		nLength += sizeof(nDefect);

		for (unsigned n = 0; n < nDefect; n++)
		{
			fs.write((char*)&data.aDefectList[n], sizeof(data.aDefectList[n]));
			nLength += sizeof(data.aDefectList[n]);
		}

		fs.close();
		
		index.strFullPath = data.strFullPath;

		index.nOffset = nSz;

		index.nLength = nLength;

		return true;
	}

	static bool Read(DCMFileIndexingData &data, const char *szFile, DCMFileIndex index)
	{
		fstream fs;
		fs.open(szFile, ios::in | ios::binary);
		if (!fs)
		{
			return false;
		}

		fs.seekg(index.nOffset, std::ios_base::beg);

		unsigned nSz;
		fs.read((char*)&nSz, sizeof(nSz));
		fs.read((char*)&data.strFullPath, nSz);

		fs.read((char*)&data.fileFeat, sizeof(data.fileFeat));

		unsigned nDefect;
		fs.read((char*)&nDefect, sizeof(nDefect));

		for (unsigned n = 0; n < nDefect; n++)
		{
			fs.read((char*)&data.aDefectList[n], sizeof(data.aDefectList[n]));
		}

		fs.close();
		
		return true;
	}

};

