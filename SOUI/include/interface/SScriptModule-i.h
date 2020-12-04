/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SScriptModule-i.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    
*/

#pragma once
#include <utilities-def.h>
#include <interface/obj-ref-i.h>
#include <interface/SEvtArgs-i.h>

SNSBEGIN

interface IWindow;
/*!
\brief
    Abstract interface required for all scripting support modules to be used with
    the SOUI system.
*/
#undef INTERFACE
#define INTERFACE IScriptModule
DECLARE_INTERFACE_(IScriptModule,IObjRef)
{
    /**
     * GetScriptEngine
     * @brief    获得脚本引擎的指针
     * @return   void * -- 脚本引擎的指针
     * Describe  
     */    
    STDMETHOD_(void *,GetScriptEngine)(THIS) PURE;

    /*************************************************************************
        Abstract interface
    *************************************************************************/
    /*!
    \brief
        Execute a script file.

    \param pszScriptFile
        String object holding the filename of the script file that is to be executed
        
    */
    STDMETHOD_(void,executeScriptFile)(THIS_ LPCSTR pszScriptFile)  PURE;

    /*!
    \brief
        Execute a script buffer.

    \param buff
        buffer of the script that is to be executed
        
    \param sz
        size of buffer
    */
    STDMETHOD_(void,executeScriptBuffer)(THIS_ LPCSTR buff, size_t sz)  PURE;
    /*!
    \brief
        Execute script code contained in the given String object.

    \param str
        String object holding the valid script code that should be executed.

    \return
        Nothing.
    */
    STDMETHOD_(void,executeString)(THIS_ LPCSTR str) PURE;


    /*!
    \brief
        Execute a scripted global 'event handler' function.  The function should take some kind of IEvtArgs like parameter
        that the concrete implementation of this function can create from the passed IEvtArgs based object.  

    \param handler_name
        String object holding the name of the scripted handler function.

    \param IEvtArgs *pEvt
        IEvtArgs based object that should be passed, by any appropriate means, to the scripted function.

    \return
        - true if the event was handled.
        - false if the event was not handled.
    */
    STDMETHOD_(BOOL,executeScriptedEventHandler)(THIS_ LPCSTR handler_name, IEvtArgs *pEvt) PURE;


    /*!
    \brief
        Return identification string for the ScriptModule.  If the internal id string has not been
        set by the ScriptModule creator, a generic string of "Unknown scripting module" will be returned.

    \return
        String object holding a string that identifies the ScriptModule in use.
    */
    STDMETHOD_(LPCSTR,getIdentifierString)(THIS) SCONST PURE;

    /*!
    \brief
            Subscribes or unsubscribe the named Event to a scripted function

    \param target
            The target EventSet for the subscription.

    \param uEvent
            Event ID to subscribe to.

    \param subscriber_name
            String object containing the name of the script function that is to be subscribed to the Event.

    \return 
    */
    STDMETHOD_(BOOL,subscribeEvent)(THIS_ IWindow* target, UINT uEvent, LPCSTR subscriber_name) PURE;

    /**
     * unsubscribeEvent
     * @brief    取消事件订阅
     * @param    IWindow * target --  目标窗口
     * @param    UINT uEvent --  目标事件
     * @param    LPCSTR subscriber_name --  脚本函数名
     * @return   bool -- true操作成功
     * Describe  
     */    
    STDMETHOD_(BOOL,unsubscribeEvent)(THIS_ IWindow* target, UINT uEvent, LPCSTR subscriber_name ) PURE;

};

#undef INTERFACE
#define INTERFACE IScriptFactory
DECLARE_INTERFACE_(IScriptFactory,IObjRef)
{
    STDMETHOD_(HRESULT,CreateScriptModule)(THIS_ IScriptModule ** ppScriptModule) PURE;
};

SNSEND
