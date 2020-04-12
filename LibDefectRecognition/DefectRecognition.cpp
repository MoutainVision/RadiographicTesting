#include "stdafx.h"
#include "DefectRecognition.h"

bool GetBoundingBox(ImageRect &r, const vector<POINT> &aPt, size_t ns, size_t ne)
{
	if (ns > ne || ne >= aPt.size())
	{
		return false;
	}

	r.xs = aPt[ns].x; r.xe = aPt[ns].x;
	r.ys = aPt[ns].y; r.ye = aPt[ns].y;
	for (size_t n = ns + 1; n <= ne; n++)
	{
		if (aPt[n].x < r.xs)	r.xs = aPt[n].x;
		if (aPt[n].x > r.xe)	r.xe = aPt[n].x;
		if (aPt[n].y < r.ys)	r.ys = aPt[n].y;
		if (aPt[n].y > r.ye)	r.ye = aPt[n].y;
	}

	return true;
}

bool GetContourPointSet(vector<POINT> &aContourPt,
	const vector<POINT> &aPt,
	size_t ns, size_t ne)
{
	ImageRect r;
	if (!GetBoundingBox(r, aPt, ns, ne))
	{
		return false;
	}

	int nW = r.xe - r.xs + 1;
	int nH = r.ye - r.ys + 1;

	size_t nWidth = nW;

	size_t nPx = nWidth * nH;

	bool *pMask = nullptr;
	if (nullptr == (pMask = new bool[nPx]))
	{
		return false;
	}
	memset(pMask, false, nPx * sizeof(bool));

	for (size_t n = ns; n <= ne; ++n)
	{
		pMask[(aPt[n].y - r.ys)*nWidth + (aPt[n].x - r.xs)] = true;
	}

	for (size_t n = ns; n <= ne; ++n)
	{
		int x = aPt[n].x - r.xs;
		int y = aPt[n].y - r.ys;

		if (x == 0 ||
			x == nW - 1 ||
			y == 0 ||
			y == nH - 1)
		{
			aContourPt.push_back(aPt[n]);
			continue;
		}

		size_t pos = y * nWidth + x;

		if (!pMask[pos + 1] ||
			!pMask[pos - 1] ||
			!pMask[pos - nWidth - 1] ||
			!pMask[pos - nWidth] ||
			!pMask[pos - nWidth + 1] ||
			!pMask[pos + nWidth - 1] ||
			!pMask[pos + nWidth] ||
			!pMask[pos + nWidth + 1])
		{
			aContourPt.push_back(aPt[n]);
		}
	}

	delete[]pMask;

	return true;
}

bool GetContourPointSet(vector<POINT> &aContourPt, const vector<POINT> &aPt)
{
	if (aPt.empty())
	{
		return false;
	}

	return GetContourPointSet(aContourPt, aPt, 0, aPt.size() - 1);
}


bool RegionTrace(vector<POINT> &aPt, unsigned char *pImg, unsigned nWidth, unsigned nHeight, 
	ImageRect roi, POINT seed, bool *pVisited)
{
	if (NULL == pImg)
	{
		return false;
	}

	int dx[8] = { -1,-1,-1,0,1,1,1,0 };
	int dy[8] = { -1,0,1,1,1,0,-1,-1 };

	vector<POINT> neighborhood;
	neighborhood.push_back(seed);

	vector<POINT> ptTemp;

	pVisited[seed.y*nWidth + seed.x] = true;
	aPt.push_back(seed);
	while (neighborhood.size() > 0)
	{
		POINT pt, next;
		int nN = neighborhood.size();
		for (int i = 0; i < nN; i++)
		{
			pt = neighborhood[i];
			unsigned char nCurGrey = pImg[pt.y*nWidth + pt.x];

			for (int k = 0; k < 8; k++)
			{
				next.x = pt.x + dx[k];
				next.y = pt.y + dy[k];
				int pos = next.y*nWidth + next.x;
				if (next.x >= roi.xs && next.x < roi.xe && 
					next.y >= roi.ys && next.y < roi.ye && 
					pImg[pos] == 255 && !pVisited[pos])
				{
					ptTemp.push_back(next);
					pVisited[pos] = true;
				}
			}

		}//for each neighbor	

		neighborhood.clear();
		neighborhood.insert(neighborhood.end(), ptTemp.begin(), ptTemp.end());
		ptTemp.clear();

		aPt.insert(aPt.end(), neighborhood.begin(), neighborhood.end());

	}//for while

	neighborhood.clear();


	return true;

}

