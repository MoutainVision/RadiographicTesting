#ifndef READDCMFILE_H
#define READDCMFILE_H

#include <iostream>
#include <string>

#include "dcmtk\dcmdata\dctk.h"
#include "LibCore.h"

using namespace std;

class ReadDCMFile
{
public:
    ReadDCMFile();

    static bool readDCMFile(std::string filePath, std::string outFileName, std::string errorStr);
};

#endif // READDCMFILE_H
