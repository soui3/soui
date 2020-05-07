#pragma once

namespace SOUI
{
	struct IAttrStorage : public IObjRef
	{
		virtual void OnSetAttribute(const IStringW * strName, const IStringW * strValue,bool bHandled)=0;
		virtual BOOL OnGetAttribute(const IStringW * strName, IStringW *strValue) const=0;
	};

	struct IAttrStorageFactory : public IObjRef
	{
		virtual HRESULT CreateAttrStorage(SWindow * owner,IAttrStorage** ppAttrStorage) const=0; 
	};

}