#pragma once

#include <interface/sresprovidermgr-i.h>
#include <atl.mini/scomcli.h>
#include <helper/SCriticalSection.h>
#include <res.mgr/SUiDef.h>

SNSBEGIN

class SOUI_EXP SResProviderMgr : public IResProviderMgr
{

public:
	SResProviderMgr(void);
	~SResProviderMgr(void);

public:
	STDMETHOD_(void,AddResProvider)(THIS_ IResProvider * pResProvider,LPCTSTR pszUidef = _T("uidef:xml_init")) OVERRIDE;

	STDMETHOD_(void,RemoveResProvider)(THIS_ IResProvider * pResProvider) OVERRIDE;

	STDMETHOD_(void,RemoveAll)(THIS) OVERRIDE;

	STDMETHOD_(IResProvider *,GetHeadResProvider)(THIS) SCONST OVERRIDE;
	STDMETHOD_(IResProvider *,GetTailResProvider)(THIS) SCONST OVERRIDE;

	STDMETHOD_(void,SetFilePrefix)(THIS_ LPCTSTR pszFilePrefix) OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	STDMETHOD_(BOOL,HasResource)(THIS_ LPCTSTR pszType,LPCTSTR pszResName) OVERRIDE;

	STDMETHOD_(HICON,LoadIcon)(THIS_ LPCTSTR pszResName,int cx=0,int cy=0,BOOL bFromFile = FALSE) OVERRIDE;

	STDMETHOD_(HCURSOR,LoadCursor)(THIS_ LPCTSTR pszResName,BOOL bFromFile = FALSE) OVERRIDE;

	STDMETHOD_(HBITMAP,LoadBitmap)(THIS_ LPCTSTR pszResName,BOOL bFromFile = FALSE) OVERRIDE;

	STDMETHOD_(IBitmap *,LoadImage)(THIS_ LPCTSTR pszType,LPCTSTR pszResName) OVERRIDE;

	STDMETHOD_(IImgX *,LoadImgX)(THIS_ LPCTSTR pszType,LPCTSTR pszResName) OVERRIDE;

	STDMETHOD_(size_t,GetRawBufferSize)(THIS_ LPCTSTR pszType,LPCTSTR pszResName) OVERRIDE;

	STDMETHOD_(BOOL,GetRawBuffer)(THIS_ LPCTSTR pszType,LPCTSTR pszResName,LPVOID pBuf,size_t size) OVERRIDE;

public:
	//从字符串返回颜色值，字符串可以是：@color/red (red是在资源包中的颜色表定义的颜色名)，也可以是rgba(r,g,b,a)，也可以是rgb(r,g,b)，还可以是#ff0000(ff)这样的格式
	COLORREF GetColor(const SStringW & strColor);
	//获取资源包中的颜色表中固定索引号的颜色，只支持从第一个资源包中查询
	COLORREF GetColor(int idx);

	//从字符串名返回字符串值，字符串可以是：@string/title (title是在资源包中的字符串表定义的命名字符串)
	SStringW GetString(const SStringW & strString);
	//获取资源包中的字符串表中固定索引号的字符串，只支持从第一个资源包中查询
	SStringW GetString(int idx);

	//从字符串名返回LayoutSize，字符串可以是：@dim/something (something是在资源包中的Dim表定义的命名字符串)
	SLayoutSize GetLayoutSize(const SStringW & strSize);
	SLayoutSize GetLayoutSize(int idx);
public://helper
	//find the match resprovider from tail to head, which contains the specified resource type and name
	IResProvider * GetMatchResProvider(LPCTSTR pszType,LPCTSTR pszResName);

	//使用type:name形式的字符串加载图片
	IBitmap * LoadImage2(const SStringW & strImgID);

	//使用name:size形式的字符串加载图标，如果没有size,则默认系统图标SIZE
	HICON     LoadIcon2(const SStringW & strIconID);
protected:
#ifdef _DEBUG
	static BOOL CALLBACK CheckUsage(LPCTSTR pszName,LPCTSTR pszType,LPARAM lp);
#endif
	LPCTSTR SysCursorName2ID(LPCTSTR pszCursorName);

	//检查资源类型是否为外部文件
	//使用file:xxx 的形式来引用外部文件资源
	BOOL    IsFileType(LPCTSTR pszType);

	SStringT	m_strFilePrefix;
	SList<IResProvider*> m_lstResPackage;

	typedef SMap<SStringT,HCURSOR> CURSORMAP;
	CURSORMAP  m_mapCachedCursor;

	SCriticalSection    m_cs;

#ifdef _DEBUG
	//资源使用计数
	SMap<SStringT,int> m_mapResUsageCount;
#endif
};

SNSEND