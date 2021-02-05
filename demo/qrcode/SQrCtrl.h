#pragma once
#include "QR_Encode.h"
#include "control/SCmnCtrl.h"

class SQrCtrl :public SImageWnd
{
	SOUI_CLASS_NAME(SQrCtrl, L"qrctrl")
public:
	SQrCtrl();
	~SQrCtrl();

	STDMETHOD_(void,SetWindowText)(THIS_ LPCTSTR lpszText) OVERRIDE;
	STDMETHOD_(BOOL,OnRelayout)(THIS_ RECT rcWnd) OVERRIDE;
protected:
	void CreateQrImg(SStringT strContent);

	void MakeCacheApha(ISkinObj *pSkin, IBitmap *_pBitCache, IBitmap *_pBitMask);

	SOUI_ATTRS_BEGIN()
		ATTR_RECT(L"qrrect", m_QrRect, FALSE)
	SOUI_ATTRS_END()
private:
	CRect m_QrRect;
};

