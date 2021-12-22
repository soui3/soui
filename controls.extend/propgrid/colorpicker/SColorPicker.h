#pragma once
#include "ColourPopup.h"

namespace SOUI
{
	DEF_EVT_EXT(EventColorChange,EVT_EXTERNAL_BEGIN+100,{
		COLORREF crSel;
	});

class SColorPicker :
	public SButton ,
	public IColorPicker
{
	SOUI_CLASS_NAME(SColorPicker, L"colorpicker")

public:
	SColorPicker(void);
	~SColorPicker(void);

	COLORREF GetColor(){return m_crCur;}
protected://IColorPicker
	virtual void OnColorChanged(COLORREF cr);
	virtual void OnColorEnd(BOOL bCancel,COLORREF cr);
    virtual IMessageLoop * GetMsgLoop();
protected:
	void OnPaint(IRenderTarget *pRT);
	void OnLButtonUp(UINT nFlags,CPoint pt);

	SOUI_MSG_MAP_BEGIN()
		MSG_WM_PAINT_EX(OnPaint)
		MSG_WM_LBUTTONUP(OnLButtonUp)
	SOUI_MSG_MAP_END()

	SOUI_ATTRS_BEGIN()
		ATTR_COLOR(L"color",m_crCur=m_crDef,TRUE)
	SOUI_ATTRS_END()
	
	COLORREF	m_crDef;
	COLORREF	m_crCur;
};

}
