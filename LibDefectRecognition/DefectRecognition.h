#pragma once


#ifdef LIBDEFECTRECOGNITION_EXPORTS
#define LIBDEFECTRECOGNITION_API __declspec(dllexport)
#else
#define LIBDEFECTRECOGNITION_API __declspec(dllimport)
#endif


#include "..\LibImgProc\RawIamge.h"

//缺陷类型枚举类型
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

#pragma pack(1)
//缺陷特征结构体
struct LIBDEFECTRECOGNITION_API DefectFeat
{
	int		area;			//面积
	int		peri;			//周长
	double	peri_area_ratio;//周长-面积比
	int		length;			//长度
	int		width;			//宽度
	double	rndness;		//似圆度
	double	rectness;		//矩形度
	int		aveGrey;		//平均灰度
	double	grey_contrast;	//灰度对比度

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
#pragma pack()

//缺陷结构体
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

//缺陷检测参数结构体
struct LIBDEFECTRECOGNITION_API DetectParam
{
	int nFilterRadius;	//滤波半径
	int nGreyDiff;		//灰度差阈值
	int nMinDefectArea;	//最小缺陷面积
	int nConnectThr;	//连通性阈值

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

