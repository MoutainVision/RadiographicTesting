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

//獲得索引列表
LIBDEFECTRECOGNITION_API void GetFileList(vector<string> &aFileList, const string &strFolder, const string &strExt);

//獲得索引數據列表
LIBDEFECTRECOGNITION_API bool GetIndexingDataList(vector<DCMFileIndexingData> &aIndexingData,
	const vector<DCMFileIndex> &aIdxList, const string &strIndexDataFile);

//文件特征匹配。如成功，則返回匹配到的文件名strMatchedFile
LIBDEFECTRECOGNITION_API bool MatchFileFeature(vector<DCMFileIndexingData> &aFeat, DCMFileFeat f, string &strMatchedFile);

LIBDEFECTRECOGNITION_API void Search(vector<RetrievalResult> &aResult, 
	DCMFile &dfile, ImageRect aoi, DetectParam pParam,
	const string &strImgLibrary,
	const string &strIndexFile);



class ImageRetrieval
{
public:
	ImageRetrieval();
	~ImageRetrieval();
};

