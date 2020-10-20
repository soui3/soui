#pragma once


#include <helper/SColor.h>
#include <helper/SAttrCracker.h>
#include <helper/obj-ref-impl.hpp>
#include <core/SkCanvas.h>
#include <core/SkBitmap.h>
#include <core/SkTypeface.h>
#include <string/tstring.h>
#include <string/strcpcvt.h>
#include <interface/SRender-i.h>
#include <souicoll.h>

SNSBEGIN

//////////////////////////////////////////////////////////////////////////
// SRenderFactory_Skia
class SRenderFactory_Skia : public TObjRefImpl<IRenderFactory>
{
public:
	SRenderFactory_Skia();

	~SRenderFactory_Skia();

	STDMETHOD_(IImgDecoderFactory *,GetImgDecoderFactory)(THIS) OVERRIDE;
	STDMETHOD_(void,SetImgDecoderFactory)(THIS_ IImgDecoderFactory *pImgDecoderFac) OVERRIDE;
	STDMETHOD_(BOOL,CreateRenderTarget)(THIS_ IRenderTarget ** ppRenderTarget,int nWid,int nHei) OVERRIDE;

	STDMETHOD_(BOOL,CreateFont)(THIS_ IFont ** ppFont, const LOGFONT *lf) OVERRIDE;

	STDMETHOD_(BOOL,CreateBitmap)(THIS_ IBitmap ** ppBitmap) OVERRIDE;

	STDMETHOD_(BOOL,CreateRegion)(THIS_ IRegion **ppRgn) OVERRIDE;

	STDMETHOD_(BOOL,CreatePath)(THIS_ IPath ** ppPath) OVERRIDE;

	STDMETHOD_(BOOL,CreatePathEffect)(THIS_ REFGUID guidEffect,IPathEffect ** ppPathEffect) OVERRIDE;

	STDMETHOD_(BOOL,CreatePathMeasure)(THIS_ IPathMeasure ** ppPathMeasure) OVERRIDE;

	STDMETHOD_(HRESULT,CreateBlurMaskFilter)(THIS_ float radius, IMaskFilter::SkBlurStyle style,IMaskFilter::SkBlurFlags flag,IMaskFilter ** ppMaskFilter) OVERRIDE;

	STDMETHOD_(HRESULT,CreateEmbossMaskFilter)(THIS_ float direction[3], float ambient, float specular, float blurRadius,IMaskFilter ** ppMaskFilter) OVERRIDE;

protected:
	SAutoRefPtr<IImgDecoderFactory> m_imgDecoderFactory;
};


//////////////////////////////////////////////////////////////////////////
// TSkiaRenderObjImpl
template<class T, OBJTYPE ot>
class TSkiaRenderObjImpl : public TObjRefImpl<T>
{
public:
	TSkiaRenderObjImpl(IRenderFactory * pRenderFac):m_pRenderFactory(pRenderFac)
	{

	}

	virtual ~TSkiaRenderObjImpl(){}

	STDMETHOD_(IRenderFactory *,GetRenderFactory)(THIS) SCONST OVERRIDE
	{
		return m_pRenderFactory;
	}

	STDMETHOD_(OBJTYPE,ObjectType)(THIS) SCONST OVERRIDE
	{
		return ot;
	}

	STDMETHOD_(HRESULT,SetAttribute)(THIS_ LPCWSTR attrName, LPCWSTR attrValue,BOOL bLoading) OVERRIDE
	{
		return E_NOTIMPL;
	}

	STDMETHOD_(void,SetAttrFinish)(THIS) OVERRIDE
	{
	}

protected:
	IRenderFactory *m_pRenderFactory;

};


//////////////////////////////////////////////////////////////////////////
// SPen_Skia
class SPen_Skia : public TSkiaRenderObjImpl<IPen,OT_PEN>
{
public:
	SPen_Skia(IRenderFactory * pRenderFac,int iStyle=PS_SOLID,COLORREF cr=0xFF000000,int cWidth=1);

	STDMETHOD_(int,GetWidth)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,SetWidth)(THIS_ int nWid) OVERRIDE;

	STDMETHOD_(int,GetStyle)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,SetStyle)(THIS_ int style) OVERRIDE;

	STDMETHOD_(COLORREF,GetColor)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,SetColor)(THIS_ COLORREF cr) OVERRIDE;

protected:

	int		m_style;
	int		m_nWidth;
	COLORREF	m_cr;
};

