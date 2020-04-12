#pragma once

#ifdef LIBIMGPROC_EXPORTS
#define LIBIMGPROC_API __declspec(dllexport)
#else
#define LIBIMGPROC_API __declspec(dllimport)
#endif

#include "ImgProc.h"

class LIBIMGPROC_API RawImage
{
private:
	//�D�񌒶Ⱥ͸߶�
	int m_nWidth, m_nHeight;

	//�D�񔵓���ʼ��ַ
	unsigned short *m_pData;

public:
	//���캯��
	RawImage();
	RawImage(int nW, int nH);
	RawImage(const unsigned short *pBuf, int nW, int nH);

	//��������
	virtual ~RawImage(void);

	//����
	bool Create(int nW, int nH);

	//�������xֵ
	bool Create(const unsigned short *pBuf, int nW, int nH);

	//�Д��Ƿ���Ч
	bool IsValid();

	////���d
	//bool Load(const CString& sFile);

	////����
	//bool Save(const CString &sFile);

	//��ؐ
	bool CopyFrom(RawImage &imgSrc);

	//�@ȡ�D�񔵓���ʼ��ַ
	unsigned short* GetData();

	//�@ȡ�D�񌒶�
	int GetWidth();

	//�@ȡ�D��߶�
	int GetHeight();

	//�@ȡ�D�����С���������ֵ
	bool GetMinMaxIntensity(unsigned short &minInt, unsigned short &maxInt);

	//�@ȡ�o�������c������ֵ
	bool GetIntensity(unsigned short &nInt, unsigned x, unsigned y);

	//�@ȡ�o�������еĻҶ�����
	bool GetRowIntensityCurve(vector<unsigned short> &aIntensity, int iRow);

	//�@ȡ�o�������еĻҶ�����
	bool GetColumnIntensityCurve(vector<unsigned short> &aIntensity, int iCol);

	//�s��
	bool Zoom(double dFactor = 2);

	//���·��D
	bool MirrorV();

	//���ҷ��D
	bool MirrorH();

	//����
	bool Clip(int xs, int xe, int ys, int ye);

	//����
	bool Reverse();

	//Laplace׃�Q
	bool Laplacian_Transform();

	//������λ׃�Q
	bool Window_Level_Transform(int nWinCentre, int nWinWidth);

	//����׃�Q
	bool Emboss_Transform(EEmbOp eop = EMB_IMAGE_GRADIENT_SOUTH_DETECT);

	//16λ�ҶȈD�D�Q��8λ�ҶȈD
	bool ConvertToMono8(unsigned char *pDst, int nW, int nP, int nH);

	//��ֵ�V��
	bool Median(int nFilterRadius = 3, ImageRect *pROI=NULL);

	//��˹�V��
	bool Gaussian(ImageRect *aoi=NULL);

};
