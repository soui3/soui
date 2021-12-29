#pragma once

#include <helper/SColor.h>
#include <helper/obj-ref-impl.hpp>
#include <sobject/Sobject.hpp>
#include <string/tstring.h>
#include <string/strcpcvt.h>
#include <interface/SRender-i.h>
#include <souicoll.h>

SNSBEGIN

//////////////////////////////////////////////////////////////////////////
// SRenderFactory_GDI
class SRenderFactory_GDI : public TObjRefImpl<IRenderFactory>
{
public:
	SRenderFactory_GDI()
	{
	}

	~SRenderFactory_GDI()
	{
	}

	STDMETHOD_(IImgDecoderFactory *,GetImgDecoderFactory)(THIS) OVERRIDE;
	STDMETHOD_(void,SetImgDecoderFactory)(THIS_ IImgDecoderFactory *pImgDecoderFac) OVERRIDE;
	STDMETHOD_(BOOL,CreateRenderTarget)(THIS_ IRenderTarget ** ppRenderTarget,int nWid,int nHei) OVERRIDE;

	STDMETHOD_(BOOL,CreateFont)(THIS_ IFont ** ppFont, const LOGFONT *lf) OVERRIDE;

	STDMETHOD_(BOOL,CreateBitmap)(THIS_ IBitmap ** ppBitmap) OVERRIDE;

	STDMETHOD_(BOOL,CreateRegion)(THIS_ IRegion **ppRgn) OVERRIDE;

	STDMETHOD_(HRESULT,CreateBlurMaskFilter)(THIS_ float radius, BlurStyle style,BlurFlags flag,IMaskFilter ** ppMaskFilter) OVERRIDE {return E_NOTIMPL;}

	STDMETHOD_(HRESULT,CreateEmbossMaskFilter)(THIS_ float direction[3], float ambient, float specular, float blurRadius,IMaskFilter ** ppMaskFilter) OVERRIDE {return E_NOTIMPL;}

	STDMETHOD_(BOOL,CreatePath)(THIS_ IPath ** ppPath) OVERRIDE;

	STDMETHOD_(BOOL,CreatePathEffect)(THIS_ REFGUID guidEffect,IPathEffect ** ppPathEffect) OVERRIDE;

	STDMETHOD_(BOOL,CreatePathMeasure)(THIS_ IPathMeasure ** ppPathMeasure) OVERRIDE;
protected:
	SAutoRefPtr<IImgDecoderFactory> m_imgDecoderFactory;
};


//////////////////////////////////////////////////////////////////////////
// TGdiRenderObjImpl
template<class T, OBJTYPE ot>
class TGdiRenderObjImpl : public TObjRefImpl<T>
{
public:
	TGdiRenderObjImpl(IRenderFactory * pRenderFac):m_pRenderFactory(pRenderFac)
	{

	}

	virtual ~TGdiRenderObjImpl(){}

	STDMETHOD_(IRenderFactory *,GetRenderFactory)(THIS) SCONST OVERRIDE
	{
		return m_pRenderFactory;
	}

	STDMETHOD_(OBJTYPE,ObjectType)(THIS) SCONST OVERRIDE
	{
		return ot;
	}

protected:
	IRenderFactory *m_pRenderFactory;

};


//////////////////////////////////////////////////////////////////////////
// SPen_GDI
class SPen_GDI : public TGdiRenderObjImpl<IPen,OT_PEN>
{
public:
	SPen_GDI(IRenderFactory * pRenderFac,int iStyle=PS_SOLID,COLORREF cr=0,int cWidth=1);

	~SPen_GDI();

	STDMETHOD_(int,GetWidth)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,SetWidth)(THIS_ int nWid) OVERRIDE;

	STDMETHOD_(int,GetStyle)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,SetStyle)(THIS_ int nStyle) OVERRIDE;

	STDMETHOD_(COLORREF,GetColor)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,SetColor)(THIS_ COLORREF cr) OVERRIDE;

	HPEN GetPen();
