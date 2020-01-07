#pragma once

namespace SOUI
{
	struct IAttrStorage : public IObjRef
	{
		virtual void OnSetAttribute(const SStringW & strName, const SStringW & strValue,bool bHandled)=0;
		virtual SStringW OnGetAttribute(const SStringW & strName) const=0;
	};

	struct IAttrStorageFactory : public IObjRef
	{
		virtual HRESULT CreateAttrStorage(IAttrStorage** ppAttrStorage) const=0; 
	};

}