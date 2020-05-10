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

//DCM文件索引结构体。存放了某个文件索引数据的大小（以字节为单位）及其在索引文件中的偏移
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


//读取索引文件
LIBDEFECTRECOGNITION_API bool LoadIndexFile(vector<DCMFileIndex> &aIdx, const char *szIndexFile = "index.idx");


//DCM文件索引数据结构体。
struct LIBDEFECTRECOGNITION_API DCMFileIndexingData
{
	string strFullPath;

	DCMFileFeat fileFeat;

	vector<DefectFeat> aDefectList;
};


//DCM索引文件类。用于索引数据文件的读写，该索引数据文件由一系列文件索引数据（DCMFileIndexingData结构体）构成，
//每一笔数据对应于一个DCM文件
class LIBDEFECTRECOGNITION_API DCMIndexingFile
{
public:
	DCMIndexingFile();
	virtual ~DCMIndexingFile();

	//创建一个新的指定名的索引文件
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

	//得到文件大小，以字节为单位
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

	//将文件索引数据写入索引文件中，返回该数据对应的文件索引。
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

		//将DCM文件全路径名写入索引文件中
		//路径名长度
		unsigned nFilePathLength = data.strFullPath.size();
		fs.write((char*)&nFilePathLength, sizeof(nFilePathLength));
		nLength += sizeof(nFilePathLength);

		//路径名字符串
		fs.write(data.strFullPath.c_str(), data.strFullPath.size() * sizeof(char));
		nLength += data.strFullPath.size() * sizeof(char);

		//将DCM文件特征写入索引文件中
		fs.write((char*)&data.fileFeat, sizeof(data.fileFeat));
		nLength += sizeof(data.fileFeat);

		//将DCM文件中的缺陷特征列表写入索引文件
		//缺陷个数
		unsigned nDefect = data.aDefectList.size();
		fs.write((char*)&nDefect, sizeof(nDefect));
		nLength += sizeof(nDefect);

		//缺陷列表
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

	//从索引文件中读取给定文件索引的索引数据
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

		std::cout << data.strFullPath << std::endl;

		unsigned nDefect;
		fs.read((char*)&nDefect, sizeof(nDefect));

		if (data.aDefectList.size() >= nDefect)
		{
			for (unsigned n = 0; n < nDefect; n++)
			{
				fs.read((char*)&data.aDefectList[n], sizeof(data.aDefectList[n]));
			}
		}
		

		fs.close();
		
		return true;
	}

};