protected:
	void UpdatePen();

	int		m_style;
	int		m_nWidth;
	COLORREF	m_cr;

	HPEN    m_hPen;
};

//////////////////////////////////////////////////////////////////////////
// SFont_GDI
class SFont_GDI: public TGdiRenderObjImpl<IFont,OT_FONT>
{
public:
	SFont_GDI(IRenderFactory * pRenderFac,const LOGFONT * plf);
	~SFont_GDI();

		/**
	* LogFont
	* @brief    获得字体的LOGFONT
	* @return   const LOGFONT * -- 包含字体信息的LOGFONT*
	* Describe  
	*/    
	STDMETHOD_(const LOGFONT *,LogFont)(THIS) SCONST OVERRIDE;

	/**
	* FamilyName
	* @brief    获取字体名
	* @return   LPCTSTR -- 字体名
	* Describe  
	*/    
	STDMETHOD_(LPCTSTR,FamilyName)(THIS) SCONST OVERRIDE;

	/**
	* TextSize
	* @brief    获取字体大小
	* @return   int -- 字体大小
	* Describe  
	*/    
	STDMETHOD_(int,TextSize)(THIS) SCONST OVERRIDE;

	/**
	* IsBold
	* @brief    查询是否为粗体
	* @return   BOOL -- true为粗体，false正常
	* Describe  
	*/    
	STDMETHOD_(BOOL,IsBold)(THIS) SCONST OVERRIDE;

	/**
	* IsUnderline
	* @brief    查询字体下划线状态
	* @return   BOOL -- true有下划线，false正常
	* Describe  
	*/    
	STDMETHOD_(BOOL,IsUnderline)(THIS) SCONST OVERRIDE;

	/**
	* IsItalic
	* @brief    查询字体的斜体状态
	* @return   BOOL -- true为斜体，false正常
	* Describe  
	*/    
	STDMETHOD_(BOOL,IsItalic)(THIS) SCONST OVERRIDE;

	/**
	* StrikeOut
	* @brief    查询字体的删除线状态
	* @return   BOOL -- true有删除线
	* Describe  
	*/    
	STDMETHOD_(BOOL,IsStrikeOut)(THIS) SCONST OVERRIDE;

	STDMETHOD_(BOOL,UpdateFont)(THIS_ const LOGFONT *pLogFont) OVERRIDE;

	STDMETHOD_(void,SetProp)(THIS_ IXmlNode *pXmlNode) OVERRIDE;

	HFONT GetFont(){return m_hFont;}
protected:
	LOGFONT     m_lf;
	HFONT       m_hFont;
};

class SBrush_GDI : public TGdiRenderObjImpl<IBrush,OT_BRUSH>
{
public:
	static SBrush_GDI * CreateSolidBrush(IRenderFactory * pRenderFac,COLORREF cr){
		return new SBrush_GDI(pRenderFac,cr);
	}

	static SBrush_GDI * CreateBitmapBrush(IRenderFactory * pRenderFac,HBITMAP hBmp)
	{
		return new SBrush_GDI(pRenderFac,hBmp);
	}


	BOOL IsBitmap(){return m_fBmp;}

	HBRUSH GetBrush(){return m_hBrush;}

	COLORREF GetColor() const {return m_cr;}
protected:
	SBrush_GDI(IRenderFactory * pRenderFac,COLORREF cr)
		:TGdiRenderObjImpl<IBrush,OT_BRUSH>(pRenderFac),m_fBmp(FALSE),m_cr(cr)
	{
		m_hBrush = ::CreateSolidBrush(m_cr&0x00ffffff);
	}
	SBrush_GDI(IRenderFactory * pRenderFac,HBITMAP hBmp)
		:TGdiRenderObjImpl<IBrush,OT_BRUSH>(pRenderFac),m_fBmp(TRUE)
	{
		m_hBrush = ::CreatePatternBrush(hBmp);
	}
	~SBrush_GDI()
	{
		DeleteObject(m_hBrush);
	}


