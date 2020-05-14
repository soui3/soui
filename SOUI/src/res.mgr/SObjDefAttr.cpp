#include "souistd.h"
#include "res.mgr/SObjDefAttr.h"
#include "SApp.h"

namespace SOUI
{

BOOL SObjDefAttr::Init( SXmlNode xmlNode )
{
    if(!xmlNode) return FALSE;

    m_xmlRoot.root().append_copy(xmlNode);

    SXmlNode xmlObjAttr=m_xmlRoot.root().child(L"objattr").first_child();
    while(xmlObjAttr)
    {
        AddKeyObject(xmlObjAttr.name(),xmlObjAttr);
        xmlObjAttr=xmlObjAttr.next_sibling();
    }

    SPOSITION pos=m_mapNamedObj->GetStartPosition();
    while(pos)
    {
        SMap<SStringW,SXmlNode>::CPair *p=m_mapNamedObj->GetNext(pos);
        BuildClassAttribute(p->m_value,p->m_key);
    }

    return TRUE;
}

void SObjDefAttr::BuildClassAttribute( SXmlNode & xmlNode, LPCWSTR pszClassName)
{
    SObjectInfo baseClassInfo =SApplication::getSingleton().BaseObjectInfoFromObjectInfo(SObjectInfo(pszClassName,Window));
    if(!baseClassInfo.IsValid()) return;

    if(HasKey(baseClassInfo.mName))
    {
        SXmlNode xmlNodeAttrs = GetKeyObject(baseClassInfo.mName);
        SXmlAttr attr=xmlNodeAttrs.first_attribute();
        while(attr)
        {
            if(!xmlNode.attribute(attr.name()))
                xmlNode.append_attribute(attr.name()).set_value(attr.value());
            attr=attr.next_attribute();
        }
    }
    BuildClassAttribute(xmlNode, baseClassInfo.mName);
}

SXmlNode SObjDefAttr::GetDefAttribute(LPCWSTR pszClassName )
{
    SASSERT(pszClassName);
	if (!HasKey(pszClassName))
	{
		SObjectInfo baseClassInfo = SApplication::getSingleton().BaseObjectInfoFromObjectInfo(SObjectInfo(pszClassName, Window));
		if (!baseClassInfo.IsValid()) return SXmlNode();

		return GetDefAttribute(baseClassInfo.mName);
	}
	else
	{
		return GetKeyObject(pszClassName);
	}
}

}//namespace SOUI

