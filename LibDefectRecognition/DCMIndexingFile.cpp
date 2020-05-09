#include "stdafx.h"
#include "DCMIndexingFile.h"

#include<fstream>


using std::ifstream;


DCMIndexingFile::DCMIndexingFile()
{
}


DCMIndexingFile::~DCMIndexingFile()
{
}


bool LoadIndexFile(vector<DCMFileIndex> &aIdx, const char *szIndexFile)
{
	ifstream ifs(szIndexFile);
	if (ifs)
	{
		DCMFileIndex index;
		while (ifs >> index.strFullPath >> index.nOffset >> index.nLength)
		{
			aIdx.push_back(index);
		}

		return true;
	}

	return false;
}