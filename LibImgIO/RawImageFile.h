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

	bool Load(const CString& sFile);

	bool Save(const CString &sFile);

	unsigned short* GetData();

	int GetWidth();

	int GetHeight();

private:
	int m_nWidth, m_nHeight;

	unsigned short *m_pData;


};

