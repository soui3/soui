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

}