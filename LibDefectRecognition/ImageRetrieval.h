#pragma once

#ifdef LIBDEFECTRECOGNITION_EXPORTS
#define LIBDEFECTRECOGNITION_API __declspec(dllexport)
#else
#define LIBDEFECTRECOGNITION_API __declspec(dllimport)
#endif

#include<string>
#include<vector>

#include "..\LibImgIO\LibImgIO.h"

#include "DefectRecognition.h"

using std::vector;
using std::string;


struct LIBDEFECTRECOGNITION_API DCMFileIndexingData
{
	string strFullPath;

	DCMFileFeat fileFeat;

	unsigned nDefectNum;
	vector<Defect> aDefectList;
};

struct LIBDEFECTRECOGNITION_API RetrievalResult
{
	unsigned index;

	double dSimilarity;
};

LIBDEFECTRECOGNITION_API void GetFileList(vector<string> &aFileList, const string &strFolder, const string &strExt);

LIBDEFECTRECOGNITION_API bool CreateIndexingFile(const char *szIndexFile);

LIBDEFECTRECOGNITION_API bool Append(DCMFileIndexingData data, const char *szIndexFile);

LIBDEFECTRECOGNITION_API void Search(vector<RetrievalResult> &aResult, DCMFile &dfile, const string &strImgLibrary);



class ImageRetrieval
{
public:
	ImageRetrieval();
	~ImageRetrieval();
};

