#pragma once

#ifdef LIBIMGIO_EXPORTS
#define LIBIMGIO_API __declspec(dllexport)
#else
#define LIBIMGIO_API __declspec(dllimport)
#endif


class LIBIMGIO_API RawImageFile
{
public:
	RawImageFile();
	RawImageFile(int nW, int nH);
	virtual ~RawImageFile();

	//bool Create(int nW, int nH);

	bool Load(const char *szFile, int nW, int nH);

	bool Save(const char *szFile);

	unsigned short* GetBuffer();

	int GetWidth();

	int GetHeight();

private:
	int m_nWidth, m_nHeight;

	unsigned short *m_pBuffer;


};

