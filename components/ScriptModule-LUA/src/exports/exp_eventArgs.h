#include <event/SEvents.h>


EventTimer * toEventTimer(IEvtArgs *pEvt)
{
    return sobj_cast<EventTimer>(pEvt);
}

EventSwndSize * toEventSize(IEvtArgs *pEvt)
{
    return sobj_cast<EventSwndSize>(pEvt);
}

SHostWnd * getHost(IEvtArgs *pEvt)
{
	if(pEvt->IsClass(EventInit::GetClassName()))
	{
		EventInit *e2 = (EventInit*)pEvt;
		return sobj_cast<SHostWnd>(e2->host);
	}
	else if(pEvt->IsClass(EventExit::GetClassName()))
	{
		EventExit *e2 = (EventExit*)pEvt;
		return sobj_cast<SHostWnd>(e2->host);
	}else
	{
		return NULL;
	}
}
BOOL ExpLua_EventArgs(lua_State *L)
{
	try{
		lua_tinker::class_add<IEvtArgs>(L,"EventArgs");
        lua_tinker::class_inh<IEvtArgs,IObject>(L);
		lua_tinker::class_def<IEvtArgs>(L,"sender",&IEvtArgs::Sender);
        lua_tinker::class_def<IEvtArgs>(L,"idFrom",&IEvtArgs::IdFrom);
        lua_tinker::class_def<IEvtArgs>(L,"nameFrom",&IEvtArgs::NameFrom);
        
		lua_tinker::def(L,"getHost",getHost);//类型转换

        lua_tinker::class_add<EventTimer>(L,"EventTimer");
        lua_tinker::class_inh<EventTimer,IEvtArgs>(L);
        lua_tinker::class_mem<EventTimer>(L,"uID",&EventTimer::uID);
        lua_tinker::def(L,"toEventTimer",toEventTimer);//类型转换

        lua_tinker::class_add<EventSwndSize>(L,"EventSwndSize");
        lua_tinker::class_inh<EventSwndSize,IEvtArgs>(L);
        lua_tinker::class_mem<EventSwndSize>(L,"szWnd",&EventSwndSize::szWnd);
        lua_tinker::def(L,"toEventSize",toEventSize);//类型转换

		return TRUE;
	}catch(...)
	{
		return FALSE;
	}
}