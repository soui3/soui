#pragma once

namespace SOUI
{
    #define IC_FIRST    (IPropertyItem *)0
    #define IC_LAST     (IPropertyItem *)1

	typedef enum _PropType{
		PT_UNKNOWN=0,
		PT_GROUP,
		PT_TEXT,
		PT_SIZE,
		PT_OPTION,
		PT_COLOR,
	}PropType;

    class SPropertyGrid;
    struct IPropertyItem : public SObject
    {
        enum PROPITEMTYPE{
            GPI_PARENT,
            GPI_FIRSTCHILD,
            GPI_LASTCHILD,
            GPI_NEXTSIBLING,
            GPI_PREVSIBLINIG,
        };
        virtual ~IPropertyItem(){}
		virtual PropType GetType() const = 0;
        virtual BOOL HasButton() const =0;
        virtual int  GetLevel() const =0;
        virtual BOOL IsExpand() const =0;
        virtual void Expand(BOOL bExpend) =0;
		virtual SStringW GetExtendType() const =0;
        virtual IPropertyItem * GetParent() const =0;
        virtual void SetParent(IPropertyItem * pParent) =0;
        virtual IPropertyItem * GetItem(PROPITEMTYPE  type) const =0;
		virtual IPropertyItem * GetChildById(int nID) const = 0;
        virtual SPropertyGrid * GetOwner() const =0;
        virtual BOOL InsertChild(IPropertyItem * pChild,IPropertyItem * pInsertAfter=IC_LAST)=0;
        virtual BOOL RemoveChild(IPropertyItem * pChild)=0;
        virtual int ChildrenCount() const =0;

        virtual SStringT GetTitle() const =0;
		virtual SStringW GetName2() const = 0;
        virtual void SetTitle(const SStringT & strName) =0;
        //virtual void SetID(int nID) =0;
        virtual SStringT GetDescription() const =0;
        virtual void SetDescription(const SStringT & strDescription) =0;
        virtual SStringT GetValue() const =0;
        virtual void SetValue(const SStringT & strValue) =0;

        virtual void DrawItem(IRenderTarget *pRT,CRect rc) =0;
        virtual void AdjustInplaceActiveWndRect(CRect & rc)=0;
        virtual BOOL IsInplaceActive() const =0;
        virtual void OnInplaceActive(BOOL bActive)=0;
        virtual BOOL OnButtonClick()=0;
        virtual void OnValueChanged()=0;
        virtual void OnChildValueChanged(IPropertyItem *pChild)=0;

		virtual BOOL IsReadOnly() const = 0;
		virtual void SetReadOnly(BOOL bReadOnly) =0;

		virtual BOOL HasValue() const = 0;
		virtual void ClearValue() = 0;

		virtual IPropertyItem * FindChildByName(LPCWSTR pszName) const = 0;
		virtual IPropertyItem * FindChildById(int nID) = 0;
    };

    struct IPropInplaceWnd{
        virtual IPropertyItem * GetOwner() =0;
        virtual void UpdateData() =0;
    };

}