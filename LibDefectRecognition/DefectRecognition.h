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
	DEFECt_UNKNOWN = -1,//δ֪����
	DEFECT_CRACK, //����
	DEFECT_SLAG,//����
	DEFECT_HOLE,//����
	DEFECT_INCOMPLETE_FUSION,//δ�ۺ�
	DEFECT_INCOMPLETE_PENETRATION,//δ��͸
	DEFECT_PIT,//����
	DEFECT_SONIM,//�ǽ�������
	DEFECT_OXIDE_INCLUSIONS//����Ĥ����
};

#pragma pack(1)
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

	DefectFeat(const DefectFeat &rhs)
	{
		area = rhs.area;
		peri = rhs.peri;
		peri_area_ratio = rhs.peri_area_ratio;
		length = rhs.length;
		width = rhs.width;
		rndness = rhs.rndness;
		rectness = rhs.rectness;
		aveGrey = rhs.aveGrey;
		grey_contrast = rhs.grey_contrast;
	}

	DefectFeat& operator=(const DefectFeat &rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		area = rhs.area;
		peri = rhs.peri;
		peri_area_ratio = rhs.peri_area_ratio;
		length = rhs.length;
		width = rhs.width;
		rndness = rhs.rndness;
		rectness = rhs.rectness;
		aveGrey = rhs.aveGrey;
		grey_contrast = rhs.grey_contrast;

		return *this;
	}

	bool operator==(DefectFeat &rhs)
	{
		if (this == &rhs)
		{
			return true;
		}

		if (area != rhs.area ||
			peri != rhs.peri ||
			peri_area_ratio != rhs.peri_area_ratio ||
			length != rhs.length ||
			width != rhs.width ||
			rndness != rhs.rndness ||
			rectness != rhs.rectness ||
			aveGrey != rhs.aveGrey ||
			grey_contrast != rhs.grey_contrast)
		{
			return false;
		}

		return true;
	}


};
#pragma pack()

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

	Defect(const Defect &rhs)
	{
		iClass = rhs.iClass;
		feat = rhs.feat;
		nPt = rhs.nPt;
		center = rhs.center;

		aPt.assign(rhs.aPt.begin(), rhs.aPt.end());

	}

	Defect& operator=(const Defect &rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		iClass = rhs.iClass;
		feat = rhs.feat;
		nPt = rhs.nPt;
		center = rhs.center;

		aPt.clear();
		aPt.assign(rhs.aPt.begin(), rhs.aPt.end());

		return *this;
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

//���ͼ���е�ȱ�ݣ�����������б���
LIBDEFECTRECOGNITION_API bool DetectDefect(vector<Defect> &aDefectList,
	unsigned short *pImg, int nW, int nH,
	ImageRect *pROI = NULL, DetectParam *pParam = NULL);

//����ͼ���ȱ���б���Ӧ���ļ��У��Թ�֮��Ĳ�ѯ֮��
LIBDEFECTRECOGNITION_API bool SaveDefect(vector<Defect> &aDefectList, const char *szDefFile = "Defects.def");

//���ļ��ж�ȡ��ͼ���ȱ���б�
LIBDEFECTRECOGNITION_API bool LoadDefect(vector<Defect> &aDefectList, const char *szDefFile = "Defects.def");



class DefectRecognition
{
public:
	DefectRecognition();
	~DefectRecognition();
};

