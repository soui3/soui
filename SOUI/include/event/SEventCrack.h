/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       eventcrack.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/09/05
* 
* Describe    SOUI系统中使用的事件映射宏
*/


#define EVENT_MAP_BEGIN()                           \
public:                                          \
    virtual BOOL _HandleEvent(SOUI::EventArgs *pEvt)\
    {                                               \
        UINT      uCode = pEvt->GetID();            \
		UINT	  nCount = 0;                       \


#define EVENT_MAP_DECLEAR()                         \
protected:                                          \
    virtual BOOL _HandleEvent(SOUI::EventArgs *pEvt);\
    

#define EVENT_MAP_BEGIN2(classname)                 \
    BOOL classname::_HandleEvent(SOUI::EventArgs *pEvt)\
    {                                               \
        UINT      uCode = pEvt->GetID();            \
		UINT	  nCount = 0;                       \


#define EVENT_MAP_END()                             \
	    if(pEvt->bubbleUp)                          \
		{                                           \
		     BOOL bRet = __super::_HandleEvent(pEvt);\
			 nCount += bRet?1:0;                     \
		}                                           \
		return nCount>0;                            \
    }                                               \

#define EVENT_MAP_BREAK()                           \
        return nCount>0;                            \
    }                                               \
 
#define CHAIN_EVENT_MAP(ChainClass)                 \
    if(pEvt->bubbleUp))                             \
	{\
		BOOL bRet = ChainClass::_HandleEvent(pEvt);  \
		nCount += bRet?1:0;                          \
		if(bRet && !pEvt->bubbleUp)                  \
		    return TRUE;                             \
	}
 
#define CHAIN_EVENT_MAP_MEMBER(theChainMember)      \
	{\
		BOOL bRet = (theChainMember)._HandleEvent(pEvt);  \
		nCount += bRet?1:0;                          \
		if(bRet && !pEvt->bubbleUp)                  \
		    return TRUE;                             \
	}

#define EVENT_CHECK_SENDER_ROOT(pRoot)              \
    {                                               \
		if(!pRoot) return FALSE;\
		SOUI::SWindow *pWnd = SOUI::sobj_cast<SOUI::SWindow>(pEvt->sender);\
		if(pWnd && pRoot != pWnd && !pWnd->IsDescendant(pRoot)) \
			return FALSE;                               \
    }

// void OnEvent(EventArgs *pEvt)
#define EVENT_HANDLER(cd, func)                     \
    if(cd == uCode)                                 \
    {                                               \
	    nCount++;                                   \
        pEvt->bubbleUp = false; func(pEvt);			\
		if(!pEvt->bubbleUp) return TRUE;            \
    } 


// void OnEvent(EventArgs *pEvt)
#define EVENT_ID_HANDLER(id, cd, func)              \
    if(cd == uCode && id == pEvt->idFrom)           \
    {                                               \
	    nCount++;                                   \
        pEvt->bubbleUp = false; func(pEvt);         \
		if(!pEvt->bubbleUp) return TRUE;            \
    }

// void OnEvent(EventArgs *pEvt)
#define EVENT_ID_RANGE_HANDLER(idMin, idMax , cd, func) \
    if(cd == uCode  && idMin <= pEvt->idFrom && idMax >= pEvt->idFrom )\
    {                                               \
	    nCount++;                                   \
        pEvt->bubbleUp = false; func(pEvt);         \
		if(!pEvt->bubbleUp) return TRUE;            \
    }


// void OnEvent(EventArgs *pEvt)
#define EVENT_NAME_HANDLER(name, cd, func)          \
    if(cd == uCode && pEvt->nameFrom!= NULL && wcscmp(pEvt->nameFrom,name)==0) \
    {                                               \
	    nCount++;                                   \
        pEvt->bubbleUp = false; func(pEvt);         \
		if(!pEvt->bubbleUp) return TRUE;            \
    }


// void OnCommand(EventArgs *pEvt)
#define EVENT_COMMAND(func)                         \
    if (SOUI::EVT_CMD == uCode)                     \
    {                                               \
	    nCount++;                                   \
        pEvt->bubbleUp = false; func(pEvt);         \
		if(!pEvt->bubbleUp) return TRUE;            \
    }                                               \

// void OnCommand()
#define EVENT_ID_COMMAND(id, func)                   \
    if (SOUI::EVT_CMD == uCode && id == pEvt->idFrom)\
    {                                                \
	    nCount++;                                   \
        pEvt->bubbleUp = false; func();              \
		if(!pEvt->bubbleUp) return TRUE;             \
    }                                                \
 
// void OnCommand(int nID)
#define EVENT_ID_COMMAND_RANGE(idMin, idMax, func)   \
    if (SOUI::EVT_CMD == uCode && idMin <= pEvt->idFrom && idMax >= pEvt->idFrom )  \
    {                                                \
	    nCount++;                                   \
        pEvt->bubbleUp = false; func(pEvt->idFrom);  \
		if(!pEvt->bubbleUp) return TRUE;             \
    }                                                \

// void OnCommand()
#define EVENT_NAME_COMMAND(name, func)               \
    if (SOUI::EVT_CMD == uCode && pEvt->nameFrom!= NULL && wcscmp(pEvt->nameFrom,name)==0)  \
    {                                                \
	    nCount++;                                   \
        pEvt->bubbleUp = false; func();              \
		if(!pEvt->bubbleUp) return TRUE;             \
    }                                                \

 
// BOOL OnContextMenu(CPoint pt)
#define EVENT_ID_CONTEXTMENU(id,func)                                      \
    if (SOUI::EVT_CTXMENU == uCode && pEvt->idFrom==id)      \
	{                                                                          \
	    nCount++;                                                              \
		pEvt->bubbleUp = false;                                                \
		SOUI::EventCtxMenu* pEvtCtxMenu = (SOUI::EventCtxMenu*)pEvt;           \
		pEvtCtxMenu->bCancel=func(pEvtCtxMenu->pt);                            \
		if(!pEvt->bubbleUp) return TRUE;                                       \
	}                                                                          \


// BOOL OnContextMenu(CPoint pt)
#define EVENT_NAME_CONTEXTMENU(name,func)                                       \
    if (SOUI::EVT_CTXMENU == uCode && pEvt->nameFrom!= NULL && wcscmp(pEvt->nameFrom,name)==0) \
	{                                                                               \
	    nCount++;                                                                   \
		pEvt->bubbleUp = false;                                                     \
		SOUI::EventCtxMenu* pEvtCtxMenu = (SOUI::EventCtxMenu*)pEvt;                \
		pEvtCtxMenu->bCancel=func(pEvtCtxMenu->pt);                                 \
		if(!pEvt->bubbleUp) return TRUE;                                            \
	}                                                                               \

