#pragma once
#include "../SPropertyItemBase.h"


namespace SOUI{

class SPropertyGroup : public SPropertyItemBase
{
	friend class SPropertyGrid;
	SOUI_CLASS_NAME(SPropertyGroup,L"propgroup")
public:
	static LPCWSTR GetInplaceItemStyleName(){return NULL;}
	virtual BOOL IsGroup() const {return TRUE;}
	virtual void DrawItem(IRenderTarget *pRT,CRect rc);
protected:
	SPropertyGroup(SPropertyGrid *pOwner):SPropertyItemBase(pOwner)
	{
	}
};

}