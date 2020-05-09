// LibImgIO.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "LibImgIO.h"

#include"dcmtk\config\osconfig.h"

//
//bool ReadDCMFile(const char *szDCMFile, unsigned short *pImgBuf, unsigned &nImgWidth, unsigned &nImgHeight)
//{
//	DcmFileFormat fileformat;
//
//	OFCondition oc = fileformat.loadFile(szDCMFile);
//
//	if (oc.good())
//	{
//		//OFString patientName;
//
//		//if (fileformat.getDataset()->findAndGetOFString(DCM_PatientName, patientName).good())
//		//{
//		//	std::cout << "Patient Name:" << patientName << std::endl;
//		//}
//
//		DcmDataset *dataset = fileformat.getDataset();	//�õ�Dicom�����ݼ������е����ݶ��洢�����ݼ�����
//		
//		//E_TransferSyntax xfer = dataset->getOriginalXfer();	//�õ������﷨
//
//		//OFString patientname;
//		//dataset->findAndGetOFString(DCM_PatientName, patientname);	//��ȡ��������     //dataset->findAndGetOFString/->findAndGetUint16
//		//std::cout << "patientName :" << patientname << std::endl;
//
//		unsigned short bit_count(0);
//		dataset->findAndGetUint16(DCM_BitsStored, bit_count);	//��ȡ���ص�λ�� bit
//		//std::cout << "bit_count :" << bit_count << std::endl;
//
//		OFString isRGB;
//		dataset->findAndGetOFString(DCM_PhotometricInterpretation, isRGB);//DCMͼƬ��ͼ��ģʽ
//		//std::cout << "isrgb :" << isRGB << std::endl;
//
//		unsigned short img_bits(0);
//		dataset->findAndGetUint16(DCM_SamplesPerPixel, img_bits);	//��������ռ�ö���byte
//		//std::cout << "img_bits :" << img_bits << std::endl;
//		//DicomImage* img_xfer = new DicomImage(xfer, 0, 0, 1);		//�ɴ����﷨�õ�ͼ���֡
//
//		unsigned short m_width;		//��ȡͼ��Ĵ����
//		unsigned short m_height;
//		dataset->findAndGetUint16(DCM_Rows, m_height);
//		dataset->findAndGetUint16(DCM_Columns, m_width);
//		//std::cout << "width :" << m_width << std::endl;
//		//std::cout << "height " << m_height << std::endl;
//
//		unsigned short center, width;  //��ȡԴͼ���еĴ�λ�ʹ���
//		dataset->findAndGetUint16(DCM_WindowCenter, center);
//		dataset->findAndGetUint16(DCM_WindowWidth, width);
//
//		DcmElement* element = NULL;    //��ȡdcm�е�����ֵ
//		OFCondition result = dataset->findAndGetElement(DCM_PixelData, element);
//		if (result.bad() || element == NULL)
//		{
//			//std::cout << "findAndGetElement result  bad. " << std::endl;
//			return false;
//		}
//
//		//std::cout << element->getLength() << std::endl;
//
//		Uint16* pixData16;
//		result = element->getUint16Array(pixData16);
//		if (result.bad())
//		{
//			//std::cout << "getUint16Array result  bad. " << std::endl;
//			return false;
//		}
//
//		
//		
//
//		return true;
//	}
//
//	return false;
//}
//
//
//bool WriteDCMFile(const char *szDCMFile, unsigned short *pImgBuf, unsigned nImgWidth, unsigned nImgHeight)
//{
//	return false;
//}
//

