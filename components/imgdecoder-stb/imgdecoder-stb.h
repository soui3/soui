// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the IMGDECODERWIC_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// IMGDECODERWIC_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include <helper/obj-ref-impl.hpp>
#include <interface/SImgDecoder-i.h>

SNSBEGIN

    class SImgFrame_STB : public IImgFrame
    {
    public:
        SImgFrame_STB(BYTE *data,int w,int h);
        ~SImgFrame_STB();

		STDMETHOD_(BOOL,GetSize)(THIS_ UINT *pWid,UINT *pHei) OVERRIDE;
		STDMETHOD_(BOOL,CopyPixels)(THIS_ 
			/* [unique][in] */ const RECT *prc,
			/* [in] */ UINT cbStride,
			/* [in] */ UINT cbBufferSize,
			/* [size_is][out] */ BYTE *pbBuffer) OVERRIDE;
		STDMETHOD_(int,GetDelay)(THIS) OVERRIDE {return 0;}

    protected:
        unsigned char * m_data;
        int      m_nWid,m_nHei;
    };
    
    class SImgX_STB : public TObjRefImpl<IImgX>
    {
        friend class SImgDecoderFactory_STB;
    public:
		STDMETHOD_(int,LoadFromMemory)(THIS_ void *pBuf,size_t bufLen) OVERRIDE;
		STDMETHOD_(int,LoadFromFileW)(THIS_ LPCWSTR pszFileName) OVERRIDE;
		STDMETHOD_(int,LoadFromFileA)(THIS_ LPCSTR pszFileName) OVERRIDE;
		STDMETHOD_(UINT,GetFrameCount)(THIS) OVERRIDE;
		STDMETHOD_(IImgFrame *, GetFrame)(THIS_ UINT iFrame) OVERRIDE;
    protected:
        SImgX_STB(BOOL bPremultiple);
        ~SImgX_STB(void);
        
        void _DoPromultiply(BYTE *pdata,int nWid,int nHei);

        BOOL m_bPremultiple;
        SImgFrame_STB *     m_pImg;
    };

    #define DESC_IMGDECODER L"stb"
    class SImgDecoderFactory_STB : public TObjRefImpl<IImgDecoderFactory>
    {
    friend class SImgX_STB;
    public:
        SImgDecoderFactory_STB();
        ~SImgDecoderFactory_STB();
        
		STDMETHOD_(BOOL,CreateImgX)(THIS_ IImgX **ppImgDecoder) OVERRIDE;
		STDMETHOD_(HRESULT,SaveImage)(THIS_ BYTE* pBits, int nWid,int nHei, LPCWSTR pszFileName, LPVOID pFormat) SCONST OVERRIDE;
		STDMETHOD_(LPCWSTR,GetDescription)(THIS) SCONST OVERRIDE;
    };
    
    //////////////////////////////////////////////////////////////////////////
    namespace IMGDECODOR_STB
    {
        SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef **pImgDecoderFactory);
    }

	SNSEND
