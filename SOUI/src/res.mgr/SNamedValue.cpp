//////////////////////////////////////////////////////////////////////////
//   File Name: SNamedValue.cpp
//////////////////////////////////////////////////////////////////////////

#include "souistd.h"
#include "res.mgr/SNamedValue.h"
#include "core/swndstyle.h"

namespace SOUI
{

//////////////////////////////////////////////////////////////////////////

const wchar_t KPrefixString[] = L"@string/";
const wchar_t KPrefixColor[] = L"@color/";
const wchar_t KPrefixDimension[] = L"@dim/";

int SIntParser::GetNullValue()
{
    return 0;
}

bool SIntParser::ParseValue(const SStringW & strValue,int & value)
{
    value = _wtoi(strValue);
    return true;
}


//////////////////////////////////////////////////////////////////////////
SStringW SStringParser::GetNullValue()
{
    return SStringW();
}

bool SStringParser::ParseValue(const SStringW & strValue, SStringW & value)
{
    value = strValue;
    return true;
}

SStringW SNamedString::Get(const SStringW & strValue) const
{
    if(strValue.IsEmpty()) return strValue;
    if(strValue.Left(ARRAYSIZE(KPrefixString)-1)==KPrefixString)
    {
        return String2Value(strValue.Mid(ARRAYSIZE(KPrefixString)-1));
    }else
    {
        return strValue;
    }
}

//////////////////////////////////////////////////////////////////////////


COLORREF SColorParser::GetNullValue()
{
    return CR_INVALID;
}

bool SColorParser::ParseValue(const SStringW & strValue, COLORREF & value)
{
	int r = 255, g = 255, b = 255, a = 255;
    int nSeg=0;
    SStringW strValueL = strValue;
    strValueL.MakeLower();
    if(strValueL.Left(1)==L"#")
    {
        nSeg = swscanf(strValueL,L"#%02x%02x%02x%02x",&r,&g,&b,&a);
    }else if(strValueL.Left(5).CompareNoCase(L"rgba2")==0)
    {
		float  percent=100.0f;
        nSeg = swscanf(strValueL,L"rgba2(%d,%d,%d,%f%%)",&r,&g,&b,&percent);
		if(nSeg == 4)
		{
			a = (int)(percent*255/100);
		}
    }else if(strValueL.Left(5).CompareNoCase(L"rgba3")==0)
    {
		float  ratio=1.0f;
        nSeg = swscanf(strValueL,L"rgba3(%d,%d,%d,%f)",&r,&g,&b,&ratio);
		if(nSeg == 4)
		{
			a = (int)(ratio*255);
		}
    }else if(strValueL.Left(4).CompareNoCase(L"rgba")==0)
    {
        nSeg = swscanf(strValueL,L"rgba(%d,%d,%d,%d)",&r,&g,&b,&a);                
    }else if(strValueL.Left(3).CompareNoCase(L"rgb")==0)
    {
        nSeg = swscanf(strValueL,L"rgb(%d,%d,%d)",&r,&g,&b);                
    }
    if(nSeg!=3 && nSeg != 4)
    {
        return false;
    }else
    {
        value = RGBA(r,g,b,a);
        return true;
    }
}

COLORREF SNamedColor::Get(const SStringW & strValue) const
{
    if(strValue.Left(ARRAYSIZE(KPrefixColor)-1)==KPrefixColor)
    {
        return String2Value(strValue.Mid(ARRAYSIZE(KPrefixColor)-1));
    }else
    {
		COLORREF crRet = SColorParser::GetNullValue();
        SColorParser::ParseValue(strValue, crRet);
        return crRet;
    }
}


////////////////////////////////////////////////////////////////////////////////////
bool SDimensionParser::ParseValue(const SStringW & strValue, SLayoutSize & value)
{
	value.parseString(strValue);
	return true;
}

SLayoutSize SDimensionParser::GetNullValue()
{
	return SLayoutSize();
}

SLayoutSize SNamedDimension::Get(const SStringW & strValue) const
{
	if (strValue.Left(ARRAYSIZE(KPrefixDimension) - 1) == KPrefixDimension)
	{
		return String2Value(strValue.Mid(ARRAYSIZE(KPrefixDimension) - 1));
	}
	else
	{
		SLayoutSize dimRet = SDimensionParser::GetNullValue();
		SDimensionParser::ParseValue(strValue, dimRet);
		return dimRet;
	}

}

/////////////////////////////////////////////////////////////////////////////////
void SNamedID::Init2(const NAMEDVALUE *pValue,int nCount,BOOL bSorted)
{
    m_lstNamedValue.RemoveAll();
    m_lstNamedValue.SetCount(nCount);
    for(int i=0;i<nCount;i++)
    {
        m_lstNamedValue.SetAt(i,pValue[i]);
    }
    if(!bSorted)
    {//自动排序
        qsort(m_lstNamedValue.GetData(),m_lstNamedValue.GetCount(),sizeof(NAMEDVALUE),Compare);
    }
}


}//namespace SOUI