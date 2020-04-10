#pragma once

#ifdef LIBIMGIO_EXPORTS
#define LIBIMGIO_API __declspec(dllexport)
#else
#define LIBIMGIO_API __declspec(dllimport)
#endif

#include "RawImageFile.h"

bool LIBIMGIO_API ReadDCMFile(const char *szDCMFile, unsigned short *pImgBuf, unsigned nImgWidth, unsigned nImgHeight);

bool LIBIMGIO_API WriteDCMFile(const char *szDCMFile, unsigned short *pImgBuf, unsigned nImgWidth, unsigned nImgHeight);
