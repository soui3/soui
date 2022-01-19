#include <core/SHostWnd.h>

SHostWnd * toHostWnd(IObject *pObj)
{
	if(pObj->IsClass(SHostWnd::GetClassName()))
		return (SHostWnd*)(SNativeWnd*)pObj;
	else
		return NULL;

    //return sobj_cast<SHostWnd>(pObj);
}

SHostDialog * toHostDialog(IObject *pObj)
{
	if(pObj->IsClass(SHostDialog::GetClassName()))
		return (SHostDialog*)(SHostWnd*)(SNativeWnd*)pObj;
	else
		return NULL;

	//return sobj_cast<SHostWnd>(pObj);
}


SRootWindow * toRoot(IObject *pObj)
{
	return sobj_cast<SRootWindow>(pObj);
}

BOOL ExpLua_HostWnd(lua_State *L)
{
	try{
		lua_tinker::class_add<SHostWnd>(L,"SHostWnd");
		lua_tinker::class_def<SHostWnd>(L,"AnimateHostWindow",&SHostWnd::AnimateHostWindow);
		lua_tinker::class_def<SHostWnd>(L,"setTimeout",&SHostWnd::setTimeout);
		lua_tinker::class_def<SHostWnd>(L,"setInterval",&SHostWnd::setInterval);
		lua_tinker::class_def<SHostWnd>(L,"clearTimer",&SHostWnd::clearTimer);
        lua_tinker::class_def<SHostWnd>(L,"GetRoot",&SHostWnd::GetRoot);
        
        lua_tinker::def(L,"toHostWnd",toHostWnd);

		lua_tinker::class_add<SRootWindow>(L,"SRoot");
		lua_tinker::class_inh<SRootWindow,SWindow>(L);
		lua_tinker::class_def<SRootWindow>(L,"GetHostWnd",&SRootWindow::GetHostWnd);
		lua_tinker::def(L,"toRoot",toRoot);

		return TRUE;
	}catch(...)
	{
		return FALSE;
	}
}