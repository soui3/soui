#pragma once
#include <unknown/obj-ref-i.h>
#include <interface/sobject-i.h>
namespace SOUI
{
	interface SOUI_EXP IInterpolator :  public IObject
	{
		virtual float getInterpolation(float input) const PURE;
	};
}