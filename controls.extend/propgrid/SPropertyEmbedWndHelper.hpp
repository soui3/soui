#pragma once

namespace SOUI
{
    template<class T>
    class TplPropEmbedWnd : public T
    {
    public:
        template<class P1>
        TplPropEmbedWnd(P1 p1):T(p1),m_bUpdateData(TRUE){}
        template<class P1,class P2>
        TplPropEmbedWnd(P1 p1,P2 p2):T(p1,p2),m_bUpdateData(TRUE){}

        void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
        {
            if(nChar == VK_ESCAPE)
            {
                m_bUpdateData=FALSE;
                T::GetParent()->SetFocus();
            }else
            {
                T::OnKeyDown(nChar,nRepCnt,nFlags);
            }
        }

        void OnKillFocus(SWND wndFocus)
        {
            if(m_bUpdateData) T::UpdateData();
            T::OnKillFocus(wndFocus);
            T::GetOwner()->OnInplaceActive(FALSE);
        }

    protected:
        virtual BOOL ProcessSwndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
        {
            if (uMsg == WM_KEYDOWN)
            {
                OnKeyDown((TCHAR)wParam, (UINT)lParam & 0xFFFF, (UINT)((lParam & 0xFFFF0000) >> 16));
                lResult = 0;
                return TRUE;
            }
            else if (uMsg == WM_KILLFOCUS)
            {
                OnKillFocus((SWND)wParam);
                lResult = 0;
                return TRUE;
            }
            else
            {
                return T::ProcessSwndMessage(uMsg, wParam, lParam, lResult);
            }
        }
        virtual void OnFinalRelease()
        {
            delete this;
        }
    private:
        BOOL m_bUpdateData;
    };

}