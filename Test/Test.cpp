// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include<vector>
#include<string>

//#include "..\LibImgIO\LibImgIO.h"
//
//#include "ximage.h"

#include "..\LibDefectRecognition\LibDefectRecognition.h"


using namespace std;

int main()
{
    std::cout << "Hello World!\n"; 

	if (true)
	{
		DCMFile df;
		if (df.Load("00036.dcm"))
		{
			std::cout << df.GetBPP() << "\t" << df.GetWindowCenter() << "\t" << df.GetWindowWidth() << std::endl;

			CxImage dstImg;
			if (df.Convert(dstImg))
			{
				dstImg.Save("00036.bmp", CXIMAGE_FORMAT_BMP);
			}

			memset(df.GetBuffer(), 0, df.GetDataLength() / 2);

			if (!df.Save("00036copy.dcm"))
			{
				std::cout << "Failed to save DCM file." << std::endl;
			}
		}
	}

	DCMFile df("00036copy.dcm");
	CxImage ximg;
	df.Convert(ximg);
	ximg.Save("00036copy.jpg", CXIMAGE_FORMAT_JPG);

	if (false)
	{
		vector<string> aFileList;
		GetFileList(aFileList, "D:\\czp\\aobo\\doc\\µ×Æ¬É¨Ãè\\A", ".dcm");
		for (size_t k = 0; k < aFileList.size(); ++k)
		{
			DCMFile df(aFileList[k].c_str());
			if (df.IsValid())
			{
				df.getFileFeature().Output2();

				CxImage dstImg;
				if (df.Convert(dstImg))
				{				
					dstImg.Save(string(aFileList[k]+".jpg").c_str(), CXIMAGE_FORMAT_JPG);
				}
			}
		}
	}

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
