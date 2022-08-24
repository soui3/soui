#include "stdafx.h"
#include "SStaticGdip.h"
#include <GdiPlus.h>

namespace SOUI{


ULONG_PTR SStaticGdip::s_gdiPlusToken=0;
BOOL SStaticGdip::initGdip()
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	if(s_gdiPlusToken!=0)
		return TRUE;
	BOOL bOK = Gdiplus::Ok == Gdiplus::GdiplusStartup(&s_gdiPlusToken, &gdiplusStartupInput, NULL);
	if(!bOK)
	{
		SASSERT(0);
	}
	return bOK;
}

void SStaticGdip::uninitGdip()
{
	if (s_gdiPlusToken != 0)
	{
		Gdiplus::GdiplusShutdown(s_gdiPlusToken);
		s_gdiPlusToken = 0;
	}
}

SStaticGdip::SStaticGdip(void)
{
}

SStaticGdip::~SStaticGdip(void)
{
}

void SStaticGdip::OnDrawLine(IRenderTarget * pRT, LPCTSTR pszBuf, int iBegin, int cchText, LPRECT pRect, UINT uFormat)
{
	HDC hdc = pRT->GetDC(0);
	{
		Gdiplus::Graphics graphics(hdc);
		IFontPtr curFont = (IFontPtr)pRT->GetCurrentObject(OT_FONT);
		Gdiplus::Font font(hdc,curFont->LogFont());

		if(uFormat & DT_CALCRECT){
			Gdiplus::PointF pt(0,0);
			Gdiplus::RectF rcOut;
			graphics.MeasureString(pszBuf+iBegin,cchText,&font,pt,&rcOut);
			pRect->right = pRect->left + (long)rcOut.GetRight();
			pRect->bottom = pRect->left + (long)rcOut.GetBottom();
		}else{
			Gdiplus::PointF pt((float)pRect->left,(float)pRect->top);
			Gdiplus::StringFormat format;
			if(uFormat & DT_LEFT) format.SetAlignment(Gdiplus::StringAlignmentNear);
			else if(uFormat & DT_CENTER) format.SetAlignment(Gdiplus::StringAlignmentCenter);
			else if(uFormat & DT_RIGHT) format.SetAlignment(Gdiplus::StringAlignmentFar);
			if(uFormat & DT_TOP) format.SetLineAlignment(Gdiplus::StringAlignmentNear);
			else if(uFormat & DT_VCENTER) format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
			else if(uFormat & DT_BOTTOM) format.SetLineAlignment(Gdiplus::StringAlignmentFar);

			COLORREF cr = pRT->GetTextColor();
			Gdiplus::SolidBrush brush(Gdiplus::Color((cr>>24)&0xff,cr & 0xff, (cr>>8)&0xff,(cr>>16)&0xff));
			graphics.DrawString(pszBuf+iBegin,cchText,&font,pt,&format,&brush);
		}
	}
	pRT->ReleaseDC(hdc);
}


SIZE SStaticGdip::OnMeasureText(IRenderTarget *pRT,LPCTSTR pszText,int cchLen)
{
	SIZE szRet={0};
	HDC hdc = pRT->GetDC(0);
	{
		Gdiplus::Graphics graphics(hdc);
		IFontPtr curFont = (IFontPtr)pRT->GetCurrentObject(OT_FONT);
		Gdiplus::Font font(hdc,curFont->LogFont());
		Gdiplus::PointF pt(0,0);
		Gdiplus::RectF rcOut;
		graphics.MeasureString(pszText,cchLen,&font,pt,&rcOut);
		szRet.cx = (long)rcOut.GetRight();
		szRet.cy = (long)rcOut.GetBottom();
	}
	pRT->ReleaseDC(hdc);
	return szRet;
}


}
