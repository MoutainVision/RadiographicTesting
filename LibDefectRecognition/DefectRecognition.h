#pragma once


#ifdef LIBDEFECTRECOGNITION_EXPORTS
#define LIBDEFECTRECOGNITION_API __declspec(dllexport)
#else
#define LIBDEFECTRECOGNITION_API __declspec(dllimport)
#endif


#include "..\LibImgProc\RawIamge.h"

//ȱ������ö������
enum LIBDEFECTRECOGNITION_API EDefectClass
{
	DEFECt_UNKNOWN = -1,
	DEFECT_CRACK,
	DEFECT_SLAG,
	DEFECT_HOLE,
	DEFECT_INCOMPLETE_FUSION,
	DEFECT_INCOMPLETE_PENETRATION,
	DEFECT_PIT,
	DEFECT_SONIM,
	DEFECT_OXIDE_INCLUSIONS
};

//ȱ�������ṹ��
struct LIBDEFECTRECOGNITION_API DefectFeat
{
	int		area;			//���
	int		peri;			//�ܳ�
	double	peri_area_ratio;//�ܳ�-�����
	int		length;			//����
	int		width;			//���
	double	rndness;		//��Բ��
	double	rectness;		//���ζ�
	int		aveGrey;		//ƽ���Ҷ�
	double	grey_contrast;	//�ҶȶԱȶ�

	DefectFeat()
	{
		area = 0;
		peri = 0;
		peri_area_ratio = 0.0;
		length = 0;
		width = 0;
		rndness = 0.0;
		rectness = 0.0;
		aveGrey = 0;
		grey_contrast = 0.0;
	}
};


//ȱ�ݽṹ��
struct LIBDEFECTRECOGNITION_API Defect
{
	int iClass;

	DefectFeat feat;
	
	unsigned nPt;
	vector<POINT> aPt;

	POINT center;

	Defect()
	{
		iClass = -1;
		nPt = 0;
	}
};

//ȱ�ݼ������ṹ��
struct LIBDEFECTRECOGNITION_API DetectParam
{
	int nFilterRadius;	//�˲��뾶
	int nGreyDiff;		//�ҶȲ���ֵ
	int nMinDefectArea;	//��Сȱ�����
	int nConnectThr;	//��ͨ����ֵ

	DetectParam()
	{
		nFilterRadius = 10;
		nGreyDiff = 3000;
		nMinDefectArea = 2;
		nConnectThr = 1;
	}
	DetectParam(int r, int d, int a, int c)
	{
		nFilterRadius = r;
		nGreyDiff = d;
		nMinDefectArea = a;
		nConnectThr = c;
	}
};

LIBDEFECTRECOGNITION_API bool DetectDefect(vector<Defect> &aDefectList,
	unsigned short *pImg, int nW, int nH,
	ImageRect *pROI = NULL, DetectParam *pParam = NULL);






class DefectRecognition
{
public:
	DefectRecognition();
	~DefectRecognition();
};

