//////////////////////////////////////////////////////////////////////////
//  Class Name: SItemPanel
// Description: A Framework wrapping frame to be used in a swindow.
//     Creator: Huang Jianxiong
//     Version: 2011.10.20 - 1.0 - Create
//////////////////////////////////////////////////////////////////////////

#pragma  once

#include "SwndContainerImpl.h"

namespace SOUI
{

class SItemPanel;

struct IItemContainer
{
    virtual void OnItemSetCapture(SItemPanel *pItem,BOOL bCapture)=0; //设置or释放鼠标捕获
    virtual BOOL OnItemGetRect(const SItemPanel *pItem,CRect &rcItem) const=0;    //获得表项的显示位置
    virtual BOOL IsItemRedrawDelay() const=0;                               //指示表项的更新方式
    virtual void OnItemRequestRelayout(SItemPanel *pItem)=0;          //列表项请求重新布局
};

class SOUI_EXP SItemPanel : public SwndContainerImpl, public SWindow
{
	SOUI_CLASS_NAME(SItemPanel,L"itemPanel")
public:
    static SItemPanel * Create(SWindow *pFrameHost,SXmlNode xmlNode,IItemContainer *pItemContainer);
    
    SWindow * GetHostWindow() { return m_pFrmHost;}

protected:
    SItemPanel(SWindow *pFrameHost,SXmlNode xmlNode,IItemContainer *pItemContainer);
    virtual ~SItemPanel() {}

    virtual void WINAPI OnFinalRelease();

public://SwndContainerImpl
    virtual LRESULT DoFrameEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);

    STDMETHOD_(BOOL,OnFireEvent)(IEvtArgs *evt);

    STDMETHOD_(CRect,GetContainerRect)() const;

    STDMETHOD_(IRenderTarget *,OnGetRenderTarget)(LPCRECT rc ,GrtFlag gdcFlags);

    STDMETHOD_(void,OnReleaseRenderTarget)(IRenderTarget *pRT,LPCRECT rc,GrtFlag gdcFlags);

    STDMETHOD_(void,OnRedraw)(LPCRECT rc);

    STDMETHOD_(BOOL,OnReleaseSwndCapture)();

    STDMETHOD_(SWND,OnSetSwndCapture)(SWND swnd);
    STDMETHOD_(HWND,GetHostHwnd)();
    STDMETHOD_(const SStringW &,GetTranslatorContext)() const;
    STDMETHOD_(void,FrameToHost)(RECT & rc) const;
    STDMETHOD_(BOOL,IsTranslucent)() const;
	STDMETHOD_(BOOL,IsSendWheel2Hover)() const;
    
    STDMETHOD_(BOOL,UpdateWindow)();

	STDMETHOD_(void,UpdateTooltip)();

    STDMETHOD_(IMessageLoop *,GetMsgLoop)();
    
    STDMETHOD_(IScriptModule *,GetScriptModule)();

	STDMETHOD_(int,GetScale)() const;

	STDMETHOD_(void,EnableIME)(BOOL bEnable);
public://SWindow
    virtual void ModifyItemState(DWORD dwStateAdd, DWORD dwStateRemove);

    virtual SWND SwndFromPoint(CPoint &pt,bool bIncludeMsgTransparent=false);

    virtual void Draw(IRenderTarget *pRT,const CRect & rc);

    virtual void SetSkin(ISkinObj *pSkin);
    virtual void SetColor(COLORREF crBk,COLORREF crSelBk);
    virtual void BeforePaint(IRenderTarget *pRT, SPainter &painter);

    virtual BOOL NeedRedrawWhenStateChange();
    virtual BOOL OnUpdateToolTip(CPoint pt, SwndToolTipInfo &tipInfo);
    virtual void RequestRelayout(SWND hSource ,BOOL bSourceResizable);
	virtual COLORREF GetBkgndColor() const;

    CRect GetItemRect() const;
    void SetItemCapture(BOOL bCapture);
    void SetItemData(LPARAM dwData);
    LPARAM GetItemData();

    LPARAM GetItemIndex()const {return m_lpItemIndex;}
    void SetItemIndex(LPARAM lp){m_lpItemIndex=lp;}

    void OnShowWindow(BOOL bShow, UINT nStatus);
    void OnDestroy();
    SOUI_MSG_MAP_BEGIN()
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_SHOWWINDOW(OnShowWindow)
    SOUI_MSG_MAP_END()
protected:
    SOUI_ATTRS_BEGIN()
        ATTR_COLOR(L"colorNormal",m_crBk,FALSE)
        ATTR_COLOR(L"colorSelected",m_crSelBk,FALSE)
        ATTR_COLOR(L"colorHover",m_crHover,FALSE)
    SOUI_ATTRS_END()
protected:
    SWindow * m_pFrmHost;
    IItemContainer * m_pItemContainer;
    COLORREF m_crBk, m_crSelBk,m_crHover;
    LPARAM        m_dwData;
    LPARAM        m_lpItemIndex;
};

BOOL SOUI_EXP IsItemInClip(const SMatrix &mtx,const CRect rcClip,const IRegion * clipRgn,const CRect & rcItem);

}//namespace SOUI