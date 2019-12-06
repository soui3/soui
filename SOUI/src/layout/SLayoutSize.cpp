#include "souistd.h"
#include "layout/SLayoutSize.h"
#pragma warning (push)
#pragma warning (disable: 4985) // disable the warning message during the include
#include <math.h>               // this is where I would normally get the warning message
#pragma warning (pop)

namespace SOUI
{
	static const wchar_t * kUnitMap[]=
	{
		L"px",
		L"dp",
		L"dip",
		L"sp",
	};

	SLayoutSize::SLayoutSize(float _fSize) :fSize(_fSize),unit(defUnit)
	{

	}

	SLayoutSize::SLayoutSize(float _fSize,Unit _unit):fSize(_fSize),unit(_unit)
	{
	}

	static int fround(float v)
	{
		return (int)floor(v+0.5f);
	}

	bool SLayoutSize::fequal(float a, float b)
	{
		return fabs(a-b)<0.00000001f;
	}

	bool SLayoutSize::valueEqual(float value)
	{
		return fequal(fSize,value);
	}

	SStringW SLayoutSize::toString() const
	{
		//copy from stringstream.
		return SStringW().Format(L"%.*g%s",8,fSize,kUnitMap[unit]);
	}


	bool SLayoutSize::isMatchParent() const
	{
		return fequal(fSize , SIZE_MATCH_PARENT);
	}

	void SLayoutSize::setMatchParent()
	{
		fSize = SIZE_MATCH_PARENT;
	}

	bool SLayoutSize::isWrapContent() const
	{
		return fequal(fSize , SIZE_WRAP_CONTENT);
	}

	void SLayoutSize::setWrapContent()
	{
		fSize = SIZE_WRAP_CONTENT;
	}

	void SLayoutSize::setSize(float fSize, Unit unit)
	{
		this->fSize = fSize;
		this->unit = unit;
	}

	bool SLayoutSize::isSpecifiedSize() const
	{
		return fSize>=SIZE_SPEC;
	}

	int SLayoutSize::toPixelSize(int scale) const
	{
		if(isMatchParent()) 
			return SIZE_MATCH_PARENT;
		else if(isWrapContent()) 
			return SIZE_WRAP_CONTENT;
		else if (unit == px)
			return (int)fSize;
		else//if(unit == dp || unit == dip || unit= sp)
			return (int)fround(fSize*scale / 100);
	}

	void SLayoutSize::setInvalid()
	{
		fSize = SIZE_UNDEF;
	}

	bool SLayoutSize::isValid() const
	{
		return !fequal(fSize,SIZE_UNDEF);
	}

	bool SLayoutSize::isZero() const
	{
		return fequal(fSize, 0.0f);
	}

	void SLayoutSize::parseString(const SStringW & strSize)
	{
		if(strSize.IsEmpty()) return;
		int cUnitSize = 0;
		for(int i=strSize.GetLength()-1;i>=0;i--)
		{
			wchar_t c = strSize[i];
			if((c>='a' && c<'z')||
				(c>='A' && c<='z'))
			{
				cUnitSize++;
			}else
			{
				break;
			}
		}
		if(cUnitSize>0)
		{
			SStringW strUnit= strSize.Right(cUnitSize);
			unit = unitFromString(strUnit);
			if(unit == unknow) 
				unit = defUnit;
			fSize = (float)_wtof(strSize.Left(strSize.GetLength()-cUnitSize));
		}else
		{
			unit = defUnit;
			fSize = (float)_wtof(strSize);
		}
	}

	//只复制数值,不复制方向
	SLayoutSize & SLayoutSize::operator=(const SLayoutSize & src)
	{
		fSize = src.fSize;
		unit = src.unit;
		return *this;
	}

	SLayoutSize SLayoutSize::fromString(const SStringW & strSize)
	{
		SLayoutSize ret;
		ret.parseString(strSize);
		return ret;
	}

	SLayoutSize::Unit SLayoutSize::setDefUnit(SLayoutSize::Unit unit)
	{
		if(unit == unknow)
			return unknow;
		Unit ret = defUnit;
		defUnit = unit;
		return ret;
	}

	SLayoutSize::Unit SLayoutSize::unitFromString(const SStringW & strUnit)
	{
		Unit ret = unknow;
		SStringW strUnit2 = strUnit;
		strUnit2.MakeLower();
		for (int i = 0; i < ARRAYSIZE(kUnitMap); i++)
		{
			if (strUnit2.Compare(kUnitMap[i]) == 0)
			{
				ret = (Unit)i;
				break;
			}
		}
		return ret;
	}

	SLayoutSize::Unit SLayoutSize::defUnit = SLayoutSize::px;
}
