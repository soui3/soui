#include "stdafx.h"
#include "SPropertyItem-Group.h"
#include "../SPropertyGrid.h"

namespace SOUI
{
	void SPropertyGroup::DrawItem( IRenderTarget *pRT,CRect rc )
	{
		pRT->FillSolidRect(rc,GetOwner()->GetGroupColor());
	}
}