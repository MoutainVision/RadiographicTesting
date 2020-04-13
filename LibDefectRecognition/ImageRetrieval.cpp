#include "stdafx.h"
#include "ImageRetrieval.h"

#include "DCMIndexingFile.h"

#include<io.h>
#include<direct.h>

using std::fstream;

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

		while (ifs >> str >> nOff >> nLen)
		{
			aIdxList.push_back(DCMFileIndex(str, nOff, nLen));
		}
	}

	return true;
}

bool FileFeatEqual(const DCMFileFeat &f1, const DCMFileFeat &f2)
{
	return false;
}

bool DefectListEqual(vector<Defect> &aDL1, vector<DefectFeat> &aDL2)
{
	return false;
}


void Search(vector<RetrievalResult> &aResult, DCMFile &dfile, 
	const string &strImgLibrary, 
	const string &strIndexDataFile)
{
	vector<string> aFileList;
	GetFileList(aFileList, strImgLibrary, ".idx");

	vector<DCMFileIndex> aIdxList;
	if (!GetIndexList(aIdxList, aFileList[0].c_str()))
	{
		return;
	}

	RetrievalResult r;

	DCMFileFeat df = dfile.getFileFeature();

	vector<Defect> aDefect;
	DetectDefect(aDefect, dfile.GetBuffer(), dfile.GetWidth(), dfile.GetHeight());

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


ImageRetrieval::ImageRetrieval()
{
}


ImageRetrieval::~ImageRetrieval()
{
}
