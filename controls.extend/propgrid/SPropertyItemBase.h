#pragma once

#include "SPropertyItem-i.h"
#include <sobject/sobject.hpp>
#include <helper/obj-ref-impl.hpp>

namespace SOUI
{
    class SPropertyItemBase : public TObjRefImpl<IPropertyItem>
    {
        SOUI_CLASS_NAME(SPropertyItemBase,L"propitembase")
    public:
        virtual ~SPropertyItemBase();

	public:
		STDMETHOD_(int,GetID)(THIS) SCONST OVERRIDE
		{
			return m_nID;
		}

		STDMETHOD_(void,SetID)(THIS_ int nID) OVERRIDE
		{
			m_nID = nID;
		}

		STDMETHOD_(LPCWSTR,GetName)(THIS) SCONST OVERRIDE
		{
			return m_strName.c_str();
		}
		STDMETHOD_(void,SetName)(THIS_ LPCWSTR pszName) OVERRIDE
		{
			m_strName = pszName;
		}

	public:
        virtual BOOL HasButton() const {return FALSE;}
        virtual int  GetLevel() const ;
        virtual BOOL IsExpand() const ;
        virtual void Expand(BOOL bExpend) ;

        virtual IPropertyItem * GetParent() const ;
        virtual void SetParent(IPropertyItem * pParent);
        virtual IPropertyItem * GetItem(PROPITEMTYPE type) const ;
		virtual IPropertyItem * GetChildById(int nID) const;

        virtual SPropertyGrid * GetOwner() const ;
        virtual BOOL InsertChild(IPropertyItem * pChild,IPropertyItem * pInsertAfter=IC_LAST);
        virtual BOOL RemoveChild(IPropertyItem * pChild);
        virtual int ChildrenCount() const;

		virtual SStringT GetTitle() const{return m_strTitle.IsEmpty()?S_CW2T(m_strName):m_strTitle;}
        virtual void SetTitle(const SStringT & strName){m_strTitle=strName;}
		virtual SStringW GetName2() const {return m_strName;}
        virtual SStringT GetDescription() const {return m_strDescription;}
        virtual void SetDescription(const SStringT & strDescription){m_strDescription =strDescription;}
        virtual SStringT GetValue() const {return _T("");}
        virtual void SetValue(const SStringT & strValue) {}

        virtual void AdjustInplaceActiveWndRect(CRect & rc){}
        virtual void DrawItem(IRenderTarget *pRT,CRect rc){}
        virtual BOOL IsInplaceActive() const {return m_bInplaceActive;}
        virtual void OnInplaceActive(BOOL bActive){ m_bInplaceActive = bActive;}
        virtual BOOL OnButtonClick(){ return FALSE;}
        virtual void OnValueChanged();
        virtual void OnChildValueChanged( IPropertyItem *pChild ){};
		virtual SList<IPropertyItem*>* GetItemList(){return &m_childs;};
		
		virtual BOOL IsReadOnly() const{return m_bReadOnly;}
		virtual void SetReadOnly(BOOL bReadOnly);
		virtual SStringW GetExtendType() const{return m_strExType;}

		IPropertyItem * FindChildByName(LPCWSTR pszName) const;
		IPropertyItem * FindChildById(int nID);
		
        SOUI_ATTRS_BEGIN()
            ATTR_STRINGT(L"title",m_strTitle,TRUE)
			ATTR_STRINGW(L"name",m_strName,FALSE)
			ATTR_INT(L"id",m_nID,FALSE)
            ATTR_STRINGT(L"description",m_strDescription,FALSE)
            ATTR_INT(L"readOnly",m_bReadOnly,FALSE)
            ATTR_CUSTOM(L"expanded",OnAttrExpanded)
			ATTR_STRINGW(L"extendType",m_strExType,FALSE)
        SOUI_ATTRS_END()

        STDMETHOD_(BOOL,InitFromXml)(THIS_ IXmlNode * xmlNode);
    protected:
        HRESULT OnAttrExpanded(const SStringW &  strValue,BOOL bLoading);

        SStringT        m_strTitle;
		SStringW		m_strName;
		int				m_nID;
        SStringT        m_strDescription;
		SStringW		m_strExType;

        SPropertyGrid * m_pOwner;
        IPropertyItem * m_pParent;

        typedef IPropertyItem* IPropertyItemPtr;
        typedef SList<IPropertyItemPtr> PropItemList;
        PropItemList    m_childs;

		BOOL            m_bExpanded:1;  //折叠或展开
		BOOL            m_bReadOnly:1;
		BOOL            m_bInplaceActive:1;
    protected:
        SPropertyItemBase(SPropertyGrid * pOwner)
            :m_pOwner(pOwner)
            ,m_pParent(NULL)
            ,m_bExpanded(TRUE)
            ,m_nID(0)
			,m_bReadOnly(FALSE)
            ,m_bInplaceActive(FALSE)
        {
            SASSERT(pOwner);
        }
    };

}