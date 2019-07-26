#pragma once

namespace SOUI
{

    /**
    * @struct     ITimelineHandler
    * @brief      时间轴处理接口
    * 
    * Describe    
    */
    struct ITimelineHandler
    {
        virtual void OnNextFrame()=0;
    };


	struct ITimelineHandlersMgr
	{
		virtual BOOL RegisterTimelineHandler(ITimelineHandler *pHandler) = 0;

		virtual BOOL UnregisterTimelineHandler(ITimelineHandler *pHandler) = 0;
	};
}