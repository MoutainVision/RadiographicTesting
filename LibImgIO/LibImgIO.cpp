// LibImgIO.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "LibImgIO.h"
//
#include"dcmtk\config\osconfig.h"

#include"dcmtk\dcmdata\dctk.h"

//#include"dcmtk\dcmimage\diargimg.h"

//
//#if _DEBUG
//#pragma comment(lib,"..\\Dependencies\\dcmtk-3.6.5\\lib\\Debug\\dcmtk.lib")
//#else
//#pragma comment(lib,"..\\Dependencies\\dcmtk-3.6.5\\lib\\Release\\dcmtk.lib")
//#endif // _DEBUG
//

bool ReadDCMFile(const char *szDCMFile, unsigned short *pImgBuf, unsigned nImgWidth, unsigned nImgHeight)
{
	DcmFileFormat fileformat;

	//////   OFCondition oc = fileformat.loadFile("G:\\DCM\\ACC-AL6-1 90kv.DCM");

	//////   OFCondition oc = fileformat.loadFile("G:\\DCM\\CR\\演示文件2.DCM");

	OFCondition oc = fileformat.loadFile(szDCMFile);
	////    OFCondition oc = fileformat.loadFile("G:\\DCM\\CR\\ACC-AL6-1 90kv.DCM");

	////    OFCondition oc = fileformat.loadFile("G:\\DCM\\CR\\演示文件2.DCM");


	if (oc.good())
	{
		OFString patientName;

		if (fileformat.getDataset()->findAndGetOFString(DCM_PatientName, patientName).good())
		{
			std::cout << "Patient Name:" << patientName << std::endl;
		}

		DcmDataset *dataset = fileformat.getDataset();	//得到Dicom的数据集，所有的数据都存储在数据集当中
		E_TransferSyntax xfer = dataset->getOriginalXfer();	//得到传输语法

		OFString patientname;
		dataset->findAndGetOFString(DCM_PatientName, patientname);	//获取病人姓名     //dataset->findAndGetOFString/->findAndGetUint16
		std::cout << "patientName :" << patientname << std::endl;

		unsigned short bit_count(0);
		dataset->findAndGetUint16(DCM_BitsStored, bit_count);	//获取像素的位数 bit
		std::cout << "bit_count :" << bit_count << std::endl;

		OFString isRGB;
		dataset->findAndGetOFString(DCM_PhotometricInterpretation, isRGB);//DCM图片的图像模式
		std::cout << "isrgb :" << isRGB << std::endl;

		unsigned short img_bits(0);
		dataset->findAndGetUint16(DCM_SamplesPerPixel, img_bits);	//单个像素占用多少byte
		std::cout << "img_bits :" << img_bits << std::endl;
		//DicomImage* img_xfer = new DicomImage(xfer, 0, 0, 1);		//由传输语法得到图像的帧

		unsigned short m_width;		//获取图像的窗宽高
		unsigned short m_height;
		dataset->findAndGetUint16(DCM_Rows, m_height);
		dataset->findAndGetUint16(DCM_Columns, m_width);
		std::cout << "width :" << m_width << std::endl;
		std::cout << "height " << m_height << std::endl;

		unsigned short center, width;  //获取源图像中的窗位和窗宽
		dataset->findAndGetUint16(DCM_WindowCenter, center);
		dataset->findAndGetUint16(DCM_WindowWidth, width);

		DcmElement* element = NULL;    //读取dcm中的像素值
		OFCondition result = dataset->findAndGetElement(DCM_PixelData, element);
		if (result.bad() || element == NULL)
		{
			std::cout << "findAndGetElement result  bad. " << std::endl;
		}

		std::cout << element->getLength() << std::endl;


		Uint16* pixData16;
		result = element->getUint16Array(pixData16);
		if (result.bad())
		{
			std::cout << "getUint16Array result  bad. " << std::endl;
		}
	}

	return false;
}


bool WriteDCMFile(const char *szDCMFile, unsigned short *pImgBuf, unsigned nImgWidth, unsigned nImgHeight)
{
	return false;
}
