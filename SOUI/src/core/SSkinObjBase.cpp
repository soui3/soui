#include <souistd.h>
#include <core/SSkinObjBase.h>

namespace SOUI
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// handle state map
	SState2Index::SState2Index(const SState2Index & src)
	{
		m_mapOfStates = src.m_mapOfStates;
	}

	BOOL SState2Index::Init(pugi::xml_node xmlNode)
	{
		pugi::xml_node xmlStates = xmlNode.child(L"states");
		if(!xmlStates) 
			return FALSE;
		pugi::xml_node xmlState = xmlStates.child(L"state");

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

	int SState2Index::GetDefIndex(DWORD dwState)
	{
		if (dwState & WndState_Hover)
			return 1;
		else if (dwState & WndState_PushDown)
			return 2;
		else if (dwState & WndState_Disable)
			return 3;
		else // WndState_Normal
			return 0;
	}

	int SState2Index::GetIndex(DWORD dwState) const
	{
		if(m_mapOfStates.IsEmpty())
		{
			return GetDefIndex(dwState);
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
	void SSkinObjBase::OnInitFinished(pugi::xml_node xmlNode)
	{
		m_state2Index.Init(xmlNode);
	}

	int SSkinObjBase::State2Index(DWORD dwState) const
	{
		return m_state2Index.GetIndex(dwState);
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
	}

	ISkinObj * SSkinObjBase::Scale(int nScale)
	{
		ISkinObj * skinObj = SApplication::getSingleton().CreateSkinByName(GetObjectClass());
		if(!skinObj) return NULL;
		_Scale(skinObj,nScale);
		return skinObj;
	}

	int SSkinObjBase::GetScale() const
	{
		return m_nScale;
	}

	int SSkinObjBase::GetStates()
	{
		return 1;
	}

	BOOL SSkinObjBase::IgnoreState()
	{
		return TRUE;
	}

	SIZE SSkinObjBase::GetSkinSize()
	{
		SIZE ret = {0, 0};

		return ret;
	}

	void SSkinObjBase::DrawByIndex(IRenderTarget *pRT, LPCRECT rcDraw, int iState)
	{
		DrawByIndex(pRT, rcDraw, iState, GetAlpha());
	}

	void SSkinObjBase::DrawByIndex(IRenderTarget *pRT, LPCRECT rcDraw, int iState,BYTE byAlpha)
	{
		_DrawByIndex(pRT, rcDraw, iState, byAlpha);
	}


	void SSkinObjBase::DrawByState(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState)
	{
		DrawByState(pRT,rcDraw,dwState,GetAlpha());
	}

	void SSkinObjBase::DrawByState(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha)
	{
		_DrawByState(pRT,rcDraw,dwState,byAlpha);
	}

	void SSkinObjBase::_DrawByState(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha)
	{
		DrawByIndex(pRT,rcDraw,State2Index(dwState),byAlpha);
	}

	void SSkinObjBase::SetAlpha(BYTE byAlpha)
	{
		m_byAlpha = byAlpha;
	}

	LPCWSTR SSkinObjBase::GetName() const
	{
		return m_strName;
	}

}