#pragma once
#include <interface/scaret-i.h>
#include <interface/sinterpolator-i.h>
#include <interface/STimelineHandler-i.h>

namespace SOUI{
    class SOUI_EXP SCaret : public TObjRefImpl<SObjectImpl<ICaret>>,public ITimelineHandler
    {
		SOUI_CLASS_NAME_EX(SCaret,L"caret", None)
    public:
        SCaret(ISwndContainer *pContainer);
		~SCaret();
	public:
		// 通过 ICaret 继承
		STDMETHOD_(BOOL,Init)(THIS_ HBITMAP hBmp, int nWid, int nHei) OVERRIDE;

		STDMETHOD_(void,SetPosition)(THIS_ int x, int y) OVERRIDE;

		STDMETHOD_(BOOL,SetVisible)(THIS_ BOOL bVisible,SWND owner) OVERRIDE;

		STDMETHOD_(BOOL,IsVisible)(THIS) SCONST OVERRIDE;

		STDMETHOD_(void,Draw)(THIS_ IRenderTarget *pRT) OVERRIDE;

		STDMETHOD_(RECT,GetRect)(THIS) SCONST OVERRIDE;
	public:
		STDMETHOD_(void,OnNextFrame)(THIS_) OVERRIDE;

	public:
		SOUI_ATTRS_BEGIN()
			ATTR_BOOL(L"animate", m_bAniCaret, FALSE)
			ATTR_COLOR(L"color", m_crCaret, FALSE)
			ATTR_INT(L"fadeTime", m_nAniFrames,FALSE)
			ATTR_INT(L"showTime", m_nShowFrames,FALSE)
			ATTR_INTERPOLATOR(L"interpolator", m_AniInterpolator,FALSE)
			ATTR_CHAIN_PTR(m_AniInterpolator,0)
		SOUI_ATTRS_END()
	protected:
		void Invalidate();
	protected:
		BOOL	m_bVisible;							/**< Is caret visible now */
		CPoint	m_ptCaret;							/**< caret position */
		SAutoRefPtr<IBitmap>    m_bmpCaret;         /**< 模拟插入符 */
		bool	m_bDrawCaret;						/**< is caret drawing */
		int		m_iFrame;
		BYTE	m_byAlpha;

		BOOL	m_bAniCaret;
		COLORREF m_crCaret;
		int		m_nAniFrames;
		int		m_nShowFrames;
		SAutoRefPtr<IInterpolator> m_AniInterpolator;
		ISwndContainer*			m_pContainer;
		SWND	m_hOwner;
	};
}

