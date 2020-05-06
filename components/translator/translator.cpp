// translator.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "translator.h"
#include <search.h>
#include <ObjBase.h>
#include <tchar.h>

using namespace pugi;

namespace SOUI
{
    int StringCmp(const IStringW *str1,const IStringW *str2)
    {
		int nRet = str1->Compare(str2->c_str());
		if(nRet<0) return -1;
		else if(nRet>0) return 1;
		else return 0;
    }

    class SStrMap
    {
        friend class STranslatorMgr;
    public:
        SStringW strSource;
        SStringW strTranslation;

        static int  Compare(const void * e1, const void * e2);
        static int  CompareInSearch(const void * e1, const void * e2);
    };

    class SStrMapEntry
    {
        friend class STranslatorMgr;
    public:
        ~SStrMapEntry();
        SStringW strCtx;
        SArray<SStrMap*> m_arrStrMap;
        static int  Compare(const void * e1, const void * e2);
        static int  CompareInSearch(const void * e1, const void * e2);
    };


    int SStrMap::Compare( const void * e1, const void * e2)
    {
        SStrMap **p1=(SStrMap**) e1;
        SStrMap **p2=(SStrMap**) e2;
        return StringCmp(&(*p1)->strSource,&(*p2)->strSource);
    }

    int SStrMap::CompareInSearch( const void * e1, const void * e2 )
    {
        IStringW * pKey=(IStringW *)e1;
        SStrMap **p2=(SStrMap**) e2;
        return StringCmp(pKey,&(*p2)->strSource);     
    }


    int SStrMapEntry::Compare( const void * e1, const void * e2 )
    {
        SStrMapEntry **p1=(SStrMapEntry**) e1;
        SStrMapEntry **p2=(SStrMapEntry**) e2;
        return StringCmp(&(*p1)->strCtx,&(*p2)->strCtx);
    }
    
    int SStrMapEntry::CompareInSearch( const void * e1, const void * e2 )
    {
        IStringW *pKey=(IStringW*) e1;
        SStrMapEntry **p2=(SStrMapEntry**) e2;
        return StringCmp(pKey,&(*p2)->strCtx);
    }

    SStrMapEntry::~SStrMapEntry()
    {
        for(UINT i=0;i<m_arrStrMap.GetCount();i++)
            delete m_arrStrMap.GetAt(i);
    }


    //////////////////////////////////////////////////////////////////////////
    // SLang
	STranslator::STranslator()
    {
		m_szLangName[0]=0;
        m_arrEntry = new SArray<SStrMapEntry*>;
    }

    STranslator::~STranslator()
    {
        for(UINT i=0;i<m_arrEntry->GetCount();i++)
            delete m_arrEntry->GetAt(i);
        delete m_arrEntry;
    }

    void STranslator::GetName(wchar_t szName[TR_MAX_NAME_LEN])
    {
		wcscpy_s(szName,TR_MAX_NAME_LEN,m_szLangName);
    }


	bool STranslator::NameEqual(LPCWSTR pszName)
	{
		return wcscmp(m_szLangName,pszName) == 0;
	}

	GUID STranslator::guid()
    {
        return m_guid;
    }

    BOOL STranslator::Load( LPVOID pData,UINT uType )
    {
        switch(uType)
        {
        case LD_XML:
            return LoadFromXml((*(pugi::xml_node*)pData));
        }
        return FALSE;
    }
    
    BOOL STranslator::LoadFromXml( pugi::xml_node xmlLang )
    {
        wcscpy_s(m_szLangName,TR_MAX_NAME_LEN,xmlLang.attribute(L"name").value());
        
        OLECHAR szIID[100] = { 0 };
        wcscpy(szIID,xmlLang.attribute(L"guid").value());

        IIDFromString(szIID,&m_guid);
        
        int ctxCount=0;
        xml_node nodeCtx=xmlLang.child(L"context");
        while(nodeCtx)
        {
            ctxCount++;
            nodeCtx=nodeCtx.next_sibling(L"context");
        }
        m_arrEntry->SetCount(ctxCount);
        nodeCtx=xmlLang.child(L"context");
        for(int i=0;i<ctxCount;i++)
        {
            SASSERT(nodeCtx);
            int strCount=0;
            xml_node nodeStr=nodeCtx.child(L"message");
            while(nodeStr)
            {
                strCount++;
                nodeStr=nodeStr.next_sibling(L"message");
            }
            
            SStrMapEntry * strMapEntry = new SStrMapEntry;
            strMapEntry->strCtx=nodeCtx.attribute(L"name").value();
            strMapEntry->m_arrStrMap.SetCount(strCount);
            nodeStr=nodeCtx.child(L"message");
            for(int j=0;j<strCount;j++)
            {
                SASSERT(nodeStr);
                SStrMap * strMap= new SStrMap;
                strMap->strSource=nodeStr.child(L"source").text().get();
                strMap->strTranslation=nodeStr.child(L"translation").text().get();
                strMapEntry->m_arrStrMap.SetAt(j,strMap);
                nodeStr=nodeStr.next_sibling(L"message");
            }
            qsort(strMapEntry->m_arrStrMap.GetData(),strMapEntry->m_arrStrMap.GetCount(),sizeof(SStrMap*),SStrMap::Compare);
            m_arrEntry->SetAt(i,strMapEntry);
            nodeCtx=nodeCtx.next_sibling(L"context");
        }
        
        qsort(m_arrEntry->GetData(),m_arrEntry->GetCount(),sizeof(SStrMapEntry*),SStrMapEntry::Compare);

		m_strFontInfo = xmlLang.attribute(L"font").as_string();
        return TRUE;
    }

