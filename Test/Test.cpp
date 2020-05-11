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

	if (true)
	{
		//以下示例演示了如何生成一个数据库，以及在该数据库上进行查重操作

		////////////////////////////////////////////////////////////////////////////////////
		//首先生成数据库。该数据库由以下几部分数据构成：
		//1. DCM图像文件集合
		//2. 每个DCM图像文件对应的缺陷文件（进行缺陷检测后生成，与图像文件同名，后缀为.def）
		//3. 图像集合中每个图像的基本信息所构成的检索数据文件（.dat）
		//4. 图像集合中每个图像的索引信息构成的文件（.idx）

		//生成索引文件。该文件记录了目前数据库中每个图像的索引信息（DCMFileIndex），用户根据这些索引信息去下面的检索数据文件
		//中读取该图像的图像基本信息，以进行显示、查重等等
		string strIndexFile = "index.idx";
		ofstream ofs(strIndexFile.c_str());

		//图像检索数据文件。存储了数据库中每个图像的图像基本信息（DCMFileIndexingData）。
		string strIndexDataFile = "DcmIndex.dat";

		//创建检索数据文件
		DCMIndexingFile::Create(strIndexDataFile.c_str());

		//检索数据
		DCMFileIndexingData indexData;

		vector<string> aFileList;
		//GetFileList(aFileList, "D:\\czp\\aobo\\doc\\底片扫描\\A\\A - 13_11_2015 - 12", ".dcm");
		GetFileList(aFileList, "D:\\czp\\aobo\\DROriginalImages", ".dcm");
		for (size_t k = 0; k < aFileList.size(); ++k)
		{
			//加载DCM文件
			DCMFile df(aFileList[k].c_str());
			if (df.IsValid())
			{
				//输出文件特征
				df.getFileFeature().Output2();

				CxImage dstImg;
				if (df.Convert(dstImg))
				{				
					dstImg.Save(string(aFileList[k]+".jpg").c_str(), CXIMAGE_FORMAT_JPG);
				}

				indexData.strFullPath = aFileList.at(k);
				indexData.fileFeat = df.getFileFeature();

				//缺陷检测
				ImageRect aoi(df.GetWidth() / 20 * 10, df.GetWidth() / 20 * 11, df.GetHeight() / 20 * 10, df.GetHeight() / 20 * 11);
				DetectParam dp;
				vector<Defect> aDefList;
				DetectDefect(aDefList, df.GetBuffer(), df.GetWidth(), df.GetHeight(), &aoi, &dp);

				if (!aDefList.empty())
				{
					for (size_t n = 0; n != aDefList.size(); ++n)
					{
						indexData.aDefectList.push_back(aDefList.at(n).feat);
					}

					string strDefFile = aFileList.at(k) + ".def";

					//保存缺陷文件
					SaveDefect(aDefList, strDefFile.c_str());
				}

				//把当前文件的基本信息写入检索数据文件中
				DCMFileIndex idx;
				DCMIndexingFile::Write(idx, strIndexDataFile.c_str(), indexData);

				//把当前文件的索引信息写入所以文件中
				ofs << idx.strFullPath << "\t" << idx.nOffset << "\t" << idx.nLength << "\n";
			}
		}

		ofs.close();

		std::cout << std::endl;
		std::cout << std::endl;


		////////////////////////////////////////////////////////////////////////////////////
		//查重测试

		////加载索引列表
		//vector<DCMFileIndex> aIdx;
		//if (!LoadIndexFile(aIdx, strIndexFile.c_str()))
		//{
		//	return -1;
		//}

		//for (size_t k = 0; k != aIdx.size(); ++k)
		//{
		//	std::cout << aIdx[k].strFullPath << "\t" << aIdx[k].nOffset << "\t" << aIdx[k].nLength << "\n";
		//}

		////加载索引数据
		//for (size_t k = 0; k != aIdx.size(); k++)
		//{
		//	DCMFileIndexingData data;
		//	if (DCMIndexingFile::Read(data, strIndexDataFile.c_str(), aIdx[k]))
		//	{
		//		data.fileFeat.Output2();
		//	}
		//}

		//std::cout << std::endl;

		//加载DCM文件
		DCMFile df("1-A.dcm");
		if (df.IsValid())
		{
			std::cout << "查重進行中，請等待...\n";
			//查重
			vector<RetrievalResult> aRes;
			Search(aRes, df, strIndexFile, strIndexDataFile);

			std::cout << "查重結束，結果如下：\n";
			for (size_t k = 0; k != aRes.size(); ++k)
			{
				std::cout << k << "\t" << aRes[k].strMatchedFile << "\t" << aRes[k].dSimilarity*100 << "%" << std::endl;
			}
		}
	}

	return 0;
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
