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
#include "ximage.h"

using namespace std;

class ReadDCMFile
{
public:
    ReadDCMFile();

    static bool readDCMFile(std::string filePath, std::string outFileName, std::string errorStr);
};

#endif // READDCMFILE_H