//////////////////////////////////////////////////////////////////////////
// SFont_Skia
class SFont_Skia: public TSkiaRenderObjImpl<IFont,OT_FONT>
{
	friend class SRenderTarget_Skia;
	friend class SPath_Skia;
public:
	SFont_Skia(IRenderFactory * pRenderFac,const LOGFONT * plf);

	virtual ~SFont_Skia();

	STDMETHOD_(HRESULT,SetAttribute)(THIS_ LPCWSTR attrName, LPCWSTR attrValue,BOOL bLoading) OVERRIDE;

	STDMETHOD_(void,SetAttrFinish)(THIS) OVERRIDE;

	STDMETHOD_(const LOGFONT *,LogFont)(THIS) SCONST OVERRIDE;

	STDMETHOD_(LPCTSTR,FamilyName)(THIS) SCONST OVERRIDE;

	STDMETHOD_(int,TextSize)(THIS) SCONST OVERRIDE;

	STDMETHOD_(BOOL,IsBold)(THIS) SCONST OVERRIDE;

	STDMETHOD_(BOOL,IsUnderline)(THIS) SCONST OVERRIDE;

	STDMETHOD_(BOOL,IsItalic)(THIS) SCONST OVERRIDE;

	STDMETHOD_(BOOL,IsStrikeOut)(THIS) SCONST OVERRIDE;

	STDMETHOD_(BOOL,UpdateFont)(THIS_ const LOGFONT *pLogFont) OVERRIDE;
private:
	const SkPaint  GetPaint() const;
	SkTypeface *GetFont() const {return m_skFont;}
	SkMaskFilter *GetBlurFilter() {return m_blurFilter;}
protected:
	SkTypeface *m_skFont;   //定义字体
	SkPaint     m_skPaint;  //定义文字绘制属性
	LOGFONT     m_lf;
	SkBlurStyle m_blurStyle;
	SkScalar	m_blurRadius;
	SkMaskFilter *m_blurFilter;
};

class SBrush_Skia : public TSkiaRenderObjImpl<IBrush,OT_BRUSH>
{
public:
	static SBrush_Skia * CreateSolidBrush(IRenderFactory * pRenderFac,COLORREF cr){
		return new SBrush_Skia(pRenderFac,cr);
	}

	static SBrush_Skia * CreateBitmapBrush(IRenderFactory * pRenderFac,SkBitmap bmp)
	{
		return new SBrush_Skia(pRenderFac,bmp);
	}

	SkBitmap GetBitmap(){return m_bmp;}

	COLORREF GetColor() {return m_cr;}

	BOOL IsBitmap(){return m_fBmp;}
protected:
	SBrush_Skia(IRenderFactory * pRenderFac,COLORREF cr)
		:TSkiaRenderObjImpl<IBrush,OT_BRUSH>(pRenderFac),m_cr(cr),m_fBmp(FALSE)
	{

	}
	SBrush_Skia(IRenderFactory * pRenderFac,SkBitmap bmp)
		:TSkiaRenderObjImpl<IBrush,OT_BRUSH>(pRenderFac),m_bmp(bmp),m_fBmp(TRUE)
	{

	}

	COLORREF m_cr;		//颜色画刷
	SkBitmap m_bmp;		//位图画刷
	BOOL	 m_fBmp;
};

//////////////////////////////////////////////////////////////////////////
// SBitmap_Skia
class SBitmap_Skia : public TSkiaRenderObjImpl<IBitmap,OT_BITMAP>
{
public:
	SBitmap_Skia(IRenderFactory *pRenderFac);
	~SBitmap_Skia();

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

public:
	const SkBitmap & GetSkBitmap() const{return m_bitmap;}
	HBITMAP  GetGdiBitmap(){return m_hBmp;}
protected:
	HBITMAP CreateGDIBitmap(int nWid,int nHei,void ** ppBits);

	HRESULT ImgFromDecoder(IImgX *imgDecoder);

	SkBitmap    m_bitmap;   //skia 管理的BITMAP
	HBITMAP     m_hBmp;     //标准的32位位图，和m_bitmap共享内存
};

//////////////////////////////////////////////////////////////////////////
//	SRegion_Skia
class SRegion_Skia: public TSkiaRenderObjImpl<IRegion,OT_RGN>
{
	friend class SRenderTarget_Skia;
public:
	SRegion_Skia(IRenderFactory *pRenderFac);
	virtual ~SRegion_Skia();

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
	SkRegion GetRegion() const;

