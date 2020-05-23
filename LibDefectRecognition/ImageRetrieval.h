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

//�@�������б�
LIBDEFECTRECOGNITION_API void GetFileList(vector<string> &aFileList, const string &strFolder, const string &strExt);

//�@�����������б�
LIBDEFECTRECOGNITION_API bool GetIndexingDataList(vector<DCMFileIndexingData> &aIndexingData,
	const vector<DCMFileIndex> &aIdxList, const string &strIndexDataFile);

//�ļ�����ƥ�䡣��ɹ����t����ƥ�䵽���ļ���strMatchedFile
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

