// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the IMGDECODERWIC_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// IMGDECODERWIC_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include <helper/obj-ref-impl.hpp>

#include <interface/SImgDecoder-i.h>
#include <interface/SRender-i.h>

struct APNGDATA;

SNSBEGIN

class SImgFrame_PNG : public IImgFrame
{
public:
	SImgFrame_PNG();
	void Attach(const BYTE * pdata,int nWid,int nHei,int nDelay);

	STDMETHOD_(BOOL,GetSize)(THIS_ UINT *pWid,UINT *pHei) OVERRIDE;
	STDMETHOD_(BOOL,CopyPixels)(THIS_ 
		/* [unique][in] */ const RECT *prc,
		/* [in] */ UINT cbStride,
		/* [in] */ UINT cbBufferSize,
		/* [size_is][out] */ BYTE *pbBuffer) OVERRIDE;
	STDMETHOD_(int,GetDelay)(THIS) OVERRIDE {return m_nFrameDelay;}
protected:
	int     m_nFrameDelay;
	int     m_nWid, m_nHei;
	const BYTE   *m_pdata;
};

class SImgX_PNG : public TObjRefImpl<IImgX>
{
	friend class SImgDecoderFactory_PNG;
public:
	STDMETHOD_(int,LoadFromMemory)(THIS_ void *pBuf,size_t bufLen) OVERRIDE;
	STDMETHOD_(int,LoadFromFileW)(THIS_ LPCWSTR pszFileName) OVERRIDE;
	STDMETHOD_(int,LoadFromFileA)(THIS_ LPCSTR pszFileName) OVERRIDE;
	STDMETHOD_(UINT,GetFrameCount)(THIS) OVERRIDE;
	STDMETHOD_(IImgFrame *, GetFrame)(THIS_ UINT iFrame) OVERRIDE;
protected:
	SImgX_PNG(BOOL bPremultiplied);
	~SImgX_PNG(void);

	int _DoDecode(APNGDATA *pData);

	BOOL m_bPremultiplied;

	APNGDATA       *    m_pngData;
	SImgFrame_PNG  *    m_pImgArray;
};

#define DESC_IMGDECODER L"apng"
class SImgDecoderFactory_PNG : public TObjRefImpl<IImgDecoderFactory>
{
	friend class SImgX_PNG;
public:
	SImgDecoderFactory_PNG();
	~SImgDecoderFactory_PNG();

	STDMETHOD_(BOOL,CreateImgX)(THIS_ IImgX **ppImgDecoder) OVERRIDE;
	STDMETHOD_(HRESULT,SaveImage)(THIS_ BYTE* pBits, int nWid,int nHei, LPCWSTR pszFileName, LPVOID pFormat) SCONST OVERRIDE;
	STDMETHOD_(LPCWSTR,GetDescription)(THIS) SCONST OVERRIDE;
};

//////////////////////////////////////////////////////////////////////////
namespace IMGDECODOR_PNG
{
	SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef **pImgDecoderFactory);
}

SNSEND