	void SetRegion(const SkRegion & rgn);

	static SkRegion::Op RGNMODE2SkRgnOP(UINT mode);


protected:
	SkRegion    m_rgn;
};


//////////////////////////////////////////////////////////////////////////
//	SPath_Skia

class SPathInfo_Skia : public TObjRefImpl<IPathInfo>
{
public:
	SPathInfo_Skia(int points);
	~SPathInfo_Skia();
public:
	STDMETHOD_(int,pointNumber)(THIS) SCONST OVERRIDE;
	STDMETHOD_(const float *,data)(THIS) SCONST OVERRIDE;
public:
	float * buffer();
private:
	int mPoints;
	float * mData;
};

class SPath_Skia: public TSkiaRenderObjImpl<IPath,OT_PATH>
{
	friend class SRenderTarget_Skia;
	friend class SPathMeasure_Skia;
public:
	SPath_Skia(IRenderFactory *pRenderFac);
	virtual ~SPath_Skia();

	STDMETHOD_(FillType,getFillType)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,setFillType)(THIS_ FillType ft) OVERRIDE;

	STDMETHOD_(BOOL,isInverseFillType)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,toggleInverseFillType)(THIS) OVERRIDE;

	STDMETHOD_(Convexity,getConvexity)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,setConvexity)(THIS_ Convexity c) OVERRIDE;

	STDMETHOD_(BOOL,isConvex)(THIS) SCONST OVERRIDE;

	STDMETHOD_(BOOL,isOval)(THIS_ RECT* rect) SCONST OVERRIDE;

	STDMETHOD_(void,reset)(THIS) OVERRIDE;

	STDMETHOD_(void,rewind)(THIS) OVERRIDE;

	STDMETHOD_(BOOL,isEmpty)(THIS) SCONST OVERRIDE;

	STDMETHOD_(BOOL,isFinite)(THIS) SCONST OVERRIDE;

	STDMETHOD_(BOOL,isLine)(THIS_ POINT line[2]) SCONST OVERRIDE;

	STDMETHOD_(BOOL,isRect)(THIS_ RECT* rect) SCONST OVERRIDE;

	STDMETHOD_(int,countPoints)(THIS_ ) SCONST OVERRIDE;

	STDMETHOD_(fPoint,getPoint)(THIS_ int index) SCONST OVERRIDE;

	STDMETHOD_(int,getPoints)(THIS_ fPoint points[], int max) SCONST OVERRIDE;

	STDMETHOD_(int,countVerbs)(THIS) SCONST OVERRIDE;

	STDMETHOD_(int,getVerbs)(THIS_ BYTE verbs[], int max) SCONST OVERRIDE;

	STDMETHOD_(RECT,getBounds)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,moveTo)(THIS_ float x, float y) OVERRIDE;

	STDMETHOD_(void,rMoveTo)(THIS_ float dx, float dy) OVERRIDE;

	STDMETHOD_(void,lineTo)(THIS_ float x, float y) OVERRIDE;

	STDMETHOD_(void,rLineTo)(THIS_ float dx, float dy) OVERRIDE;

	STDMETHOD_(void,quadTo)(THIS_ float x1, float y1, float x2, float y2) OVERRIDE;

	STDMETHOD_(void,rQuadTo)(THIS_ float dx1, float dy1, float dx2, float dy2) OVERRIDE;

	STDMETHOD_(void,conicTo)(THIS_ float x1, float y1, float x2, float y2,
		float w) OVERRIDE;

	STDMETHOD_(void,rConicTo)(THIS_ float dx1, float dy1, float dx2, float dy2,
		float w) OVERRIDE;

	STDMETHOD_(void,cubicTo)(THIS_ float x1, float y1, float x2, float y2,
		float x3, float y3) OVERRIDE;

	STDMETHOD_(void,rCubicTo)(THIS_ float x1, float y1, float x2, float y2,
		float x3, float y3) OVERRIDE;

	STDMETHOD_(void,arcTo)(THIS_ const RECT& oval, float startAngle, float sweepAngle,
		bool forceMoveTo) OVERRIDE;

	STDMETHOD_(void,arcTo)(THIS_ float x1, float y1, float x2, float y2,
		float radius) OVERRIDE;

	STDMETHOD_(void,close)(THIS) OVERRIDE;

	STDMETHOD_(BOOL,isRect)(THIS_ bool* isClosed, Direction* direction) SCONST OVERRIDE;

	STDMETHOD_(void,addRect)(THIS_ const RECT& rect, Direction dir /*= kCW_Direction*/) OVERRIDE;

	STDMETHOD_(void,addRect)(THIS_ float left, float top, float right, float bottom,
		Direction dir/* = kCW_Direction*/) OVERRIDE;

	STDMETHOD_(void,addOval)(THIS_ const RECT * oval, Direction dir/*= kCW_Direction*/) OVERRIDE;

	STDMETHOD_(void,addCircle)(THIS_ float x, float y, float radius,
		Direction dir /*= kCW_Direction*/) OVERRIDE;

	STDMETHOD_(void,addArc)(THIS_ const RECT* oval, float startAngle, float sweepAngle) OVERRIDE;

	STDMETHOD_(void,addRoundRect)(THIS_ const RECT* rect, float rx, float ry,
		Direction dir/* = kCW_Direction*/) OVERRIDE;

	STDMETHOD_(void,addRoundRect)(THIS_ const RECT* rect, const float radii[],
		Direction dir/* = kCW_Direction*/) OVERRIDE;

	STDMETHOD_(void,addPoly)(THIS_ const POINT pts[], int count, bool close) OVERRIDE;

	STDMETHOD_(void,addPath)(THIS_ const IPath * src, float dx, float dy,
		AddPathMode mode /*= kAppend_AddPathMode*/) OVERRIDE;

	STDMETHOD_(void,reverseAddPath)(THIS_ const IPath* src) OVERRIDE;

	STDMETHOD_(void,offset)(THIS_ float dx, float dy) OVERRIDE;

	STDMETHOD_(void,transform)(THIS_ const IxForm * matrix) OVERRIDE;

	STDMETHOD_(BOOL,getLastPt)(THIS_ POINT* lastPt) SCONST OVERRIDE;

	STDMETHOD_(void,setLastPt)(THIS_ float x, float y) OVERRIDE;

	STDMETHOD_(void,addString)(THIS_ LPCTSTR pszText,int nLen, float x,float y, const IFont *pFont) OVERRIDE;

	STDMETHOD_(IPathInfo*, approximate)(THIS_ float acceptableError) OVERRIDE;

