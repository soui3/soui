#pragma once
#include <interface/SNativeWnd-i.h>
#include <sobject/Sobject.hpp>
#include <event/SEvents.h>
#include <event/SEventSet.h>

#include "SSingleton2.h"
//////////////////////////////////////////////////////////////////////////
// thunk 技术实现参考http://www.cppblog.com/proguru/archive/2008/08/24/59831.html
//////////////////////////////////////////////////////////////////////////
SNSBEGIN

	class SOUI_EXP SNativeWndHelper: public SSingleton2<SNativeWndHelper>{
		SINGLETON2_TYPE(SINGLETON_SIMPLEWNDHELPER)
    public:
        HANDLE GetHeap(){return m_hHeap;}

        void LockSharePtr(void * p);
        void UnlockSharePtr();
        void * GetSharePtr(){return m_sharePtr;}

        HINSTANCE GetAppInstance(){return m_hInst;}
        ATOM GetSimpleWndAtom(){return m_atom;}
    private:
        SNativeWndHelper(HINSTANCE hInst,LPCTSTR pszClassName, BOOL bImeApp);
        ~SNativeWndHelper();

        HANDLE                m_hHeap;
        CRITICAL_SECTION    m_cs;
        void *                m_sharePtr;

        ATOM                m_atom;
        HINSTANCE            m_hInst;
    };

#if defined(_M_IX86)
// 按一字节对齐
#pragma pack(push, 1)
struct tagThunk
{
    DWORD m_mov;  // 4个字节
    DWORD m_this;
    BYTE  m_jmp;
    DWORD m_relproc;
    //关键代码   //////////////////////////////////////
    void Init(DWORD_PTR proc, void* pThis)
    {
        m_mov = 0x042444C7;
        m_this = (DWORD)(ULONG_PTR)pThis;  // mov [esp+4], pThis;而esp+4本来是放hWnd,现在被偷着放对象指针了.
        m_jmp = 0xe9;
        // 跳转到proc指定的入口函数
        m_relproc = (DWORD)((INT_PTR)proc - ((INT_PTR)this + sizeof(tagThunk)));
        // 告诉CPU把以上四条语句不当数据，当指令,接下来用GetCodeAddress获得的指针就会运行此指令
        FlushInstructionCache(GetCurrentProcess(), this, sizeof(tagThunk));
    }
    void* GetCodeAddress()
    {
        return this; // 指向this,那么由GetCodeAddress获得的函数pProc是从DWORD m_mov;开始执行的
    }
};
#pragma pack(pop)
#elif defined(_M_AMD64)
#pragma pack(push,2)
struct tagThunk
{
    USHORT  RcxMov;         // mov rcx, pThis
    ULONG64 RcxImm;         //
    USHORT  RaxMov;         // mov rax, target
    ULONG64 RaxImm;         //
    USHORT  RaxJmp;         // jmp target
    void Init(DWORD_PTR proc, void *pThis)
    {
        RcxMov = 0xb948;          // mov rcx, pThis
        RcxImm = (ULONG64)pThis;  //
        RaxMov = 0xb848;          // mov rax, target
        RaxImm = (ULONG64)proc;   //
        RaxJmp = 0xe0ff;          // jmp rax
        FlushInstructionCache(GetCurrentProcess(), this, sizeof(tagThunk));
    }
    //some thunks will dynamically allocate the memory for the code
    void* GetCodeAddress()
    {
        return this;
    }
};
#pragma pack(pop)
#elif defined(_ARM_)
#pragma pack(push,4)
    struct tagThunk // this should come out to 16 bytes
    {
        DWORD    m_mov_r0;        // mov    r0, pThis
        DWORD    m_mov_pc;        // mov    pc, pFunc
        DWORD    m_pThis;
        DWORD    m_pFunc;
        void Init(DWORD_PTR proc, void* pThis)
        {
            m_mov_r0 = 0xE59F0000;
            m_mov_pc = 0xE59FF000;
            m_pThis = (DWORD)pThis;
            m_pFunc = (DWORD)proc;
            // write block from data cache and
            //  flush from instruction cache
            FlushInstructionCache(GetCurrentProcess(), this, sizeof(tagThunk));
        }
        void* GetCodeAddress()
        {
            return this;
        }
    };
#pragma pack(pop)
#else
#error Only AMD64, ARM and X86 supported
#endif

