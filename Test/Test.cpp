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
		//����ʾ����ʾ����������һ�����ݿ⣬�Լ��ڸ����ݿ��Ͻ��в��ز���

		////////////////////////////////////////////////////////////////////////////////////
		//�����������ݿ⡣�����ݿ������¼��������ݹ��ɣ�
		//1. DCMͼ���ļ�����
		//2. ÿ��DCMͼ���ļ���Ӧ��ȱ���ļ�������ȱ�ݼ��������ɣ���ͼ���ļ�ͬ������׺Ϊ.def��
		//3. ͼ�񼯺���ÿ��ͼ���Ļ�����Ϣ�����ɵļ��������ļ���.dat��
		//4. ͼ�񼯺���ÿ��ͼ����������Ϣ���ɵ��ļ���.idx��

		//���������ļ������ļ���¼��Ŀǰ���ݿ���ÿ��ͼ����������Ϣ��DCMFileIndex�����û�������Щ������Ϣȥ�����ļ��������ļ�
		//�ж�ȡ��ͼ����ͼ��������Ϣ���Խ�����ʾ�����صȵ�
		string strIndexFile = "index.idx";
		ofstream ofs(strIndexFile.c_str());

		//ͼ�����������ļ����洢�����ݿ���ÿ��ͼ����ͼ��������Ϣ��DCMFileIndexingData����
		string strIndexDataFile = "DcmIndex.dat";

		//�������������ļ�
		DCMIndexingFile::Create(strIndexDataFile.c_str());

		//��������
		DCMFileIndexingData indexData;

		vector<string> aFileList;
		//GetFileList(aFileList, "D:\\czp\\aobo\\doc\\��Ƭɨ��\\A\\A - 13_11_2015 - 12", ".dcm");
		GetFileList(aFileList, "D:\\czp\\aobo\\DROriginalImages", ".dcm");
		for (size_t k = 0; k < aFileList.size(); ++k)
		{
			//����DCM�ļ�
			DCMFile df(aFileList[k].c_str());
			if (df.IsValid())
			{
				//�����ļ�����
				df.getFileFeature().Output2();

				CxImage dstImg;
				if (df.Convert(dstImg))
				{
					dstImg.Save(string(aFileList[k]+".jpg").c_str(), CXIMAGE_FORMAT_JPG);
				}

				indexData.strFullPath = aFileList.at(k);
				indexData.fileFeat = df.getFileFeature();

				//ȱ�ݼ���
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

					//����ȱ���ļ�
					SaveDefect(aDefList, strDefFile.c_str());
				}

				//�ѵ�ǰ�ļ��Ļ�����Ϣд�����������ļ���
				DCMFileIndex idx;
				DCMIndexingFile::Write(idx, strIndexDataFile.c_str(), indexData);

				//�ѵ�ǰ�ļ���������Ϣд�������ļ���
				ofs << idx.strFullPath << "\t" << idx.nOffset << "\t" << idx.nLength << "\n";
			}
		}

		ofs.close();

		std::cout << std::endl;
		std::cout << std::endl;


		////////////////////////////////////////////////////////////////////////////////////
		//���ز���

		////���������б�
		//vector<DCMFileIndex> aIdx;
		//if (!LoadIndexFile(aIdx, strIndexFile.c_str()))
		//{
		//	return -1;
		//}

		//for (size_t k = 0; k != aIdx.size(); ++k)
		//{
		//	std::cout << aIdx[k].strFullPath << "\t" << aIdx[k].nOffset << "\t" << aIdx[k].nLength << "\n";
		//}

		////������������
		//for (size_t k = 0; k != aIdx.size(); k++)
		//{
		//	DCMFileIndexingData data;
		//	if (DCMIndexingFile::Read(data, strIndexDataFile.c_str(), aIdx[k]))
		//	{
		//		data.fileFeat.Output2();
		//	}
		//}

		//std::cout << std::endl;

		//����DCM�ļ�
		DCMFile df("1-A.dcm");
		if (df.IsValid())
		{
			std::cout << "�����M���У�Ո�ȴ�...\n";
			//����
			vector<RetrievalResult> aRes;
			Search(aRes, df, strIndexFile, strIndexDataFile);

			std::cout << "���ؽY�����Y�����£�\n";
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
