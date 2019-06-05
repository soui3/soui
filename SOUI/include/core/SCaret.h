#pragma once
#include <interface/scaret-i.h>

namespace SOUI{
    class SOUI_EXP SCaret : public TObjRefImpl<ICaret>
    {
    public:
        SCaret();
	public:
		// 通过 ICaret 继承
		virtual BOOL Init(HBITMAP hBmp, int nWid, int nHei) override;
		virtual void Draw(IRenderTarget * pRT) override;
		virtual int GoNextState() override;
		virtual void SetPosition(int x, int y) override;
		virtual void SetVisible(BOOL bVisible) override;
		virtual BOOL IsVisible() const  override;
		virtual RECT GetRect() const override;
	protected:
		BOOL	m_bVisible;							/**< Is caret visible now */
		CPoint	m_ptCaret;							/**< caret position */
		SAutoRefPtr<IBitmap>    m_bmpCaret;         /**< 模拟插入符 */
		bool	m_bDrawCaret;						/**< is caret drawing */

	};
}

