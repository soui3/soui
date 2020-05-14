#include "souistd.h"
#include "res.mgr/SStylePool.h"

namespace SOUI
{

    //////////////////////////////////////////////////////////////////////////
    //SStylePool
    
    // Get style object from pool by class name
    SXmlNode SStylePool::GetStyle(LPCWSTR lpszName)
    {
        if(!HasKey(lpszName)) return SXmlNode();
        return GetKeyObject(lpszName);
    }

    // Load style-pool from xml tree
    BOOL SStylePool::Init(SXmlNode xmlStyleRoot)
    {
        if(!xmlStyleRoot) return FALSE;
        
        xmlStyleRoot = m_xmlDoc.root().append_copy(xmlStyleRoot);

		for (SXmlNode xmlChild = xmlStyleRoot.first_child(); xmlChild; xmlChild = xmlChild.next_sibling())
		{
			SStringW strClsName = xmlChild.name();
			if (strClsName.CompareNoCase(L"class") == 0)
			{
				strClsName = xmlChild.attribute(L"name").value();
				if (strClsName.IsEmpty()) continue;
				xmlChild.remove_attribute(L"name");//删除name属性，防止该属性被处理
			}
			SASSERT(!xmlChild.attribute(L"name"));
			AddKeyObject(strClsName, xmlChild);
		}

        return TRUE;
    }

    //////////////////////////////////////////////////////////////////////////
    // SStylePoolMgr
    SXmlNode SStylePoolMgr::GetStyle( LPCWSTR lpszName)
    {
        SPOSITION pos=m_lstStylePools.GetTailPosition();
        while(pos)
        {
            SStylePool *pStylePool=m_lstStylePools.GetPrev(pos);
            SXmlNode style = pStylePool->GetStyle(lpszName);
            if(style) return style; 
        }
        return SXmlNode();
    }

    void SStylePoolMgr::PushStylePool( SStylePool *pStylePool )
    {
        m_lstStylePools.AddTail(pStylePool);
        pStylePool->AddRef();
    }

    SStylePool * SStylePoolMgr::PopStylePool(SStylePool *pStylePool)
    {
        SStylePool * pRet=NULL;
        if(pStylePool)
        {
            SPOSITION pos=m_lstStylePools.Find(pStylePool);
            if(pos)
            {
                pRet=m_lstStylePools.GetAt(pos);
                m_lstStylePools.RemoveAt(pos);
            }
        }else
        {
            pRet = m_lstStylePools.RemoveTail();
        }
        if(pRet) pRet->Release();
        return pRet;
    }

    SStylePoolMgr::~SStylePoolMgr()
    {
        SPOSITION pos=m_lstStylePools.GetHeadPosition();
        while(pos)
        {
            SStylePool *p = m_lstStylePools.GetNext(pos);
            p->Release();
        }
        m_lstStylePools.RemoveAll();
    }

	/////////////////////////////////////////////////////////////////////
	BOOL STemplatePool::Init(SXmlNode xmlNode)
	{
		if (!xmlNode) return FALSE;
		for (SXmlNode xmlChild = xmlNode.first_child(); xmlChild; xmlChild = xmlChild.next_sibling())
		{
			SStringW strTempName = xmlChild.name();
			SStringW strValue = xmlChild.ToString();
			AddKeyObject(strTempName, strValue);
		}
		return TRUE;
	}

	SStringW STemplatePool::GetTemplateString(const SStringW & strName) const
	{
		SStringW strRet;
		GetKeyObject(strName, strRet);
		return strRet;
	}

	/////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// STemplatePoolMgr
	SStringW STemplatePoolMgr::GetTemplateString(const SStringW & strName)
	{
		SPOSITION pos = m_lstTemplatePools.GetTailPosition();
		while (pos)
		{
			STemplatePool *pStylePool = m_lstTemplatePools.GetPrev(pos);
			SStringW strRet = pStylePool->GetTemplateString(strName);
			if (!strRet.IsEmpty()) return strRet;
		}
		return SStringW();
	}

	void STemplatePoolMgr::PushTemplatePool(STemplatePool *pStylePool)
	{
		m_lstTemplatePools.AddTail(pStylePool);
		pStylePool->AddRef();
	}

	STemplatePool * STemplatePoolMgr::PopTemplatePool(STemplatePool *pStylePool)
	{
		STemplatePool * pRet = NULL;
		if (pStylePool)
		{
			SPOSITION pos = m_lstTemplatePools.Find(pStylePool);
			if (pos)
			{
				pRet = m_lstTemplatePools.GetAt(pos);
				m_lstTemplatePools.RemoveAt(pos);
			}
		}
		else
		{
			pRet = m_lstTemplatePools.RemoveTail();
		}
		if (pRet) pRet->Release();
		return pRet;
	}

	STemplatePoolMgr::~STemplatePoolMgr()
	{
		SPOSITION pos = m_lstTemplatePools.GetHeadPosition();
		while (pos)
		{
			STemplatePool *p = m_lstTemplatePools.GetNext(pos);
			p->Release();
		}
		m_lstTemplatePools.RemoveAll();
	}
}//end of namespace SOUI
