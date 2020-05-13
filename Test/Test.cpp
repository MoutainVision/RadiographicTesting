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
	if (true)
	{
		DCMFile df;
		//if (df.Load("2.16.840.1.114226.1423554068.2992184.110.13.dcm"))
		if (df.Load("1-A.dcm"))
		{
			std::cout << "File feature vector of the input image is: \n";
			df.getFileFeature().Output2();

			//std::cout << df.GetBPP() << "\t" << df.GetWindowCenter() << "\t" << df.GetWindowWidth() << std::endl;

			CxImage dstImg;
			if (df.Convert(dstImg))
			{
				dstImg.Save("mono8.bmp", CXIMAGE_FORMAT_BMP);
			}

			//memset(df.GetBuffer(), 0, df.GetDataLength() / 2);

			//if (!df.Save("copy.dcm"))
			//{
			//	std::cout << "Failed to save DCM file." << std::endl;
			//}
		}
	}

	//DCMFile df("00036copy.dcm");
	//CxImage ximg;
	//df.Convert(ximg);
	//ximg.Save("00036copy.jpg", CXIMAGE_FORMAT_JPG);

	if (true)
	{
		std::cout << "\n";
		std::cout << "Database in building, please wait...\n\n";
		//查重测试

		////////////////////////////////////////////////////////////////////////////////////
		//第一步：建立数据库，涉及以下数据读写
		//1. DCM图像的文件特征
		//2. 将DCM图像检测到的缺陷（如有的话）保存到同名的缺陷文件中（后缀名：.def）
		//3. 将DCM图像的索引数据写入到索引数据文件中（后缀：.dat）
		//4. 将每个图像的索引信息写入到索引文件中（后缀：.idx）

		//创建空的索引信息文件
		string strIndexFile = "index.idx";
		ofstream ofs(strIndexFile.c_str());


		//创建空的索引数据文件
		string strIndexDataFile = "DcmIndex.dat";
		DCMIndexingFile::Create(strIndexDataFile.c_str());

		//索引数据
		DCMFileIndexingData indexData;

		vector<string> aFileList;
		//GetFileList(aFileList, "D:\\czp\\aobo\\doc\\��Ƭɨ��\\A\\A - 13_11_2015 - 12", ".dcm");
		GetFileList(aFileList, "D:\\czp\\aobo\\DROriginalImages", ".dcm");
		for (size_t k = 0; k < aFileList.size(); ++k)
		{
			//加载文件
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

				//检测缺陷
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

					//保存缺陷到外部文件中
					SaveDefect(aDefList, strDefFile.c_str());
				}

				//将索引数据写入索引数据文件
				DCMFileIndex idx;
				DCMIndexingFile::Write(idx, strIndexDataFile.c_str(), indexData);

				//将索引信息写入索引文件
				ofs << idx.strFullPath << "\t" << idx.nOffset << "\t" << idx.nLength << "\n";
			}
		}

		ofs.close();

		std::cout << std::endl;
		std::cout << std::endl;


		////////////////////////////////////////////////////////////////////////////////////
		//查重开始

		////加载索引文件
		//vector<DCMFileIndex> aIdx;
		//if (!LoadIndexFile(aIdx, strIndexFile.c_str()))
		//{
		//	return -1;
		//}

		//for (size_t k = 0; k != aIdx.size(); ++k)
		//{
		//	std::cout << aIdx[k].strFullPath << "\t" << aIdx[k].nOffset << "\t" << aIdx[k].nLength << "\n";
		//}

		////打印所有索引数据
		//for (size_t k = 0; k != aIdx.size(); k++)
		//{
		//	DCMFileIndexingData data;
		//	if (DCMIndexingFile::Read(data, strIndexDataFile.c_str(), aIdx[k]))
		//	{
		//		data.fileFeat.Output2();
		//	}
		//}

		//std::cout << std::endl;

		//打开待查重图像
		DCMFile df("1-A.dcm");
		if (df.IsValid())
		{
			std::cout << "Duplication check is in progress, please wait...\n";

			int w = df.GetWidth();
			int h = df.GetHeight();

			DetectParam dp;
			ImageRect aoi(w/20*9, w/20*11, h/20*9, h/20*11);
			vector<RetrievalResult> aRes;
			Search(aRes, df, aoi, dp, strIndexFile, strIndexDataFile);

			std::cout << "Duplication check finished. The result are as below: \n";
			for (size_t k = 0; k != aRes.size(); ++k)
			{
				std::cout << k << "\t" << aRes[k].strMatchedFile << "\t" << aRes[k].dSimilarity*100 << "%" << std::endl;
			}
		}
		else
		{
			std::cout << "Failed to opened the specified image:\n";
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
