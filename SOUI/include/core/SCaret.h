#pragma once
#include <interface/scaret-i.h>
#include <interface/sinterpolator-i.h>

namespace SOUI{
    class SOUI_EXP SCaret : public TObjRefImpl<SObjectImpl<ICaret>>
    {
		SOUI_CLASS_NAME_EX(SCaret,L"caret", None)
    public:
        SCaret(ISwndContainer *pContainer);
	public:
		// 通过 ICaret 继承
		virtual BOOL Init(HBITMAP hBmp, int nWid, int nHei) override;
		virtual void Draw(IRenderTarget * pRT) override;
		virtual void SetPosition(int x, int y) override;
		virtual BOOL SetVisible(BOOL bVisible) override;
		virtual BOOL IsVisible() const  override;
		virtual RECT GetRect() const override;

	public:
		virtual void OnNextFrame() override;

	public:
		SOUI_ATTRS_BEGIN()
			ATTR_BOOL(L"animate", m_bAniCaret, FALSE)
			ATTR_COLOR(L"color", m_crCaret, FALSE)
			ATTR_INT(L"speed", m_nFrames,FALSE)
			ATTR_INTERPOLATOR(L"interpolator", m_AniInterpolator,FALSE)
			ATTR_CHAIN_PTR(m_AniInterpolator,0)
		SOUI_ATTRS_END()
	protected:
		BOOL	m_bVisible;							/**< Is caret visible now */
		CPoint	m_ptCaret;							/**< caret position */
		SAutoRefPtr<IBitmap>    m_bmpCaret;         /**< 模拟插入符 */
		bool	m_bDrawCaret;						/**< is caret drawing */
		int		m_iFrame;
		BYTE	m_byAlpha;

		BOOL	m_bAniCaret;
		COLORREF m_crCaret;
		int		m_nFrames;
		SAutoRefPtr<IInterpolator> m_AniInterpolator;
		ISwndContainer*			m_pContainer;
	};
}

