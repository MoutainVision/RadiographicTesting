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

//DCM�ļ������ṹ�塣������ĳ���ļ��������ݵĴ�С�����ֽ�Ϊ��λ�������������ļ��е�ƫ��
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


//��ȡ�����ļ�
LIBDEFECTRECOGNITION_API bool LoadIndexFile(vector<DCMFileIndex> &aIdx, const char *szIndexFile = "index.idx");


//DCM�ļ��������ݽṹ�塣
struct LIBDEFECTRECOGNITION_API DCMFileIndexingData
{
	string strFullPath;

	DCMFileFeat fileFeat;

	vector<DefectFeat> aDefectList;
};


//DCM�����ļ��ࡣ�������������ļ��Ķ�д�������������ļ���һϵ���ļ��������ݣ�DCMFileIndexingData�ṹ�壩���ɣ�
//ÿһ�����ݶ�Ӧ��һ��DCM�ļ�
class LIBDEFECTRECOGNITION_API DCMIndexingFile
{
public:
	DCMIndexingFile();
	virtual ~DCMIndexingFile();

	//����һ���µ�ָ�����������ļ�
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

	//�õ��ļ���С�����ֽ�Ϊ��λ
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

	//���ļ���������д�������ļ��У����ظ����ݶ�Ӧ���ļ�������
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

		//��DCM�ļ�ȫ·����д�������ļ���
		//·��������
		unsigned nFilePathLength = data.strFullPath.size();
		fs.write((char*)&nFilePathLength, sizeof(nFilePathLength));
		nLength += sizeof(nFilePathLength);

		//·�����ַ���
		fs.write(data.strFullPath.c_str(), data.strFullPath.size() * sizeof(char));
		nLength += data.strFullPath.size() * sizeof(char);

		//��DCM�ļ�����д�������ļ���
		fs.write((char*)&data.fileFeat, sizeof(data.fileFeat));
		nLength += sizeof(data.fileFeat);

		//��DCM�ļ��е�ȱ�������б�д�������ļ�
		//ȱ�ݸ���
		unsigned nDefect = data.aDefectList.size();
		fs.write((char*)&nDefect, sizeof(nDefect));
		nLength += sizeof(nDefect);

		//ȱ���б�
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

	//�������ļ��ж�ȡ�����ļ���������������
	static bool Read(DCMFileIndexingData &data, const char *szFile, DCMFileIndex index)
	{
		fstream fs;
		fs.open(szFile, ios::in | ios::binary);
		if (!fs)
		{
			return false;
		}

		fs.seekg(index.nOffset, std::ios_base::beg);

		unsigned nFileNameLength;
		fs.read((char*)&nFileNameLength, sizeof(nFileNameLength));

		if (nFileNameLength > 0)
		{
			char *szBuf = nullptr;
			szBuf = new char[nFileNameLength+1];
			fs.read(szBuf, nFileNameLength);
			szBuf[nFileNameLength] = '\0';
			data.strFullPath = szBuf;// +'\0';
			delete []szBuf;
		}

		fs.read((char*)&data.fileFeat, sizeof(data.fileFeat));

		//std::cout << data.strFullPath << std::endl;

		unsigned nDefect;
		fs.read((char*)&nDefect, sizeof(nDefect));

		DefectFeat df;
		for (unsigned n = 0; n < nDefect; n++)
		{
			fs.read((char*)&df, sizeof(df));
			data.aDefectList.push_back(df);
		}


		fs.close();

		return true;
	}

};
