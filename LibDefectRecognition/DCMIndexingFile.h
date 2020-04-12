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
};

struct LIBDEFECTRECOGNITION_API DCMFileIndexingData
{
	string strFullPath;

	DCMFileFeat fileFeat;

	unsigned nDefectNum;
	vector<Defect> aDefectList;
};


class DCMIndexingFile
{
public:
	DCMIndexingFile();
	virtual ~DCMIndexingFile();

	bool Create(const char *szNewFile)
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

	bool Write(const char *szFile, const DCMFileIndexingData &data)
	{
		fstream fs;
		fs.open(szFile, ios::app | ios::binary);
		if (!fs)
		{
			return false;
		}

		unsigned nFilePathLength = data.strFullPath.size();
		fs.write((char*)&nFilePathLength, sizeof(nFilePathLength));
		fs.write(data.strFullPath.c_str(), data.strFullPath.size() * sizeof(char));

		fs.write((char*)&data.fileFeat, sizeof(data.fileFeat));



		fs.close();

		return true;
	}


	bool GetFileSize(const char *szFileName, size_t &nFileSize)
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

};

