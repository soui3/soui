#pragma once

namespace SOUI{

class SStaticGdip : public SStatic
{
	SOUI_CLASS_NAME(SStaticGdip,L"text2")
public:
	SStaticGdip(void);
	~SStaticGdip(void);

public:
	static BOOL initGdip();
	static void uninitGdip();
protected:
	void OnDrawLine(IRenderTarget * pRT, LPCTSTR pszBuf, int iBegin, int cchText, LPRECT pRect, UINT uFormat) override;
	SIZE OnMeasureText(IRenderTarget *pRT,LPCTSTR pszText,int cchLen) override;
private:
	static ULONG_PTR s_gdiPlusToken;
};

}

