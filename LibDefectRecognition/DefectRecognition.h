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
	DEFECt_UNKNOWN = -1,//未知类型
	DEFECT_CRACK, //裂纹
	DEFECT_SLAG,//夹钨
	DEFECT_HOLE,//气孔
	DEFECT_INCOMPLETE_FUSION,//未熔合
	DEFECT_INCOMPLETE_PENETRATION,//未焊透
	DEFECT_PIT,//凹坑
	DEFECT_SONIM,//非金属夹杂
	DEFECT_OXIDE_INCLUSIONS//氧化膜夹杂
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

//缺陷检测参数结构体
struct LIBDEFECTRECOGNITION_API DetectParam
{
	int nFilterRadius;	//滤波半径
	int nGreyDiff;		//灰度差阈值
	int nMinDefectArea;	//最小缺陷面积
	int nConnectThr;	//连通性阈值

	DetectParam()
	{
		nFilterRadius = 5;
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

//检测图像中的缺陷，并将结果用列表返回
LIBDEFECTRECOGNITION_API bool DetectDefect(vector<Defect> &aDefectList,
	unsigned short *pImg, int nW, int nH,
	ImageRect *pROI = NULL, DetectParam *pParam = NULL);

//保存图像的缺陷列表到对应的文件中，以供之后的查询之用
LIBDEFECTRECOGNITION_API bool SaveDefect(vector<Defect> &aDefectList, const char *szDefFile = "Defects.def");

//从文件中读取出图像的缺陷列表
LIBDEFECTRECOGNITION_API bool LoadDefect(vector<Defect> &aDefectList, const char *szDefFile = "Defects.def");



class DefectRecognition
{
public:
	DefectRecognition();
	~DefectRecognition();
};

