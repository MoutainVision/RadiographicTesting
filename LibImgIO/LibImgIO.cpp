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

	//////   OFCondition oc = fileformat.loadFile("G:\\DCM\\CR\\��ʾ�ļ�2.DCM");

	OFCondition oc = fileformat.loadFile(szDCMFile);
	////    OFCondition oc = fileformat.loadFile("G:\\DCM\\CR\\ACC-AL6-1 90kv.DCM");

	////    OFCondition oc = fileformat.loadFile("G:\\DCM\\CR\\��ʾ�ļ�2.DCM");


	if (oc.good())
	{
		OFString patientName;

		if (fileformat.getDataset()->findAndGetOFString(DCM_PatientName, patientName).good())
		{
			std::cout << "Patient Name:" << patientName << std::endl;
		}

		DcmDataset *dataset = fileformat.getDataset();	//�õ�Dicom�����ݼ������е����ݶ��洢�����ݼ�����
		E_TransferSyntax xfer = dataset->getOriginalXfer();	//�õ������﷨

		OFString patientname;
		dataset->findAndGetOFString(DCM_PatientName, patientname);	//��ȡ��������     //dataset->findAndGetOFString/->findAndGetUint16
		std::cout << "patientName :" << patientname << std::endl;

		unsigned short bit_count(0);
		dataset->findAndGetUint16(DCM_BitsStored, bit_count);	//��ȡ���ص�λ�� bit
		std::cout << "bit_count :" << bit_count << std::endl;

		OFString isRGB;
		dataset->findAndGetOFString(DCM_PhotometricInterpretation, isRGB);//DCMͼƬ��ͼ��ģʽ
		std::cout << "isrgb :" << isRGB << std::endl;

		unsigned short img_bits(0);
		dataset->findAndGetUint16(DCM_SamplesPerPixel, img_bits);	//��������ռ�ö���byte
		std::cout << "img_bits :" << img_bits << std::endl;
		//DicomImage* img_xfer = new DicomImage(xfer, 0, 0, 1);		//�ɴ����﷨�õ�ͼ���֡

		unsigned short m_width;		//��ȡͼ��Ĵ����
		unsigned short m_height;
		dataset->findAndGetUint16(DCM_Rows, m_height);
		dataset->findAndGetUint16(DCM_Columns, m_width);
		std::cout << "width :" << m_width << std::endl;
		std::cout << "height " << m_height << std::endl;

		unsigned short center, width;  //��ȡԴͼ���еĴ�λ�ʹ���
		dataset->findAndGetUint16(DCM_WindowCenter, center);
		dataset->findAndGetUint16(DCM_WindowWidth, width);

		DcmElement* element = NULL;    //��ȡdcm�е�����ֵ
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