	HBRUSH   m_hBrush;
	BOOL	 m_fBmp;
	COLORREF    m_cr;
};

//////////////////////////////////////////////////////////////////////////
// SBitmap_GDI
class SBitmap_GDI : public TGdiRenderObjImpl<IBitmap,OT_BITMAP>
{
public:
	SBitmap_GDI(IRenderFactory *pRenderFac)
		:TGdiRenderObjImpl<IBitmap,OT_BITMAP>(pRenderFac),m_hBmp(0)
	{
		m_sz.cx=m_sz.cy=0;
	}
	virtual ~SBitmap_GDI()
	{
		if(m_hBmp) DeleteObject(m_hBmp);
	}

	STDMETHOD_(HRESULT,Init)(THIS_ int nWid,int nHei,const LPVOID pBits) OVERRIDE;

	STDMETHOD_(HRESULT,Init2)(THIS_ IImgFrame *pImgFrame) OVERRIDE;

	STDMETHOD_(HRESULT,LoadFromFile)(THIS_ LPCTSTR pszFileName) OVERRIDE;

	STDMETHOD_(HRESULT,LoadFromMemory)(THIS_ LPBYTE pBuf,size_t szLen) OVERRIDE;

	STDMETHOD_(UINT,Width)(THIS) SCONST OVERRIDE;

	STDMETHOD_(UINT,Height)(THIS) SCONST OVERRIDE;

	STDMETHOD_(SIZE,Size)(THIS) SCONST OVERRIDE;

	STDMETHOD_(LPVOID,LockPixelBits)(THIS) OVERRIDE;

	STDMETHOD_(void,UnlockPixelBits)(THIS_ LPVOID pBuf) OVERRIDE;

	STDMETHOD_(const LPVOID,GetPixelBits)(THIS) SCONST OVERRIDE;

	STDMETHOD_(HRESULT,Clone)(THIS_ IBitmap **ppClone) SCONST OVERRIDE;

	STDMETHOD_(HRESULT,Scale)(THIS_ IBitmap **pOutput,int nScale,FilterLevel filterLevel) SCONST OVERRIDE;

	STDMETHOD_(HRESULT,Scale2)(THIS_ IBitmap **pOutput,int nWid,int nHei,FilterLevel filterLevel) SCONST OVERRIDE;

	STDMETHOD_(HRESULT,Save)(THIS_ LPCWSTR pszFileName,const LPVOID pFormat) SCONST OVERRIDE;
		virtual void SetMaskFilter(IMaskFilter *pMaskFilter){}
		virtual IMaskFilter* GetMaskFilter(){return NULL;}
public:
	HBITMAP  GetBitmap(){return m_hBmp;}

	static HBITMAP CreateGDIBitmap(int nWid,int nHei,void ** ppBits);
protected:

	HRESULT ImgFromDecoder(IImgX *imgDecoder);
	SIZE        m_sz;
	HBITMAP     m_hBmp;     //标准的32位位图，和m_bitmap共享内存
};

//////////////////////////////////////////////////////////////////////////
//	SRegion_GDI
class SRegion_GDI: public TGdiRenderObjImpl<IRegion,OT_RGN>
{
	friend class SRenderTarget_GDI;
public:
	SRegion_GDI(IRenderFactory *pRenderFac);
	~SRegion_GDI();

	STDMETHOD_(void,CombineRect)(THIS_ LPCRECT lprect,int nCombineMode) OVERRIDE;

	STDMETHOD_(void,CombineRgn)(THIS_ const IRegion * pRgnSrc,int nCombineMode ) OVERRIDE;

	STDMETHOD_(void,CombineRoundRect)(THIS_ LPCRECT lprect, POINT ptConner, int nCombineMode) OVERRIDE;

	STDMETHOD_(void,CombineEllipse)(THIS_ LPCRECT lprect , int nCombineMode) OVERRIDE;

	STDMETHOD_(void,CombinePolygon)(THIS_ const POINT *pts, int count, int nPolygonMode, int nCombineMode) OVERRIDE;