bool DCMFile::Load(const char *szSrcFileName)
{
	DcmFileFormat fileformat;

	OFCondition oc = fileformat.loadFile(szSrcFileName);

	if (oc.good())
	{
		DcmDataset *dataset = fileformat.getDataset();	//�õ�Dicom�����ݼ������е����ݶ��洢�����ݼ�����

		unsigned short bit_count(0);
		OFCondition result = dataset->findAndGetUint16(DCM_BitsStored, bit_count);	//��ȡ���ص�λ�� bit
		//std::cout << "bit_count :" << bit_count << std::endl;

		OFString isRGB;
		result = dataset->findAndGetOFString(DCM_PhotometricInterpretation, isRGB);//DCMͼƬ��ͼ��ģʽ
		//std::cout << "isrgb :" << isRGB << std::endl;

		unsigned short img_bits(0);
		result = dataset->findAndGetUint16(DCM_SamplesPerPixel, img_bits);	//��������ռ�ö���byte
		//std::cout << "img_bits :" << img_bits << std::endl;
		//DicomImage* img_xfer = new DicomImage(xfer, 0, 0, 1);		//�ɴ����﷨�õ�ͼ���֡

		unsigned short m_width;		//��ȡͼ��Ĵ����
		unsigned short m_height;
		result = dataset->findAndGetUint16(DCM_Rows, m_height);
		if (result.bad())
		{
			std::cout << "Failed to retrieve the height!" << std::endl;
		}
		result = dataset->findAndGetUint16(DCM_Columns, m_width);
		if (result.bad())
		{
			std::cout << "Failed to retrieve the width!" << std::endl;
		}
		//std::cout << "width :" << m_width << std::endl;
		//std::cout << "height " << m_height << std::endl;

		unsigned short center, width;  //��ȡԴͼ���еĴ�λ�ʹ���
		result = dataset->findAndGetUint16(DCM_WindowCenter, center);
		if (result.bad())
		{
			std::cout << "Failed to retrieve the window centre!" << std::endl;
			std::cout << string(result.text()) << std::endl;
		}
		result = dataset->findAndGetUint16(DCM_WindowWidth, width);
		if (result.bad())
		{
			std::cout << "Failed to retrieve the window width!" << std::endl;
			std::cout << string(result.text()) << std::endl;
		}

		DcmElement* element = NULL;    //��ȡdcm�е�����ֵ
		result = dataset->findAndGetElement(DCM_PixelData, element);
		if (result.bad() || element == NULL)
		{
			std::cout << "findAndGetElement result  bad. " << std::endl;
			return false;
		}

		//std::cout << element->getLength() << std::endl;
		unsigned nElemLen = element->getLength();

		Uint16* pixData16;
		result = element->getUint16Array(pixData16);
		if (result.bad())
		{
			std::cout << "getUint16Array result  bad. " << std::endl;
			return false;
		}

		Release();

		m_nImgWidth = m_width;
		m_nImgHeight = m_height;
		size_t N = (size_t)m_nImgWidth*m_nImgHeight;
		m_pImgBuf = new unsigned short[N];
		memcpy(m_pImgBuf, pixData16, N * sizeof(unsigned short));
		m_nImgBPP = bit_count;
		if (0 == center && 0 == width)
		{
			unsigned short nMin = 65535, nMax = 0;
			for (size_t n = 0; n < N; n++)
			{
				if (m_pImgBuf[n] < nMin)
				{
					nMin = m_pImgBuf[n];
				}
				if (m_pImgBuf[n] > nMax)
				{
					nMax = m_pImgBuf[n];
				}
			}

			m_nWinWidth = (nMax - nMin + 1) / 2;
			m_nWinCentre= (nMax + nMin + 1) / 2;
		}
		else
		{ 
			m_nWinCentre = center;
			m_nWinWidth = width;
		}
		m_nDataLength = nElemLen;
		m_nSamplesPerPixel = img_bits;
		m_strPhotoInterp = isRGB.c_str();

		//memset(pixData16, 0, nElemLen / 2);
		//fileformat.saveFile("hhh.dcm");
	
		m_dcmFileFmt = fileformat;

		return true;
	}

	return false;
}

bool DCMFile::Save(const char *szDstFileName)
{
	OFCondition oc = m_dcmFileFmt.putUint16Array(m_pImgBuf, m_nDataLength);
	DcmDataset *dataset = m_dcmFileFmt.getDataset();	//�õ�Dicom�����ݼ������е����ݶ��洢�����ݼ�����
	if (NULL == dataset)
	{
		return false;
	}

	DcmElement* element = NULL;    //��ȡdcm�е�����ֵ
	OFCondition result = dataset->findAndGetElement(DCM_PixelData, element);
	if (result.bad() || element == NULL)
	{
		return false;
	}

	Uint16* pixData16;
	result = element->getUint16Array(pixData16);
	if (result.bad())
	{
		return false;
	}

	if (element->getLength() != m_nDataLength)
	{
		//std::cout << "Failed to save due to inequal length!" << std::endl;
		return false;
	}

	memcpy(pixData16, m_pImgBuf, m_nDataLength);

	oc = m_dcmFileFmt.saveFile(szDstFileName);
	if (!oc.good())
	{
		//std::cout << "Failed to save!" << std::endl;
		return false;
	}

	return true;
}

