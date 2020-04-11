#pragma once

#ifdef LIBIMGPROC_EXPORTS
#define LIBIMGPROC_API __declspec(dllexport)
#else
#define LIBIMGPROC_API __declspec(dllimport)
#endif

#include<vector>

using std::vector;


struct LIBIMGPROC_API ImageRect
{
	int xs, xe, ys, ye;

	ImageRect()
		: xs(0)
		, xe(0)
		, ys(0)
		, ye(0)
	{
	}

	ImageRect(int xxs, int xxe, int yys, int yye)
		: xs(xxs)
		, xe(xxe)
		, ys(yys)
		, ye(yye)
	{
	}
};

typedef enum LIBIMGPROC_API EEmbossOperator
{
	EMB_IMAGE_GRADIENT_SOUTH_DETECT,	//�ݶȼ��, �ϣ���ֱ��
	EMB_IMAGE_GRADIENT_EAST_DETECT,		//�ݶȼ��, ��(ˮƽ)
	EMB_IMAGE_GRADIENT_NORTHEAST_DETECT,//�ݶȼ��, ����-����
	EMB_IMAGE_GRADIENT_SOUTHEAST_DETECT,//�ݶȼ��, ����-����

	EMB_IMAGE_DIFFERENCE_EAST_DETECT,		//ƽ�ƺͲ�ֱ�Ե��ǿ, ����ˮƽ��
	EMB_IMAGE_DIFFERENCE_SOUTH_DETECT,		//ƽ�ƺͲ�ֱ�Ե��ǿ, �ϣ���ֱ��
	EMB_IMAGE_DIFFERENCE_NORTHEAST_DETECT,	//ƽ�ƺͲ�ֱ�Ե��ǿ, ����-����
	EMB_IMAGE_DIFFERENCE_SOUTHEAST_DETECT,	//ƽ�ƺͲ�ֱ�Ե��ǿ, ����-����

	EMB_IMAGE_PREWITT_EAST_DETECT,		//Prewitt��Ե��ǿ, ����ˮƽ��
	EMB_IMAGE_PREWITT_SOUTH_DETECT,		//Prewitt��Ե��ǿ, �ϣ���ֱ��
	EMB_IMAGE_PREWITT_NORTHEAST_DETECT, //Prewitt��Ե��ǿ, ����-����
	EMB_IMAGE_PREWITT_SOUTHEAST_DETECT	//Prewitt��Ե��ǿ, ����-����
}EEmbOp;

//�@ȡ��e��
LIBIMGPROC_API void GetKernel(int pKernel[9],	//��e�ˣ�ݔ������
	EEmbOp eop			//�����������
);

//����Ч��
LIBIMGPROC_API bool Emboss(unsigned short *pImg, //�D�񔵓�ָᘣ��Ȟ�ݔ������ݔ��
	int nRow,				//ݔ��D���Д����߶�
	int nCol,				//ݔ��D���Д�������
	EEmbOp eop			//�����������
);
LIBIMGPROC_API bool Emboss(unsigned char *pImg,	//�D�񔵓�ָᘣ��Ȟ�ݔ������ݔ��
	int nRow,				//ݔ��D���Д����߶�
	int nCol,				//ݔ��D���Д�������
	EEmbOp eop			//�����������
);

//16λ�ҶȈD�D�Q��8λ�ҶȈD��������16λ�D�����С�����Ҷ�ֵ
LIBIMGPROC_API bool Convert(unsigned char *pDst,		//ݔ����8λ�ҶȈD��
	unsigned short *pSrc,	//ݔ���16λ�ҶȈD��
	int nW,					//ݔ��D��Č���
	int nP,					//��ݔ���D����ֹ�����
	int nH,					//ݔ��D��ĸ߶�
	long &maxIntensity,		//ݔ��D��ĵ����Ҷ�ֵ
	long &minIntensity		//ݔ��D�����С�Ҷ�ֵ
);

//8λ�ҶȈD�D�Q��16λ�ҶȈD
LIBIMGPROC_API bool Convert(unsigned short *pDst,	//ݔ����16λ�ҶȈD��
	unsigned char *pSrc,		//ݔ���8λ�ҶȈD��
	int nW,					//ݔ��D��Č���
	int nP,					//ݔ��D����ֹ�����
	int nH					//ݔ��D��ĸ߶�
);

//��ֵ�V��
LIBIMGPROC_API bool MedianFiltering(unsigned short *pImg,	//�D�񔵓�ָᘣ��Ȟ�ݔ������ݔ��
	int nW,					//ݔ��D��Č���
	int nH,					//ݔ��D��ĸ߶�
	int nFilterRadius,		//�V���돽
	ImageRect *aoi=NULL
);

//��˹�V��
LIBIMGPROC_API bool GaussianFiltering(unsigned short *pImg, //�D�񔵓�ָᘣ��Ȟ�ݔ������ݔ��
									int nWidth, 	//ݔ��D��Č���
									int nHeight, //ݔ��D��ĸ߶�
									ImageRect *aoi = NULL); //�V������


//������λ׃�Q
LIBIMGPROC_API bool WindowLevelTransform(unsigned short *pImg,	//�D�񔵓�ָᘣ��Ȟ�ݔ������ݔ��
	int nW,					//ݔ��D��Č���
	int nH,					//ݔ��D��ĸ߶�
	int nWinCentre,			//��λ
	int nWindWidth			//����
);

