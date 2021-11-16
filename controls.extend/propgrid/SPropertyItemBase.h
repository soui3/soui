#pragma once

#include "SPropertyItem-i.h"
#include <sobject/sobject.hpp>
#include <unknown/obj-ref-i.h>
#include <unknown/obj-ref-impl.hpp>

namespace SOUI
{
    class SPropertyItemBase : public TObjRefImpl<IPropertyItem>
    {
        SOUI_CLASS_NAME(SPropertyItemBase,L"propitembase")
    public:
        virtual ~SPropertyItemBase();
        virtual LPCWSTR GetItemClass() const {return GetClassName();}

        virtual BOOL IsGroup() const {return FALSE;}
        virtual BOOL HasButton() const {return FALSE;}
        virtual int  GetLevel() const ;
        virtual BOOL IsExpand() const ;
        virtual void Expand(BOOL bExpend) ;

        virtual IPropertyItem * GetParent() const ;
        virtual void SetParent(IPropertyItem * pParent);
        virtual IPropertyItem * GetItem(PROPITEMTYPE type) const ;
        virtual SPropertyGrid * GetOwner() const ;
        virtual BOOL InsertChild(IPropertyItem * pChild,IPropertyItem * pInsertAfter=IC_LAST);
        virtual BOOL RemoveChild(IPropertyItem * pChild);
        virtual int ChildrenCount() const;

		virtual SStringT GetTitle() const{return m_strTitle.IsEmpty()?S_CW2T(m_strName):m_strTitle;}
        virtual void SetTitle(const SStringW & strName){m_strTitle=strName;}
		virtual LPCWSTR  GetName() const {return m_strName;}
		virtual SStringW GetName2() const {return m_strName;}
        virtual int GetID()const {return m_nID;}
        virtual void SetID(int nID) {m_nID = nID;}
        virtual SStringT GetDescription() const {return m_strDescription;}
        virtual void SetDescription(const SStringT & strDescription){m_strDescription =strDescription;}
        virtual SStringT GetString() const {return _T("");}
        virtual void SetString(const SStringT & strValue) {}

		virtual void SetStringOnly(const SStringT & strValue){};


        virtual void AdjustInplaceActiveWndRect(CRect & rc){rc.bottom--;}
        virtual void DrawItem(IRenderTarget *pRT,CRect rc){}
        virtual bool IsInplaceActive(){return m_bInplaceActive;}
        virtual void OnInplaceActive(bool bActive){ m_bInplaceActive = bActive;}
        virtual void OnButtonClick(){}
        virtual void OnValueChanged();
        virtual void OnChildValueChanged( IPropertyItem *pChild ){};
		virtual SList<IPropertyItem*>* GetItemList(){return &m_childs;};
		IPropertyItem * FindChildByName(LPCWSTR pszName) const;
		IPropertyItem * FindChildById(int nID);

        SOUI_ATTRS_BEGIN()
            ATTR_STRINGT(L"title",m_strTitle,TRUE)
			ATTR_STRINGW(L"name",m_strName,FALSE)
			ATTR_INT(L"id",m_nID,FALSE)
            ATTR_STRINGT(L"description",m_strDescription,FALSE)
            ATTR_INT(L"readOnly",m_bReadOnly,FALSE)
            ATTR_CUSTOM(L"expanded",OnAttrExpanded)
        SOUI_ATTRS_END()

        virtual BOOL InitFromXml(pugi::xml_node xmlNode);
    protected:
        HRESULT OnAttrExpanded(const SStringW &  strValue,BOOL bLoading);

        SStringW        m_strTitle;
		SStringW		m_strName;
		int				m_nID;
        SStringT        m_strDescription;

        SPropertyGrid * m_pOwner;
        IPropertyItem * m_pParent;

        typedef IPropertyItem* IPropertyItemPtr;
        typedef SList<IPropertyItemPtr> PropItemList;
        PropItemList    m_childs;

        BOOL            m_bExpanded;  //折叠或展开
        BOOL            m_bReadOnly;
        bool            m_bInplaceActive;
    protected:
        SPropertyItemBase(SPropertyGrid * pOwner)
            :m_pOwner(pOwner)
            ,m_pParent(NULL)
            ,m_bExpanded(TRUE)
            ,m_nID(0)
            ,m_bInplaceActive(false)
        {
            SASSERT(pOwner);
        }
    };

}