	STDMETHOD_(BOOL,PtInRegion)(THIS_ POINT pt) SCONST OVERRIDE;

	STDMETHOD_(BOOL,RectInRegion)(THIS_ LPCRECT lprect) SCONST OVERRIDE;

	STDMETHOD_(void,GetRgnBox)(THIS_ LPRECT lprect) SCONST OVERRIDE;

	STDMETHOD_(BOOL,IsEmpty)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,Offset)(THIS_ POINT pt) OVERRIDE;

	STDMETHOD_(void,Clear)(THIS) OVERRIDE;

	STDMETHOD_(BOOL,IsEqual)(THIS_ const IRegion * testRgn) SCONST OVERRIDE;
protected:
	HRGN GetRegion() const;
	void _CombineRgn(HRGN hRgn,int nCombineMode);

	HRGN    m_hRgn;
};

//////////////////////////////////////////////////////////////////////////
//	SRenderTarget_GDI
//////////////////////////////////////////////////////////////////////////
class SRenderTarget_GDI: public TObjRefImpl<IRenderTarget>
{
public:
	SRenderTarget_GDI(IRenderFactory* pRenderFactory,int nWid,int nHei);
	~SRenderTarget_GDI();

	STDMETHOD_(HRESULT,CreateCompatibleRenderTarget)(THIS_ SIZE szTarget,IRenderTarget **ppRenderTarget) OVERRIDE;
	STDMETHOD_(HRESULT,CreatePen)(THIS_ int iStyle,COLORREF cr,int cWidth,IPen ** ppPen) OVERRIDE;
	STDMETHOD_(HRESULT,CreateSolidColorBrush)(THIS_ COLORREF cr,IBrush ** ppBrush) OVERRIDE;
	STDMETHOD_(HRESULT,CreateBitmapBrush)(THIS_ IBitmap *pBmp,IBrush ** ppBrush ) OVERRIDE;
	STDMETHOD_(HRESULT,CreateRegion)(THIS_ IRegion ** ppRegion ) OVERRIDE;

	STDMETHOD_(HRESULT,Resize)(THIS_ SIZE sz) OVERRIDE;

	STDMETHOD_(HRESULT,OffsetViewportOrg)(THIS_ int xOff, int yOff, LPPOINT lpPoint/*=NULL*/) OVERRIDE;
	STDMETHOD_(HRESULT,GetViewportOrg)(THIS_ LPPOINT lpPoint) OVERRIDE;
	STDMETHOD_(HRESULT,SetViewportOrg)(THIS_ POINT pt) OVERRIDE;

	STDMETHOD_(HRESULT,PushClipRect)(THIS_ LPCRECT pRect,UINT mode/*=RGN_AND*/) OVERRIDE;
	STDMETHOD_(HRESULT,PushClipRegion)(THIS_ IRegion *pRegion,UINT mode/*=RGN_AND*/) OVERRIDE;
	STDMETHOD_(HRESULT,PopClip)(THIS) OVERRIDE;

	STDMETHOD_(HRESULT,ExcludeClipRect)(THIS_ LPCRECT pRc) OVERRIDE;
	STDMETHOD_(HRESULT,IntersectClipRect)(THIS_ LPCRECT pRc) OVERRIDE;

	STDMETHOD_(HRESULT,SaveClip)(THIS_ int *pnState) OVERRIDE;
	STDMETHOD_(HRESULT,RestoreClip)(THIS_ int nState/*=-1*/) OVERRIDE;

	STDMETHOD_(HRESULT,GetClipRegion)(THIS_ IRegion **ppRegion) OVERRIDE;
	STDMETHOD_(HRESULT,GetClipBox)(THIS_ LPRECT prc) OVERRIDE;

	STDMETHOD_(HRESULT,DrawText)(THIS_ LPCTSTR pszText,int cchLen,LPRECT pRc,UINT uFormat) OVERRIDE;
	STDMETHOD_(HRESULT,MeasureText)(THIS_ LPCTSTR pszText,int cchLen, SIZE *psz) OVERRIDE;
	STDMETHOD_(HRESULT,TextOut)(THIS_ int x,int y, LPCTSTR lpszString,int nCount) OVERRIDE;