class  SOUI_EXP SNativeWnd : public TObjRefImpl<SObjectImpl<INativeWnd>>
{
	SOUI_CLASS_NAME_EX(TObjRefImpl<SObjectImpl<INativeWnd>>,L"SNativeWnd",NativeWnd)
public:
    SNativeWnd();
    virtual ~SNativeWnd(void);

    static ATOM RegisterSimpleWnd(HINSTANCE hInst,LPCTSTR pszSimpleWndName);

	static ATOM RegisterSimpleWnd2(HINSTANCE hInst, LPCTSTR pszSimpleWndName);

	STDMETHOD_(int,GetID)(THIS) SCONST {return GetDlgCtrlID();}
	STDMETHOD_(void,SetID)(THIS_ int nID) {SetWindowLongPtr(GWL_ID,nID);}

	STDMETHOD_(HWND,CreateWindow)(THIS_ LPCTSTR lpWindowName, DWORD dwStyle,DWORD dwExStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent,LPVOID lpParam ) OVERRIDE;

	STDMETHOD_(HWND,GetHwnd)(THIS) OVERRIDE;

	STDMETHOD_(BOOL,SubclassWindow)(THIS_ HWND hWnd) OVERRIDE;

	STDMETHOD_(HWND,UnsubclassWindow)(THIS_ BOOL bForce = FALSE) OVERRIDE;

	STDMETHOD_(const MSG *, GetCurrentMessage)(THIS) SCONST OVERRIDE;

	STDMETHOD_(int,GetDlgCtrlID)(THIS) SCONST OVERRIDE;

	STDMETHOD_(DWORD,GetStyle)(THIS) SCONST OVERRIDE;

	STDMETHOD_(DWORD,GetExStyle)(THIS) SCONST OVERRIDE;

	STDMETHOD_(LONG_PTR,GetWindowLongPtr)(THIS_ int nIndex) SCONST OVERRIDE;

	STDMETHOD_(LONG_PTR,SetWindowLongPtr)(THIS_ int nIndex, LONG_PTR dwNewLong) OVERRIDE;

	STDMETHOD_(HWND,GetParent)(THIS) OVERRIDE;

	STDMETHOD_(HWND,SetParent)(THIS_ HWND hWndNewParent) OVERRIDE;

	STDMETHOD_(BOOL,IsWindowEnabled)(THIS) SCONST OVERRIDE;

	STDMETHOD_(BOOL,ModifyStyle)(THIS_ DWORD dwRemove, DWORD dwAdd, UINT nFlags=0) OVERRIDE;

	STDMETHOD_(BOOL,ModifyStyleEx)(THIS_ DWORD dwRemove, DWORD dwAdd, UINT nFlags=0) OVERRIDE;

	STDMETHOD_(BOOL,SetWindowPos)(THIS_ HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags) OVERRIDE;

	STDMETHOD_(BOOL,CenterWindow)(THIS_ HWND hWndCenter = NULL) OVERRIDE;

	STDMETHOD_(BOOL,DestroyWindow)(THIS) OVERRIDE;

	STDMETHOD_(BOOL,IsWindow)(THIS) OVERRIDE;

	STDMETHOD_(BOOL,Invalidate)(THIS_ BOOL bErase = TRUE) OVERRIDE;

	STDMETHOD_(BOOL,InvalidateRect)(THIS_ LPCRECT lpRect, BOOL bErase = TRUE) OVERRIDE;

	STDMETHOD_(BOOL,GetWindowRect)(THIS_ LPRECT lpRect) SCONST OVERRIDE;

	STDMETHOD_(BOOL,GetClientRect)(THIS_ LPRECT lpRect) SCONST OVERRIDE;

	STDMETHOD_(BOOL,ClientToScreen)(THIS_ LPPOINT lpPoint) SCONST OVERRIDE;

	STDMETHOD_(BOOL,ClientToScreen2)(THIS_ LPRECT lpRect) SCONST OVERRIDE;

	STDMETHOD_(BOOL,ScreenToClient)(THIS_ LPPOINT lpPoint) SCONST OVERRIDE;

	STDMETHOD_(BOOL,ScreenToClient2)(THIS_ LPRECT lpRect) SCONST OVERRIDE;

	STDMETHOD_(int,MapWindowPoints)(THIS_ HWND hWndTo, LPPOINT lpPoint, UINT nCount) SCONST OVERRIDE;

