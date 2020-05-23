#include "stdafx.h"
#include "ImageRetrieval.h"

#include "DCMIndexingFile.h"

#include<io.h>
#include<direct.h>
#include<vector>

using std::fstream;
using std::vector;

string::size_type FindSubStrWithNoCase(string str, string substr)
{
	//turn all strings to lowercase for finding
	for (string::size_type ix = 0; ix != str.size(); ++ix)
	{
		str[ix] = tolower(str[ix]);
	}

	for (string::size_type ix = 0; ix != substr.size(); ++ix)
	{
		substr[ix] = tolower(substr[ix]);
	}

	return str.find(substr);
}

void GetFileList(vector<string> &aFileList, const string &strFolder, const string &strExt)
{
	struct _finddata_t fd;
	string root(strFolder);
	root += "\\*.*";
	long hFile = _findfirst(root.c_str(), &fd);
	if (hFile == -1)//the given path is an empty directory
	{
		return;
	}

	do
	{
		size_t strLength = strlen(fd.name);
		if (((strLength == 1) && (fd.name[0] == '.'))//current directory
			|| ((strLength == 2) && (fd.name[0] == '.') && (fd.name[1] == '.'))//parent directory
			)
		{
			continue;
		}

		string full_path = strFolder + "\\" + fd.name;

		if (fd.attrib & _A_SUBDIR)//current object is a directory
		{
			GetFileList(aFileList, full_path, strExt);
		}
		else//current object is a file
		{
			if (strExt == "*.*")
			{
				aFileList.push_back(full_path);
			}
			else
			{
				string::size_type pos = full_path.rfind('\\');
				string ext = full_path.substr(pos + 1);
				pos = ext.rfind('.');
				if (pos != string::npos)//the file has a postfix
				{
					ext = ext.substr(pos);//get the postfix

					if (FindSubStrWithNoCase(ext, strExt) != string::npos)
					{//the postfix is in the predefined postfix list
						aFileList.push_back(full_path);
					}
				}
				else//the file has no postfix
				{
					if (strExt == "")//i do want to return files with no postfix
					{
						aFileList.push_back(full_path);
					}
				}
			}
		}

	} while (_findnext(hFile, &fd) == 0);

	_findclose(hFile);

	return;
}


bool GetIndexList(vector<DCMFileIndex> &aIdxList, const char*szIndexFile)
{
	fstream ifs(szIndexFile, ios::in);
	if (ifs)
	{
		string str;
		unsigned nOff, nLen;
		string strDef;

		while (ifs >> str >> nOff >> nLen >> strDef)
		{
			aIdxList.push_back(DCMFileIndex(str, nOff, nLen, strDef));
		}

		return true;
	}

	return false;
}

bool FileFeatEqual(DCMFileFeat &f1, const DCMFileFeat &f2)
{
	return (f1 == f2);
}

bool DefectListEqual(vector<Defect> &aDL1, vector<DefectFeat> &aDL2)
{
	for (size_t k1 = 0; k1 != aDL1.size(); ++k1)
	{
		for (size_t k2 = 0; k2 != aDL2.size(); ++k2)
		{
			if (aDL1.at(k1).feat == aDL2.at(k2))
			{
				return true;
			}
		}
	}

	return false;
}

void Search(vector<RetrievalResult> &aResult, DCMFile &dfile, ImageRect aoi, DetectParam param,
	const string &strIndexFile, 
	const string &strIndexDataFile)
{
	if (!dfile.IsValid())
	{
		return;
	}

	//加载数据库的索引列表（为效率起见，该动作应该在函数外完成）
	vector<DCMFileIndex> aIdxList;
	if (!GetIndexList(aIdxList, strIndexFile.c_str()))
	{
		return;
	}
	//for (size_t k = 0; k != aIdxList.size(); ++k)
	//{
	//	std::cout << aIdxList[k].strFullPath << "\t" << aIdxList[k].nOffset << "\t" << aIdxList[k].nLength << "\n";
	//}


	RetrievalResult r;

	//取得文件的特征
	DCMFileFeat df = dfile.getFileFeature();

	//缺陷检测
	vector<Defect> aDefect;
	DetectDefect(aDefect, dfile.GetBuffer(), dfile.GetWidth(), dfile.GetHeight(), &aoi, &param);

	//先对比文件特征和包含的缺陷特征列表
	for (size_t k = 0; k != aIdxList.size(); k++)
	{
		DCMFileIndexingData data;
		if (DCMIndexingFile::Read(data, strIndexDataFile.c_str(), aIdxList[k]))
		{
			//先對比文件特征
			if (FileFeatEqual(df, data.fileFeat))
			{
				r.strMatchedFile = data.strFullPath;

				r.index = k;
				r.dSimilarity = 1.0;

				aResult.push_back(r);

				return;
			}

			//在對比缺陷特征
			if (DefectListEqual(aDefect, data.aDefectList))
			{
				r.strMatchedFile = data.strFullPath;

				r.index = k;
				r.dSimilarity = 1.0;

				aResult.push_back(r);

				return;
			}

		}
	}

	//通過文件特征和缺陷特征對比都沒找到，只能進行圖像匹配了。。。
	for (size_t k = 0; k != aIdxList.size(); k++)
	{
		DCMFile searchfile(aIdxList.at(k).strFullPath.c_str());

		double dSimilarity = 0.0;
		dfile.Match(dSimilarity, searchfile);
		if (dSimilarity > 0.99)
		{
			RetrievalResult r;
			r.dSimilarity = dSimilarity;
			r.strMatchedFile = aIdxList.at(k).strFullPath;

			aResult.push_back(r);

			break;
		}
	}

	return;
}

bool GetIndexingDataList(vector<DCMFileIndexingData> &aIndexingData,
	const vector<DCMFileIndex> &aIdxList, const string &strIndexDataFile)
{
	for (size_t k = 0; k != aIdxList.size(); k++)
	{
		DCMFileIndexingData data;
		if (!DCMIndexingFile::Read(data, strIndexDataFile.c_str(), aIdxList[k]))
		{
			return false;
		}

		aIndexingData.push_back(data);
	}

	return true;
}



bool MatchFileFeature(vector<DCMFileIndexingData> &aFeat, DCMFileFeat f, string &strMatchedFile)
{
	for (size_t k = 0; k != aFeat.size(); ++k)
	{
		if (FileFeatEqual(aFeat.at(k).fileFeat, f))
		{
			strMatchedFile = aFeat.at(k).strFullPath;

			return true;
		}
	}

	return false;
}



ImageRetrieval::ImageRetrieval()
{
}


ImageRetrieval::~ImageRetrieval()
{
}
