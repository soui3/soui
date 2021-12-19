#pragma once
#include "../SPropertyItemBase.h"


namespace SOUI{

class SPropertyGroup : public SPropertyItemBase
{
	friend class SPropertyGrid;
	SOUI_CLASS_NAME(SPropertyGroup,L"propgroup")
public:
	static LPCWSTR GetInplaceItemStyleName(){return NULL;}
	virtual PropType GetType() const {return PT_GROUP;}
	virtual void DrawItem(IRenderTarget *pRT,CRect rc);
	virtual BOOL HasValue() const ;
	virtual void ClearValue() ;
	virtual void OnChildValueChanged( IPropertyItem *pChild );
protected:
	SPropertyGroup(SPropertyGrid *pOwner);
};

}