bool DCMFile::Convert(CxImage &dstImg)
{
	if (IsValid())
	{
		dstImg.Create(m_nImgWidth, m_nImgHeight, 8);
		
		if (dstImg.IsValid())
		{
			unsigned short min = 65535, max = 0;
			for (unsigned short y = 0; y < m_nImgHeight; y++)
				for (unsigned short x = 0; x < m_nImgWidth; x++)
				{
					if (m_pImgBuf[y*m_nImgWidth + x] > max)
					{
						max = m_pImgBuf[y*m_nImgWidth + x];
					}

					if (m_pImgBuf[y*m_nImgWidth + x] < min)
					{
						min = m_pImgBuf[y*m_nImgWidth + x];
					}
				}

			//std::cout << min << "\t" << max << std::endl;


			RGBQUAD rgb[256];
			for (int k = 0; k < 256; k++)
			{
				rgb[k].rgbBlue = rgb[k].rgbGreen = rgb[k].rgbRed = k;
			}
			dstImg.SetPalette(rgb, 256);

			unsigned char *pimg = dstImg.GetBits();
			unsigned nPitch = dstImg.GetEffWidth();
			double dRatio = 255.0 / (max - min);
			for (unsigned short y = 0; y < m_nImgHeight; y++)
				for (unsigned short x = 0; x < m_nImgWidth; x++)
				{
					pimg[y*nPitch + x] = (unsigned char)((m_pImgBuf[y*m_nImgWidth + x] - min) * dRatio);
				}

			dstImg.Flip();

			return true;
		}
	}

	return false;
}

DCMFileFeat DCMFile::getFileFeature()
{
	DCMFileFeat ffeat;

	if (IsValid())
	{
		ffeat.nBpp = m_nImgBPP;
		ffeat.nW = m_nImgWidth;
		ffeat.nH = m_nImgHeight;
		ffeat.winC = m_nWinCentre;
		ffeat.winW = m_nWinWidth;
		ffeat.nSpp = m_nSamplesPerPixel;
		ffeat.nLen = m_nDataLength;
		//ffeat.strPI = m_strPhotoInterp;

		unsigned aHist[65536];
		memset(aHist, 0, 65536 * sizeof(unsigned));
		unsigned N = m_nImgHeight * m_nImgWidth;
		for (int k = 0; k < N; k++)
		{
			aHist[m_pImgBuf[k]]++;
		}

		unsigned short nMinG = 65535;
		unsigned short nNonZeroMinG = 65535;
		bool bMinFound = false;
		for (int i = 0; i < 65536; i++)
		{
			if (0 < aHist[i])
			{
				if (!bMinFound)
				{
					nMinG = i;
					bMinFound = true;
				}

				if (0 < i)
				{
					nNonZeroMinG = i;
					break;
				}
			}
		}

		unsigned short nMaxG = 0;
		for (int i = 65535; i >= 0; i--)
		{
			if (0 < aHist[i])
			{
				nMaxG = i;
				break;
			}
		}

		double dAveG = 0.0;
		for (int i = 65535; i >= 0; i--)
		{
			if (0 < aHist[i])
			{
				dAveG += aHist[i] * i;
			}
		}
		
		//for (unsigned i = 1; i < N; i++)
		//{
		//	dAveG += m_pImgBuf[i];

		//	if (m_pImgBuf[i] > nMaxG)
		//		nMaxG = m_pImgBuf[i];

		//	if (m_pImgBuf[i] < nMinG)
		//		nMinG = m_pImgBuf[i];
		//}

		ffeat.nMinG = nMinG;
		ffeat.nNonZeroMinG = nNonZeroMinG;
		ffeat.nMaxG = nMaxG;
		ffeat.nAveG = (unsigned short)(dAveG / N);
	}

	return ffeat;
}


bool DCMFile::Match(double &dSimilarity, DCMFile &file)
{
	if (!IsValid())
	{
		return false;
	}

	//ImageRect aoi()

	return false;
}