	STDMETHOD_(HRESULT,DrawRectangle)(THIS_ LPCRECT pRect) OVERRIDE;
	STDMETHOD_(HRESULT,FillRectangle)(THIS_ LPCRECT pRect) OVERRIDE;
	STDMETHOD_(HRESULT,FillSolidRect)(THIS_ LPCRECT pRect,COLORREF cr) OVERRIDE;
	STDMETHOD_(HRESULT,DrawRoundRect)(THIS_ LPCRECT pRect,POINT pt) OVERRIDE;
	STDMETHOD_(HRESULT,FillRoundRect)(THIS_ LPCRECT pRect,POINT pt) OVERRIDE;
	STDMETHOD_(HRESULT,FillSolidRoundRect)(THIS_ LPCRECT pRect,POINT pt,COLORREF cr) OVERRIDE;
	STDMETHOD_(HRESULT,ClearRect)(THIS_ LPCRECT pRect,COLORREF cr) OVERRIDE;
	STDMETHOD_(HRESULT,InvertRect)(THIS_ LPCRECT pRect) OVERRIDE;
	STDMETHOD_(HRESULT,DrawEllipse)(THIS_ LPCRECT pRect) OVERRIDE;
	STDMETHOD_(HRESULT,FillEllipse)(THIS_ LPCRECT pRect) OVERRIDE;
	STDMETHOD_(HRESULT,FillSolidEllipse)(THIS_ LPCRECT pRect,COLORREF cr) OVERRIDE;

	STDMETHOD_(HRESULT,DrawArc)(THIS_ LPCRECT pRect,float startAngle,float sweepAngle,bool useCenter) OVERRIDE;
	STDMETHOD_(HRESULT,FillArc)(THIS_ LPCRECT pRect,float startAngle,float sweepAngle) OVERRIDE;

