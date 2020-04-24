
#include "string/sstringdata.h"
#include "soui_mem_wrapper.h"
#include <Windows.h>

namespace SOUI
{
    // Globals

    // For an empty string, m_pszData will point here
    // (note: avoids special case of checking for NULL m_pszData)
    // empty string data (and locked)
    static int _tstr_rgInitData[] = { -1, 0, 0, 0, 0, 0, 0, 0 };

	TStringData* TStringData::InitDataNil()
	{
		static TStringData* _tstr_initDataNil = (TStringData*)&_tstr_rgInitData;
		return _tstr_initDataNil;
	}

	const void* TStringData::InitPszNil()
	{
		static const void* _tstr_initPszNil = (const void*)(((unsigned char*)&_tstr_rgInitData) + sizeof(TStringData));
		return _tstr_initPszNil;
	}

	void* TStringData::data() const
	{
		return (void*)(this + 1);
	}

	void TStringData::Unlock()
	{
		SASSERT(IsLocked());
		if (IsLocked())
		{
			nRefs++;  // Locked buffers can't be shared, so no interlocked operation necessary
			if (nRefs == 0)
				nRefs = 1;
		}
	}

	void TStringData::Lock()
	{
		SASSERT(nRefs <= 1);
		nRefs--;  // Locked buffers can't be shared, so no interlocked operation necessary
		if (nRefs == 0)
			nRefs = -1;
	}

	bool TStringData::IsLocked() const
	{
		return nRefs < 0;
	}

	bool TStringData::IsShared() const
	{
		return nRefs > 1;
	}

	void TStringData::Release()
	{
		SASSERT(nRefs != 0);
		if (InterlockedDecrement(&nRefs) <= 0)
			soui_mem_wrapper::SouiFree(this);
	}

	void TStringData::AddRef()
	{
		SASSERT(nRefs > 0);
		InterlockedIncrement(&nRefs);
	}
}