	STDMETHOD_(int,MapWindowRect)(THIS_ HWND hWndTo, LPRECT lpRect) SCONST OVERRIDE;

	STDMETHOD_(UINT_PTR,SetTimer)(THIS_ UINT_PTR nIDEvent, UINT nElapse, void (CALLBACK* lpfnTimer)(HWND, UINT, UINT_PTR, DWORD) = NULL) OVERRIDE;

	STDMETHOD_(BOOL,KillTimer)(THIS_ UINT_PTR nIDEvent) OVERRIDE;

	STDMETHOD_(HDC,GetDC)(THIS) OVERRIDE;

	STDMETHOD_(HDC,GetWindowDC)(THIS) OVERRIDE;

	STDMETHOD_(int,ReleaseDC)(THIS_ HDC hDC) OVERRIDE;

	STDMETHOD_(BOOL,CreateCaret)(THIS_ HBITMAP hBitmap) OVERRIDE;

	STDMETHOD_(BOOL,HideCaret)(THIS) OVERRIDE;

	STDMETHOD_(BOOL,ShowCaret)(THIS) OVERRIDE;

	STDMETHOD_(HWND,SetCapture)(THIS) OVERRIDE;

	STDMETHOD_(HWND,SetFocus)(THIS) OVERRIDE;

	STDMETHOD_(LRESULT,SendMessage)(THIS_ UINT message, WPARAM wParam = 0, LPARAM lParam = 0) OVERRIDE;

	STDMETHOD_(BOOL,PostMessage)(THIS_ UINT message, WPARAM wParam = 0, LPARAM lParam = 0) OVERRIDE;

	STDMETHOD_(BOOL,SendNotifyMessage)(THIS_ UINT message, WPARAM wParam = 0, LPARAM lParam = 0) OVERRIDE;

	STDMETHOD_(BOOL,SetWindowText)(THIS_ LPCTSTR lpszString) OVERRIDE;

	STDMETHOD_(int,GetWindowText)(THIS_ LPTSTR lpszStringBuf, int nMaxCount) SCONST OVERRIDE;

	STDMETHOD_(BOOL,IsIconic)(THIS) SCONST OVERRIDE;

	STDMETHOD_(BOOL,IsZoomed)(THIS) SCONST OVERRIDE;

	STDMETHOD_(BOOL,IsWindowVisible)(THIS) SCONST OVERRIDE;

	STDMETHOD_(BOOL,MoveWindow)(THIS_ int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE) OVERRIDE;

	STDMETHOD_(BOOL,MoveWindow2)(THIS_ LPCRECT lpRect, BOOL bRepaint = TRUE) OVERRIDE;

	STDMETHOD_(BOOL,ShowWindow)(THIS_ int nCmdShow) OVERRIDE;

	STDMETHOD_(int,SetWindowRgn)(THIS_ HRGN hRgn,BOOL bRedraw= TRUE) OVERRIDE;

	STDMETHOD_(BOOL,SetLayeredWindowAttributes)(THIS_ COLORREF crKey,BYTE bAlpha,DWORD dwFlags) OVERRIDE;

	STDMETHOD_(BOOL,UpdateLayeredWindow)(THIS_ HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, POINT *pptSrc,COLORREF crKey, BLENDFUNCTION *pblend,DWORD dwFlags) OVERRIDE;

	STDMETHOD_(BOOL,SubscribeEvent)(THIS_ const IEvtSlot *pSlot) OVERRIDE;

	STDMETHOD_(BOOL,UnsubscribeEvent)(THIS_ const IEvtSlot *pSlot) OVERRIDE;

    LRESULT DefWindowProc();
    LRESULT ForwardNotifications(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT ReflectNotifications(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    static BOOL DefaultReflectionHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

public://EXTRACT FROM BEGIN_MSG_MAP_EX and END_MSG_MAP
    virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0);
protected:
    LRESULT DefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual void OnFinalMessage(HWND hWnd);

    const MSG * m_pCurrentMsg;
    BOOL m_bDestoryed;
	SEventSet m_evtSet;
public:

    HWND m_hWnd;
protected:
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg,
                                       WPARAM wParam, LPARAM lParam);

    // 只执行一次
    static LRESULT CALLBACK StartWindowProc(HWND hWnd, UINT uMsg,
                                            WPARAM wParam, LPARAM lParam);

    tagThunk *m_pThunk;
    WNDPROC    m_pfnSuperWindowProc;
};

SNSEND