#include "stdafx.h"
#include "ImageRetrieval.h"

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


bool CreateIndexingFile(const char *szIndexFile)
{
	return false;
}

bool Append(DCMFileIndexingData data, const char *szIndexFile)
{
	return false;
}

bool GetIndexingData(vector<DCMFileIndexingData> &aIdxData, const char*szIndexFile)
{
	return false;
}

bool FileFeatEqual(const DCMFileFeat &f1, const DCMFileFeat &f2)
{
	return false;
}

bool DefectListEqual(vector<Defect> &aDL1, vector<Defect> &aDL2)
{
	return false;
}


void Search(vector<RetrievalResult> &aResult, DCMFile &dfile, const string &strImgLibrary)
{
	vector<string> aFileList;
	GetFileList(aFileList, strImgLibrary, ".idx");

	vector<DCMFileIndexingData> aIdxData;
	if (!GetIndexingData(aIdxData, aFileList[0].c_str()))
	{
		return;
	}

	RetrievalResult r;

	DCMFileFeat df = dfile.getFileFeature();
	for (size_t k = 0; k != aIdxData.size(); k++)
	{
		//compare df and aIdxData.at(k).fileFeat
		if (FileFeatEqual(df, aIdxData.at(k).fileFeat))
		{
			r.index = k;
			r.dSimilarity = 1.0;

			return;
		}
	}

	vector<Defect> aDefect;

	DetectDefect(aDefect, dfile.GetBuffer(), dfile.GetWidth(), dfile.GetHeight());
	for (size_t k = 0; k != aIdxData.size(); k++)
	{
		//compare aDefect and aIdxData.at(k).a
		if (DefectListEqual(aDefect, aIdxData.at(k).aDefectList))
		{
			r.index = k;
			r.dSimilarity = 1.0;

			return;
		}
	}

	//image matching below...

	return;
}


ImageRetrieval::ImageRetrieval()
{
}


ImageRetrieval::~ImageRetrieval()
{
}
