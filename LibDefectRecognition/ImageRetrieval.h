#pragma once

#ifdef LIBDEFECTRECOGNITION_EXPORTS
#define LIBDEFECTRECOGNITION_API __declspec(dllexport)
#else
#define LIBDEFECTRECOGNITION_API __declspec(dllimport)
#endif

#include "DCMIndexingFile.h"

struct LIBDEFECTRECOGNITION_API RetrievalResult
{
	string strMatchedFile;

	unsigned index;

	double dSimilarity;
};

LIBDEFECTRECOGNITION_API void GetFileList(vector<string> &aFileList, const string &strFolder, const string &strExt);

LIBDEFECTRECOGNITION_API void Search(vector<RetrievalResult> &aResult, 
	DCMFile &dfile, 
	const string &strImgLibrary,
	const string &strIndexFile);



class ImageRetrieval
{
public:
	ImageRetrieval();
	~ImageRetrieval();
};