protected:
	SkPath      m_skPath;
};

class SMaskFilter_Skia : public TObjRefImpl<IMaskFilter>
{
	friend class SRenderTarget_Skia;
public:
	SMaskFilter_Skia(SkMaskFilter *maskFilter);

	~SMaskFilter_Skia();
protected:
	SkMaskFilter * m_maskFilter;
};

//////////////////////////////////////////////////////////////////////////
//	SRenderTarget_Skia
//////////////////////////////////////////////////////////////////////////
class SRenderTarget_Skia: public TObjRefImpl<IRenderTarget>
{
public:
	SRenderTarget_Skia(IRenderFactory* pRenderFactory,int nWid,int nHei);
	~SRenderTarget_Skia();

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
	STDMETHOD_(void,SetMaskFilter)(THIS_ IMaskFilter *pMaskFilter) OVERRIDE;
	STDMETHOD_(IMaskFilter *,GetMaskFilter)(THIS) OVERRIDE;
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
public:
	SkCanvas *GetCanvas(){return m_SkCanvas;}

protected:
	bool SetPaintXferMode(SkPaint & paint,int nRopMode);

protected:
	SkCanvas *m_SkCanvas;
	SColor            m_curColor;
	SAutoRefPtr<SBitmap_Skia> m_curBmp;
	SAutoRefPtr<SPen_Skia> m_curPen;
	SAutoRefPtr<SBrush_Skia> m_curBrush;
	SAutoRefPtr<SFont_Skia> m_curFont;

	SkPoint         m_ptOrg;
	SkPaint			m_paint;

	//注意保存4个默认的RenderObject对象
	SAutoRefPtr<IBitmap> m_defBmp;
	SAutoRefPtr<IPen> m_defPen;
	SAutoRefPtr<IBrush> m_defBrush;
	SAutoRefPtr<IFont> m_defFont;
	SAutoRefPtr<IMaskFilter> m_curMaskFilter;

	SAutoRefPtr<IRenderFactory> m_pRenderFactory;

	HDC m_hGetDC;
	UINT m_uGetDCFlag;

	bool			m_bAntiAlias;
	SList<int>		m_lstLayerId;	//list to save layer ids
	int				m_xferMode;
};

namespace RENDER_SKIA
{
	SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef ** ppRenderFactory);
}
SNSEND
