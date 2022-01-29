#include "stdafx.h"
#include "SPropertyItem-Group.h"
#include "../SPropertyGrid.h"

namespace SOUI
{
	SPropertyGroup::SPropertyGroup(SPropertyGrid *pOwner) :SPropertyItemBase(pOwner)
	{
	}

	void SPropertyGroup::DrawItem( IRenderTarget *pRT,CRect rc )
	{
		pRT->FillSolidRect(rc,GetOwner()->GetGroupColor());
	}

	void SPropertyGroup::ClearValue()
	{

	}

	BOOL SPropertyGroup::HasValue() const
	{
		return ChildrenCount()!=0;
	}

	void SPropertyGroup::OnChildValueChanged(IPropertyItem *pChild)
	{
		GetOwner()->OnItemValueChanged(pChild);
	}

}