//������˹׃�Q
LIBIMGPROC_API bool Laplacian(unsigned short *pImg,	//�D�񔵓�ָᘣ��Ȟ�ݔ������ݔ��
	int nW,				//ݔ��D��Č���		
	int nH					//ݔ��D��ĸ߶�	
);
LIBIMGPROC_API bool Laplacian(unsigned char *pImg,	//�D�񔵓�ָᘣ��Ȟ�ݔ������ݔ��
	int nW,				//ݔ��D��Č���
	int nH					//ݔ��D��ĸ߶�
);

//�s��
LIBIMGPROC_API bool Resize(unsigned short *&pImg,	//�D�񔵓�ָᘣ��Ȟ�ݔ��Ҳ��ݔ��
	int &nW,					//�D�񌒶ȣ��Ȟ�ݔ��Ҳ��ݔ��
	int &nH,					//�D��߶ȣ��Ȟ�ݔ��Ҳ��ݔ��
	int nNewW,				//ݔ���D�񌒶�
	int nNewH);				//ݔ���D��߶�

//�Ŵ�2��
LIBIMGPROC_API bool ZoomIn2(unsigned short *&pImg,	//�D�񔵓�ָᘣ��Ȟ�ݔ��Ҳ��ݔ��
	int &nW,				//�D�񌒶ȣ��Ȟ�ݔ��Ҳ��ݔ��
	int &nH					//�D��߶ȣ��Ȟ�ݔ��Ҳ��ݔ��
);

//�sС2��
LIBIMGPROC_API bool ZoomOut2(unsigned short *&pImg,	//�D�񔵓�ָᘣ��Ȟ�ݔ��Ҳ��ݔ��
	int &nW,				//�D�񌒶ȣ��Ȟ�ݔ��Ҳ��ݔ��
	int &nH					//�D��߶ȣ��Ȟ�ݔ������ݔ��
);

//형r����D90��
LIBIMGPROC_API bool Rotate90(unsigned short *&pImg,	//�D�񔵓�ָᘣ��Ȟ�ݔ��Ҳ��ݔ��
	int &nW,				//�D�񌒶ȣ��Ȟ�ݔ��Ҳ��ݔ��
	int &nH					//�D��߶ȣ��Ȟ�ݔ������ݔ��
);

//���·��D
LIBIMGPROC_API bool Flip(unsigned short *pImg,	//�D�񔵓�ָᘣ��Ȟ�ݔ��Ҳ��ݔ��
	int nW,					//ݔ��D�񌒶�
	int nH					//ݔ��D��߶�
);

//���ҷ��D
LIBIMGPROC_API bool Mirror(unsigned short *pImg,	//�D�񔵓�ָᘣ��Ȟ�ݔ��Ҳ��ݔ��
	int nW,					//ݔ��D�񌒶�
	int nH					//ݔ��D��߶�
);

//����
LIBIMGPROC_API bool Invert(unsigned short *pImg,	//�D�񔵓�ָᘣ��Ȟ�ݔ��Ҳ��ݔ��
	int nW,					//ݔ��D�񌒶�
	int nH					//ݔ��D��߶�
);

//����
LIBIMGPROC_API bool Crop(unsigned short *&pImg,	//�D�񔵓�ָᘣ��Ȟ�ݔ��Ҳ��ݔ��
	int &nW,				//�D�񌒶ȣ��Ȟ�ݔ��Ҳ��ݔ��
	int &nH,				//�D�񔵓�ָᘣ��Ȟ�ݔ������ݔ��
	int xs, int xe,			//������������ұ߽�
	int ys, int ye			//������������±߽�
);

//��ȡ�Ҷ�����
LIBIMGPROC_API bool GetIntensityCurve(vector<unsigned short> &aIntensity,
	unsigned short *pImg,	//ݔ��D�񔵓�ָ�
	int nW,				//ݔ��D�񌒶�
	int nH,				//ݔ��D��߶�
	int xs, int ys,		//���ͼ������
	int xe, int ye			//�յ�ͼ������
);

//���������
LIBIMGPROC_API bool GetSNR(double &dMean,	//��ֵ
	double &dStd,	//����
	double &dSNR,	//�����
	unsigned short *pImg,	//ݔ��D�񔵓�ָ�
	int nW,					//ݔ��D�񌒶�
	int nH,					//ݔ��D��߶�
	int xs, int ys,			//���ͼ������
	int xe, int ye			//�յ�ͼ������
);

//Ӌ��16λ�ҶȈD���ֱ���D
LIBIMGPROC_API void GetHist(unsigned aF[65536],		//ֱ���D
	unsigned short *pImg,	//ݔ��D�񔵓�ָ�
	int nW,					//ݔ��D�񌒶�
	int nH					//ݔ��D��߶�
);

//Ӌ��8λ�ҶȈD���ֱ���D
LIBIMGPROC_API void GetHist(unsigned aF[256],		//ֱ���D
	unsigned char *pImg,	//ݔ��D�񔵓�ָ�
	int nW,					//�D�񌒶�
	int nP,					//ݔ��D���ֹ�����
	int nH					//ݔ��D��߶�
);

//٤�RУ��
LIBIMGPROC_API bool GammaCorrection(unsigned short *pImg, int nW, int nH, double dGamma,
	unsigned short nDark = 0, unsigned short nBright = 65535);