    int STranslator::tr( const IStringW & strSrc,const IStringW & strCtx,wchar_t *pszOut, int nBufLen ) const 
    {
        SStrMapEntry** pEntry = (SStrMapEntry**)bsearch(&strCtx,m_arrEntry->GetData(),m_arrEntry->GetCount(),sizeof(SStrMapEntry*),SStrMapEntry::CompareInSearch);
        if(pEntry)
        {
            SStrMap ** pMap=(SStrMap**)bsearch(&strSrc,(*pEntry)->m_arrStrMap.GetData(),(*pEntry)->m_arrStrMap.GetCount(),sizeof(SStrMap*),SStrMap::CompareInSearch);
            if(pMap)
            {//从指定的上下文中查找翻译
                SStringW strRet=(*pMap)->strTranslation;
				if(pszOut == NULL)
					return strRet.GetLength()+1;
					
				if(nBufLen < strRet.GetLength()+1) 
					return -1;

				wcscpy_s(pszOut,nBufLen,strRet);
                return strRet.GetLength()+1;
            }
        }
		if(!strCtx.IsEmpty())
		{//从空白上下文中查找
			return tr(strSrc,SStringW(),pszOut,nBufLen);
		}
        return FALSE;
    }

	void STranslator::getFontInfo(IStringW *strFont) const
	{
		strFont->Copy(&m_strFontInfo);
	}

    //////////////////////////////////////////////////////////////////////////
    //  STranslator
    BOOL STranslatorMgr::InstallTranslator(ITranslator *pTranslator)
    {
		if (m_szLangName[0]==0)
			pTranslator->GetName(m_szLangName);
		if (!pTranslator->NameEqual(m_szLangName))
			return FALSE;

        SPOSITION pos=m_lstLang->GetHeadPosition();
        while(pos)
        {
            ITranslator *p=m_lstLang->GetNext(pos);
            if(IsEqualGUID(pTranslator->guid(),p->guid()))
            {
                return FALSE;
            }
        }
        m_lstLang->AddHead(pTranslator);
        pTranslator->AddRef();

        return TRUE;
    }

    BOOL STranslatorMgr::UninstallTranslator(REFGUID id)
    {
        SPOSITION pos=m_lstLang->GetHeadPosition();
        while(pos)
        {
            SPOSITION posBackup=pos;
            ITranslator *p=m_lstLang->GetNext(pos);
            if(IsEqualGUID(id,p->guid()))
            {
                m_lstLang->RemoveAt(posBackup);
                p->Release();

                return TRUE;
            }
        }
        return FALSE;
    }

    STranslatorMgr::STranslatorMgr( void )
    {
		m_szLangName[0] = 0;
        m_lstLang=new SList<ITranslator*>;
    }

    STranslatorMgr::~STranslatorMgr( void )
    {
        SPOSITION pos=m_lstLang->GetHeadPosition();
        while(pos)
        {
            ITranslator *pLang=m_lstLang->GetNext(pos);
            pLang->Release();
        }
        delete m_lstLang;
    }

    int STranslatorMgr::tr(const IStringW & strSrc,const IStringW & strCtx,wchar_t *pszOut,int nBufLen)  const 
    {
        if(strSrc.IsEmpty()) return 0;
        SPOSITION pos=m_lstLang->GetHeadPosition();
        while(pos)
        {
            ITranslator *pLang=m_lstLang->GetNext(pos);
            int nRet = pLang->tr(strSrc,strCtx,pszOut,nBufLen);
			if(nRet > 0 || nRet == -1) return nRet;
        }
        return 0;
    }

    BOOL STranslatorMgr::CreateTranslator( ITranslator ** ppTranslator )
    {
        *ppTranslator = new STranslator;
        return TRUE;
    }


	void STranslatorMgr::SetLanguage(const IStringW & strLang)
	{
		if (strLang.Compare(m_szLangName)!=0)
		{
			SPOSITION pos = m_lstLang->GetHeadPosition();
			while (pos)
			{
				ITranslator *pTrans = m_lstLang->GetNext(pos);
				pTrans->Release();
			}
			m_lstLang->RemoveAll();
		}
		wcscpy_s(m_szLangName,TR_MAX_NAME_LEN, strLang.c_str());
	}

	void STranslatorMgr::GetLanguage(wchar_t szName[TR_MAX_NAME_LEN]) const
	{
		wcscpy_s(szName,TR_MAX_NAME_LEN,m_szLangName);
	}

    //////////////////////////////////////////////////////////////////////////
    //  
    namespace TRANSLATOR
    {
        BOOL SCreateInstance( IObjRef **ppTrans )
        {
            *ppTrans = new STranslatorMgr;
            return TRUE;
        }
    }
}
