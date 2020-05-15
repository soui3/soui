//////////////////////////////////////////////////////////////////////////
//  Class Name: SFontPool
// Description: Font Pool
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "souistd.h"
#include "res.mgr/sfontpool.h"
#include "helper/SplitString.h"
#include "layout/SLayoutSize.h"
#include "helper/SHostMgr.h"

namespace SOUI
{

template<> SFontPool* SSingleton<SFontPool>::ms_Singleton    = 0;

SFontPool::SFontPool(IRenderFactory *pRendFactory)
    :m_RenderFactory(pRendFactory)
{
    m_pFunOnKeyRemoved=OnKeyRemoved;
}

void SFontPool::OnKeyRemoved(const IFontPtr & obj)
{
	obj->Release();
}

IFontPtr SFontPool::GetFont(FONTSTYLE style, const SStringW & fontFaceName,SXmlNode xmlExProp)
{
	IFontPtr hftRet=0;

	SStringW strFace = fontFaceName;
	if(strFace.IsEmpty()) strFace = GetDefFontInfo().strFaceName;
	
	SStringW strXmlProp= xmlExProp.ToString();

	FontInfo info = {style.dwStyle,strFace,strXmlProp};

	SAutoLock autoLock(m_cs);

	if(HasKey(info))
	{
		hftRet=GetKeyObject(info);
	}
	else
	{
		hftRet = _CreateFont(info,xmlExProp);
		AddKeyObject(info,hftRet);
	}
	return hftRet;

}

static const WCHAR  KFontPropSeprator=   L',';   //字体属性之间的分隔符，不再支持其它符号。
static const WCHAR  KPropSeprator    =   L':';   //一个属性name:value对之间的分隔符
static const WCHAR  KAttrFalse[]     =   L"0";
static const WCHAR  KFontFace[]      =   L"face";
static const WCHAR  KFontBold[]      =   L"bold";
static const WCHAR  KFontUnderline[] =   L"underline";
static const WCHAR  KFontItalic[]    =   L"italic";
static const WCHAR  KFontStrike[]    =   L"strike";
static const WCHAR  KFontAdding[]    =   L"adding";
static const WCHAR  KFontSize[]      =   L"size";
static const WCHAR  KFontCharset[]   =   L"charset";
static const WCHAR  KFontWeight[]    =   L"weight";

IFontPtr SFontPool::GetFont( const SStringW & strFont ,int scale)
{
    FONTSTYLE fntStyle(GetDefFontInfo().style);
	fntStyle.attr.cSize = 0;

    SStringW strFace;//不需要默认字体, 在后面GetFont里会检查.
    SStringW attr=strFont; 
    attr.MakeLower();                                        
    SStringWList fontProp;
    SplitString(attr,KFontPropSeprator,fontProp);

	short cAdding = 0;
	short cSize = 0;

	SXmlDoc docExProp;
	SXmlNode nodePropEx = docExProp.root().append_child();
    for(int i=(int)fontProp.GetCount()-1;i>=0;i--)
    {
        SStringWList strPair;
        if(2!=SplitString(fontProp[i],KPropSeprator,strPair))
        {
            fontProp.RemoveAt(i);
            continue;
        }
        if(strPair[0] == KFontFace)
        {
            strFace = strPair[1];
        }else if(strPair[0] == KFontAdding)
        {
			SLayoutSize sz = GETLAYOUTSIZE(strPair[1]);
            cAdding=(short)sz.toPixelSize(scale);;
        }else if(strPair[0] == KFontSize)
        {
			SLayoutSize sz = GETLAYOUTSIZE(strPair[1]);
            cSize=(short)sz.toPixelSize(scale);
        }else if(strPair[0] == KFontItalic)
        {
            fntStyle.attr.fItalic = strPair[1] != KAttrFalse;
        }else if(strPair[0] == KFontBold)
        {
            fntStyle.attr.fBold = strPair[1] != KAttrFalse;
        }else if(strPair[0] == KFontStrike)
        {
            fntStyle.attr.fStrike = strPair[1] != KAttrFalse;
        }else if(strPair[0] == KFontUnderline)
        {
            fntStyle.attr.fUnderline = strPair[1] != KAttrFalse;
        }else if(strPair[0] == KFontCharset)
        {
            fntStyle.attr.byCharset = (BYTE)_wtoi(strPair[1]);
		}else if (strPair[0] == KFontWeight)
		{
			fntStyle.attr.byWeight = (_wtoi(strPair[1]) +2)/ 4;//+2 for 四舍五入. /4是为了把weight scale到0-250.
		}else
		{
			nodePropEx.append_attribute(strPair[0]).set_value(strPair[1]);
		}
    }

	if(cSize!=0)
	{//size property is higher than adding
		fntStyle.attr.cSize = cSize;
	}
	else
	{
		FONTSTYLE fontStyle(GetDefFontInfo().style);
		SLayoutSize defSize((float)fontStyle.attr.cSize);
		fntStyle.attr.cSize = defSize.toPixelSize(scale) + cAdding;  //cAdding为正代表字体变大，否则变小
	}

    return GetFont(fntStyle, strFace,nodePropEx);
}


IFontPtr SFontPool::_CreateFont(const LOGFONT &lf)
{
    SASSERT(m_RenderFactory);
    
    IFontPtr pFont=NULL;
    m_RenderFactory->CreateFont(&pFont,&lf);

    return pFont;
}

IFontPtr SFontPool::_CreateFont(const FontInfo &fontInfo,SXmlNode xmlExProp)
{
	LOGFONT lfNew={0};
        
	lfNew.lfCharSet     = fontInfo.style.attr.byCharset;

	//优先使用weigth属性.
	lfNew.lfWeight      = fontInfo.style.attr.byWeight * 4;
	if(lfNew.lfWeight == 0)//没有weight属性时检查bold属性.
		lfNew.lfWeight      = (fontInfo.style.attr.fBold ? FW_BOLD : FW_NORMAL);
    lfNew.lfUnderline   = (FALSE != fontInfo.style.attr.fUnderline);
    lfNew.lfItalic      = (FALSE != fontInfo.style.attr.fItalic);
    lfNew.lfStrikeOut   = (FALSE != fontInfo.style.attr.fStrike);
	lfNew.lfHeight = -abs((short)fontInfo.style.attr.cSize);
    lfNew.lfQuality = CLEARTYPE_QUALITY;
    
    _tcscpy_s(lfNew.lfFaceName,_countof(lfNew.lfFaceName), S_CW2T(fontInfo.strFaceName));

    IFontPtr ret = _CreateFont(lfNew);
	for(SXmlAttr attr=xmlExProp.first_attribute();attr;attr=attr.next_attribute())
	{
		ret->SetAttribute(attr.name(),attr.value(),FALSE);
	}
	ret->SetAttrFinish();
	return ret;
}

const FontInfo & SFontPool::GetDefFontInfo() const
{
	return m_defFontInfo;
}

void SFontPool::SetDefFontInfo(const FontInfo & fontInfo)
{
	SAutoLock autoLock(m_cs);
	m_defFontInfo = fontInfo;
	RemoveAll();
	SHostMgr::getSingletonPtr()->DispatchMessage(true,UM_UPDATEFONT);
}

void SFontPool::SetDefFontInfo(const SStringW & strFontInfo)
{
	FontInfo fi = FontInfoFromString(strFontInfo);
	SetDefFontInfo(fi);
}

FontInfo SFontPool::FontInfoFromString(const SStringW &strFontDesc)
{
	FontInfo fi;
	fi.style=0;
	SArray<SStringW> strLst;
	int nSeg = SplitString(strFontDesc,KFontPropSeprator,strLst);
	for(int i=0;i<nSeg;i++)
	{
		SArray<SStringW> kv;
		int n = SplitString(strLst[i],KPropSeprator,kv);
		if(n!=2) continue;
		if(kv[0].CompareNoCase(KFontFace)==0)
		{
			if(kv[1][0]==L'\'' || kv[1][0]==L'\"')
				fi.strFaceName = kv[1].Mid(1,kv[1].GetLength()-2);
			else
				fi.strFaceName=kv[1];			
		}else if(kv[0].CompareNoCase(KFontSize)==0)
		{
			fi.style.attr.cSize = abs(_wtoi(kv[1]));
		}else if(kv[0].CompareNoCase(KFontCharset)==0)
		{
			fi.style.attr.byCharset = _wtoi(kv[1]);
		}else if(kv[0].CompareNoCase(KFontWeight)==0)
		{
			fi.style.attr.byWeight=_wtoi(kv[1])/4;
		}else if(kv[0].CompareNoCase(KFontBold)==0)
		{
			fi.style.attr.fBold = _wtoi(kv[1]);
		}else if(kv[0].CompareNoCase(KFontItalic)==0)
		{
			fi.style.attr.fItalic = _wtoi(kv[1]);
		}else if(kv[0].CompareNoCase(KFontStrike)==0)
		{
			fi.style.attr.fStrike=_wtoi(kv[1]);
		}else if(kv[0].CompareNoCase(KFontUnderline)==0)
		{
			fi.style.attr.fUnderline=_wtoi(kv[1]);
		}
	}
	return fi;
}

SStringW SFontPool::FontInfoToString(const FontInfo &fi)
{
	char szBuf[200];
	Log4zStream s(szBuf,200);
	s<<KFontFace<<":\'"<<fi.strFaceName.c_str()<<"\'"<<",";
	s<<KFontSize<<":"<<(short)fi.style.attr.cSize<<",";
	s<<KFontCharset<<":"<<fi.style.attr.byCharset<<",";
	s<<KFontWeight<<":"<<fi.style.attr.byWeight*4<<",";
	s<<KFontBold<<":"<<(fi.style.attr.fBold?"1":"0")<<",";
	s<<KFontItalic<<":"<<(fi.style.attr.fItalic?"1":"0")<<",";
	s<<KFontStrike<<":"<<(fi.style.attr.fStrike?"1":"0")<<",";
	s<<KFontUnderline<<":"<<(fi.style.attr.fUnderline?"1":"0");
	return S_CA2W(szBuf);

}

}//namespace SOUI