#include "stdafx.h"
#include "DCMIndexingFile.h"

#include<fstream>


using std::ifstream;
using std::ofstream;

DCMIndexingFile::DCMIndexingFile()
{
}


DCMIndexingFile::~DCMIndexingFile()
{
}

bool CreateIndexFile(const char *szIndexFile)
{
	ofstream ofs(szIndexFile);
	if (ofs)
		return true;
	else
		return false;
}

bool AppendIndexFile(DCMFileIndex idx, const char *szIndexFile)
{
	ofstream ofs(szIndexFile, ios::app);
	if (ofs)
	{
		ofs << idx.strFullPath << "\t" << idx.nOffset << "\t" << idx.nLength << "\t" << idx.strDefFile <<"\n";

		return true;
	}

	return false;
}

bool LoadIndexFile(vector<DCMFileIndex> &aIdx, const char *szIndexFile)
{
	ifstream ifs(szIndexFile);
	if (ifs)
	{
		DCMFileIndex index;
		while (ifs >> index.strFullPath >> index.nOffset >> index.nLength >> index.strDefFile)
		{
			aIdx.push_back(index);
		}

		return true;
	}

	return false;
}