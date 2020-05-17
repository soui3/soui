// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the IMGDECODERWIC_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// IMGDECODERWIC_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include <wincodec.h>
#include <helper/obj-ref-impl.hpp>

#include <interface/SImgDecoder-i.h>

SNSBEGIN

    class SImgFrame_WIC : public IImgFrame
    {
    public:
        SImgFrame_WIC(IWICBitmapSource *pFrame=NULL);
        void SetWICBitmapSource(IWICBitmapSource *pFrame);
        void SetFrameDelay(int nDelay);

		STDMETHOD_(BOOL,GetSize)(THIS_ UINT *pWid,UINT *pHei) OVERRIDE;
		STDMETHOD_(BOOL,CopyPixels)(THIS_ 
			/* [unique][in] */ const RECT *prc,
			/* [in] */ UINT cbStride,
			/* [in] */ UINT cbBufferSize,
			/* [size_is][out] */ BYTE *pbBuffer) OVERRIDE;
		STDMETHOD_(int,GetDelay)(THIS) OVERRIDE {return m_nFrameDelay;}
    protected:
        SAutoRefPtr<IWICBitmapSource>  m_pFrame;
        int     m_nFrameDelay;
    };
    
    class SImgX_WIC : public TObjRefImpl<IImgX>
    {
        friend class SImgDecoderFactory_WIC;
    public:
		STDMETHOD_(int,LoadFromMemory)(THIS_ void *pBuf,size_t bufLen) OVERRIDE;
		STDMETHOD_(int,LoadFromFileW)(THIS_ LPCWSTR pszFileName) OVERRIDE;
		STDMETHOD_(int,LoadFromFileA)(THIS_ LPCSTR pszFileName) OVERRIDE;
		STDMETHOD_(UINT,GetFrameCount)(THIS) OVERRIDE;
		STDMETHOD_(IImgFrame *, GetFrame)(THIS_ UINT iFrame) OVERRIDE;
    protected:
        SImgX_WIC(BOOL bPremultiplied);
        ~SImgX_WIC(void);
        
        int _DoDecode(IWICBitmapDecoder * pDecoder);

        SImgFrame_WIC *     m_pImgArray;
        UINT				m_uImgCount;
        BOOL                m_bPremultiplied;

    };

    #define DESC_IMGDECODER L"wic"
    class SImgDecoderFactory_WIC : public TObjRefImpl<IImgDecoderFactory>
    {
    friend class SImgX_WIC;
    public:
        SImgDecoderFactory_WIC();
        ~SImgDecoderFactory_WIC();
        
		STDMETHOD_(BOOL,CreateImgX)(THIS_ IImgX **ppImgDecoder) OVERRIDE;
		STDMETHOD_(HRESULT,SaveImage)(THIS_ BYTE* pBits, int nWid,int nHei, LPCWSTR pszFileName, LPVOID pFormat) SCONST OVERRIDE;
		STDMETHOD_(LPCWSTR,GetDescription)(THIS) SCONST OVERRIDE;
    protected:
        static SAutoRefPtr<IWICImagingFactory> s_wicImgFactory;
    };
    
    //////////////////////////////////////////////////////////////////////////
    namespace IMGDECODOR_WIC
    {
        SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef **pImgDecoderFactory);
    }
}//end of namespace SOUI

