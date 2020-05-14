#include <souistd.h>
#include <core/SSkinObjBase.h>

SNSBEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// handle state map
SState2Index::SState2Index(const SState2Index & src)
{
	m_mapOfStates = src.m_mapOfStates;
}

BOOL SState2Index::Init(IXmlNode *pNode)
{
	SXmlNode xmlNode(pNode);
	SXmlNode xmlStates = xmlNode.child(L"states");
	if(!xmlStates) 
		return FALSE;
	SXmlNode xmlState = xmlStates.child(L"state");

	while(xmlState)
	{
		int iState = xmlState.attribute(L"index").as_int();
		SStringW strValue = xmlState.attribute(L"value").as_string();
		strValue.MakeLower();
		SStringWList lstValues ;
		size_t nValues = SplitString(strValue,L'|',lstValues);
		DWORD dwState = WndState_Normal;
		for(size_t i = 0;i<nValues;i++)
		{
			dwState |= String2State(lstValues[i]);
		}

		m_mapOfStates[dwState] = iState;
		xmlState = xmlState.next_sibling(L"state");
	}
	return TRUE;
}

int SState2Index::GetDefIndex(DWORD dwState, bool checkAsPushdown /*= false*/)
{
	int idx = 0;
	if (dwState & WndState_Disable)
		idx = 3;
	else if (dwState & WndState_PushDown)
		idx = 2;
	else if (dwState & WndState_Hover)
		idx= 1;
	else // WndState_Normal
		idx = 0;
	if (dwState & WndState_Check)
	{
		if (checkAsPushdown)
			idx = 2;
		else
			idx += 4;
	}
	return idx;
}

int SState2Index::GetIndex(DWORD dwState,bool checkAsPushdown) const
{
	if(m_mapOfStates.IsEmpty())
	{
		int iRet = GetDefIndex(dwState, checkAsPushdown);

		return iRet;
	}else
	{
		const SMap<DWORD,int>::CPair *p = m_mapOfStates.Lookup(dwState);
		if(!p) return -1;
		return p->m_value;
	}
}

DWORD SState2Index::String2State(const SStringW & strState)
{
	const static struct _StateMap{
		LPCWSTR pszName;
		DWORD   dwValue;
	}kStateMap []={
		{L"normal",WndState_Normal},
		{L"hover",WndState_Hover},
		{L"pushdown",WndState_PushDown},
		{L"disable",WndState_Disable},
		{L"checked",WndState_Check},
	};
	static SMap<SStringW,DWORD> stateMap;
	static bool bInited = false;
	if(!bInited)
	{//init map
		for(int j=0;j<ARRAYSIZE(kStateMap);j++)
		{
			stateMap[kStateMap[j].pszName]=kStateMap[j].dwValue;
		}
	}
	SMap<SStringW,DWORD>::CPair * p = stateMap.Lookup(strState);
	if(!p) return WndState_Normal;
	return p->m_value;
}

////////////////////////////////////////////////////////////////////////////
SSkinObjBase::SSkinObjBase() 
:m_byAlpha(0xFF)
, m_bEnableColorize(true)
, m_crColorize(0)
, m_bEnableScale(true)
, m_nScale(100) 
, m_checkAsPushdown(true)
{}

void SSkinObjBase::OnInitFinished(IXmlNode * pNode)
{
	m_state2Index.Init(pNode);
}

int SSkinObjBase::State2Index(DWORD dwState) const
{
	return m_state2Index.GetIndex(dwState,m_checkAsPushdown);
}

BYTE SSkinObjBase::GetAlpha() const
{
	return m_byAlpha;
}

void SSkinObjBase::_Scale(ISkinObj *pObj, int nScale)
{
	SSkinObjBase * pSkinObj = sobj_cast<SSkinObjBase>(pObj);
	pSkinObj->m_nScale = nScale;
	pSkinObj->m_byAlpha = m_byAlpha;
	pSkinObj->m_bEnableColorize = m_bEnableColorize;
	pSkinObj->m_crColorize = m_crColorize;
	pSkinObj->m_strName = m_strName;
	pSkinObj->m_checkAsPushdown = m_checkAsPushdown;
}

ISkinObj * SSkinObjBase::Scale(int nScale)
{
	if(!m_bEnableScale)
	{
		AddRef();
		return this;
	}
	ISkinObj * skinObj = SApplication::getSingleton().CreateSkinByName(GetObjectClass());
	if(!skinObj) return NULL;
	_Scale(skinObj,nScale);
	return skinObj;
}

int SSkinObjBase::GetScale() const
{
	return m_nScale;
}

int SSkinObjBase::GetStates() const
{
	return 1;
}

SIZE SSkinObjBase::GetSkinSize() const
{
	SIZE ret = {0, 0};

	return ret;
}

void SSkinObjBase::DrawByIndex(IRenderTarget *pRT, LPCRECT rcDraw, int iState) const
{
	DrawByIndex(pRT, rcDraw, iState, GetAlpha());
}

void SSkinObjBase::DrawByIndex(IRenderTarget *pRT, LPCRECT rcDraw, int iState,BYTE byAlpha) const
{
	_DrawByIndex(pRT, rcDraw, iState, byAlpha);
}

void SSkinObjBase::DrawByState(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState) const
{
	DrawByState(pRT,rcDraw,dwState,GetAlpha());
}

void SSkinObjBase::DrawByState(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha) const
{
	_DrawByState(pRT,rcDraw,dwState,byAlpha);
}

void SSkinObjBase::_DrawByState(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha) const
{
	int idx = State2Index(dwState);
	DrawByIndex(pRT,rcDraw,idx,byAlpha);
}

void SSkinObjBase::SetAlpha(BYTE byAlpha)
{
	m_byAlpha = byAlpha;
}

LPCWSTR SSkinObjBase::GetName() const
{
	return m_strName;
}

void SSkinObjBase::OnColorize(COLORREF cr)
{
}

SNSEND