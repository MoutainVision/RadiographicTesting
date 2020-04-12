#ifndef READDCMFILE_H
#define READDCMFILE_H

#include <iostream>
#include <string>

//#ifdef UNICODE
//typedef wchar_t TCHAR;
//#else
//typedef char TCHAR;
//#endif

#include "dcmtk\dcmdata\dctk.h"
//#include "ximage.h"

#include "LibImgIO.h"

#include "appconfig.h"

using namespace std;

class ReadDCMFile
{
public:
    ReadDCMFile();

    static bool readDCMFile(std::string filePath, std::string outFileName, std::string errorStr);

	static bool readDCMFileLib(std::string filePath, std::string outFileName, std::string errorStr);

    static bool readDCMFileLib(std::string filePath, string outFileName, DcmFileNode &info);
};

#endif // READDCMFILE_H