	STDMETHOD_(HRESULT,DrawLines)(THIS_ LPPOINT pPt,size_t nCount) OVERRIDE;
	STDMETHOD_(HRESULT,GradientFill)(THIS_ LPCRECT pRect,BOOL bVert,COLORREF crBegin,COLORREF crEnd,BYTE byAlpha/*=0xFF*/) OVERRIDE;
	STDMETHOD_(HRESULT,GradientFillEx)(THIS_ LPCRECT pRect,const POINT* pts,COLORREF *colors,float *pos,int nCount,BYTE byAlpha/*=0xFF*/ ) OVERRIDE;
	STDMETHOD_(HRESULT,GradientFill2)(THIS_ LPCRECT pRect,GradientType type,COLORREF crStart,COLORREF crCenter,COLORREF crEnd,float fLinearAngle,float fCenterX,float fCenterY,int nRadius,BYTE byAlpha/*=0xFF*/) OVERRIDE;
	STDMETHOD_(HRESULT,DrawIconEx)(THIS_ int xLeft, int yTop, HICON hIcon, int cxWidth,int cyWidth,UINT diFlags) OVERRIDE;
	STDMETHOD_(HRESULT,DrawBitmap)(THIS_ LPCRECT pRcDest,const IBitmap *pBitmap,int xSrc,int ySrc,BYTE byAlpha/*=0xFF*/) OVERRIDE;
	STDMETHOD_(HRESULT,DrawBitmapEx)(THIS_ LPCRECT pRcDest,const IBitmap *pBitmap,LPCRECT pRcSrc,UINT expendMode, BYTE byAlpha/*=0xFF*/) OVERRIDE;
	STDMETHOD_(HRESULT,DrawBitmap9Patch)(THIS_ LPCRECT pRcDest,const IBitmap *pBitmap,LPCRECT pRcSrc,LPCRECT pRcSourMargin,UINT expendMode,BYTE byAlpha/*=0xFF*/) OVERRIDE;
	STDMETHOD_(HRESULT,BitBlt)(THIS_ LPCRECT pRcDest,IRenderTarget *pRTSour,int xSrc,int ySrc,DWORD dwRop/*=kSrcCopy*/) OVERRIDE;
	STDMETHOD_(HRESULT,AlphaBlend)(THIS_ LPCRECT pRcDest,IRenderTarget *pRTSrc,LPCRECT pRcSrc,BYTE byAlpha) OVERRIDE;
	STDMETHOD_(IRenderObj *,GetCurrentObject)(THIS_ OBJTYPE uType) OVERRIDE;
	STDMETHOD_(HRESULT,SelectDefaultObject)(THIS_ OBJTYPE objType, IRenderObj ** pOldObj/* = NULL*/) OVERRIDE;
	STDMETHOD_(HRESULT,SelectObject)(THIS_ IRenderObj *pObj,IRenderObj ** pOldObj/* = NULL*/) OVERRIDE;
	STDMETHOD_(COLORREF,GetTextColor)(THIS) OVERRIDE;
	STDMETHOD_(COLORREF,SetTextColor)(THIS_ COLORREF color) OVERRIDE;
	STDMETHOD_(void,SetMaskFilter)(THIS_ IMaskFilter *pMaskFilter) OVERRIDE{}
	STDMETHOD_(IMaskFilter *,GetMaskFilter)(THIS) OVERRIDE{return NULL;}
	STDMETHOD_(HDC,GetDC)(THIS_ UINT uFlag) OVERRIDE;
	STDMETHOD_(void,ReleaseDC)(THIS_ HDC hdc) OVERRIDE;
	STDMETHOD_(HRESULT,SetTransform)(THIS_ const float matrix[9], float oldMatrix[9]/*=NULL*/) OVERRIDE;
	STDMETHOD_(HRESULT,GetTransform)(THIS_ float matrix[9]) SCONST OVERRIDE;
	STDMETHOD_(COLORREF,GetPixel)(THIS_ int x, int y) OVERRIDE;
	STDMETHOD_(COLORREF,SetPixel)(THIS_ int x, int y, COLORREF cr) OVERRIDE;
	STDMETHOD_(HRESULT,PushClipPath)(THIS_ const IPath * path, UINT mode, bool doAntiAlias = false) OVERRIDE;
	STDMETHOD_(HRESULT,DrawPath)(THIS_ const IPath * path,IPathEffect * pathEffect=NULL) OVERRIDE;
	STDMETHOD_(HRESULT,FillPath)(THIS_ const IPath * path) OVERRIDE;
	STDMETHOD_(HRESULT,PushLayer)(THIS_ const RECT * pRect,BYTE byAlpha/*=0xFF*/) OVERRIDE;
	STDMETHOD_(HRESULT,PopLayer)(THIS) OVERRIDE;
	STDMETHOD_(HRESULT,SetXfermode)(THIS_ int mode,int *pOldMode=NULL) OVERRIDE;
	STDMETHOD_(BOOL,SetAntiAlias)(THIS_ BOOL bAntiAlias) OVERRIDE;
	STDMETHOD_(BOOL,GetAntiAlias)(THIS) SCONST OVERRIDE;
protected:
	HDC               m_hdc;
	SColor            m_curColor;
	SAutoRefPtr<SBitmap_GDI> m_curBmp;
	SAutoRefPtr<SPen_GDI> m_curPen;
	SAutoRefPtr<SBrush_GDI> m_curBrush;
	SAutoRefPtr<SFont_GDI> m_curFont;
	POINT               m_ptOrg;

	//注意保存4个默认的GDI对象
	SAutoRefPtr<IBitmap> m_defBmp;
	SAutoRefPtr<IPen> m_defPen;
	SAutoRefPtr<IBrush> m_defBrush;
	SAutoRefPtr<IFont> m_defFont;
	SAutoRefPtr<IRenderFactory> m_pRenderFactory;
	UINT m_uGetDCFlag;
};

namespace RENDER_GDI
{
	SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef ** ppRenderFactory);
}

SNSEND