bool CalcDetect(unsigned short *pImg, int nW, int nH, vector<POINT> &aPt, Defect &d)
{
	if (aPt.size() == 0)
		return false;

	double xc = 0, yc = 0;
	double g = 0;
	double dMaxDist = 0.0;
	int k1 = -1, k2 = -1;
	for (unsigned n = 0; n < aPt.size(); n++)
	{
		xc += aPt[n].x;
		yc += aPt[n].y;
		g += pImg[aPt[n].y*nW + aPt[n].x];

		for (unsigned m = n+1; m < aPt.size(); m++)
		{
			double curDist = (aPt[n].x - aPt[m].x)*(aPt[n].x - aPt[m].x) +
							 (aPt[n].y - aPt[m].y)*(aPt[n].y - aPt[m].y);

			if (dMaxDist < curDist)
			{
				dMaxDist = curDist;
				k1 = n;
				k2 = m;
			}
		}
	}
	
	xc /= aPt.size();
	yc /= aPt.size();
	g /= aPt.size();

	if (-1 != k1 && -1 != k2)
	{
		d.feat.length = sqrt(dMaxDist);

		double dx = aPt[k1].x - aPt[k2].x;
		double dy = aPt[k2].y - aPt[k2].y;

		double dA = dy;
		double dB = -dx;
		double dC = dx * aPt[k1].y - dy * aPt[k1].x;
		double dD = sqrt(dA*dA + dB * dB);
		double dMin = 1.0e14;
		double dMax = -1.0e15;
		for (unsigned n = 0; n < aPt.size(); n++)
		{
			double dCurDist = (dA*aPt[n].x + dB * aPt[n].y + dC);
			if (dCurDist > dMax)
			{
				dMax = dCurDist;
			}

			if (dCurDist < dMin)
			{
				dMin = dCurDist;
			}
		}

		d.feat.width = (dMax - dMin) / dD;
	}

	d.center.x = (long)(xc + 0.5);
	d.center.y = (long)(yc + 0.5);
	d.feat.aveGrey = (unsigned short)(g + 0.5);

	d.feat.area = aPt.size();

	vector<POINT> aC;
	GetContourPointSet(aC, aPt);
	d.feat.peri = aC.size();

	d.feat.peri_area_ratio = (double)d.feat.peri / (double)d.feat.area;

	d.feat.rndness = 4 * 3.1415926 * d.feat.area / d.feat.peri / d.feat.peri;

	return true;
}


bool DetectDefect(vector<Defect> &aDefectList, unsigned short *pImg, int nW, int nH, ImageRect *pROI, DetectParam *pParam)
{
	ImageRect roi;
	if (NULL == pROI)
	{
		roi = ImageRect(0, nW - 1, 0, nH - 1);
	}
	else
	{
		roi = *pROI;
	}

	DetectParam detParam;
	if (NULL != pParam)
	{
		detParam = *pParam;
	}

	unsigned short *pOri = new unsigned short[nW*nH];
	memcpy(pOri, pImg, nW*nH * sizeof(unsigned short));

	if (!MedianFiltering(pImg, nW, nH, detParam.nFilterRadius, &roi))
	{
		return false;
	}
	
	//取得二值图像
	unsigned char *pMonoImg = new unsigned char[nW*nH];
	memset(pMonoImg, 0, nW*nH);
	unsigned ncnt1 = 0, ncnt2 = 0;
	for (int y = roi.ys; y < roi.ye; y++)
		for (int x = roi.xs; x < roi.xe; x++)
		{
			if (abs(pImg[y*nW + x] - pOri[y*nW + x]) > detParam.nGreyDiff)
			{
				pMonoImg[y*nW + x] = 255;
				ncnt1++;
			}
			else
			{
				pMonoImg[y*nW + x] = 0;
				ncnt2++;
			}
		}

	//遍历二值图像，得到缺陷
	bool *pVisited = new bool[nW*nH];
	memset(pVisited, false, nW*nH * sizeof(bool));

	for (int y = roi.ys; y < roi.ye; y++)
	{
		for (int x = roi.xs; x < roi.xe; x++)
		{
			if (!pVisited[y*nW + x] && pMonoImg[y*nW + x] == 255)
			{
				POINT pt;
				pt.x = x;
				pt.y = y;
				vector<POINT> aPt;
				RegionTrace(aPt, pMonoImg, nW, nH, roi, pt, pVisited);
				if (aPt.size() > detParam.nMinDefectArea && aPt.size() < 500)
				{
					Defect d;
					d.nPt = aPt.size();
					d.aPt.assign(aPt.begin(), aPt.end());
										
					if (CalcDetect(pOri, nW, nH, aPt, d))
					{
						aDefectList.push_back(d);
					}
				}
			}
		}
	}
	delete[]pVisited;

	delete[]pMonoImg;

	delete[]pOri;
	   
	return false;
}





DefectRecognition::DefectRecognition()
{
}


DefectRecognition::~DefectRecognition()
{
}
