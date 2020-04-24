#pragma once
#include "utilities-def.h"
namespace SOUI
{
	struct TStringData
	{
		long nRefs;            // Reference count: negative == locked
		int nDataLength;    // Length of currently used data in XCHARs (not including terminating null)
		int nAllocLength;    // Length of allocated data in XCHARs (not including terminating null)

		void* data() const;

		void AddRef();
		void Release();
		bool IsShared() const;
		bool IsLocked() const;

		void Lock();
		void Unlock();

		static TStringData* InitDataNil();
		static const void*  InitPszNil();
	};
}