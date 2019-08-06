/********************************************************************
created:	2014/10/01
created:	01:10:2014   16:32
filename: 	SSkinPNGX.h
file base:	SSkinPNGX
file ext:	h
author:		huangjianxiong
*********************************************************************/
#pragma once
#include <interface/SSkinobj-i.h>
#include <unknown/obj-ref-impl.hpp>

#include "SSkinAni.h"

namespace SOUI
{

    /**
    * @class     SSkinAPNG
    * @brief     APNG图片加载及显示对象
    * 
    * Describe
    */
    class SSkinPNGX : public SSkinAni
    {
        SOUI_CLASS_NAME(SSkinPNGX, L"pngx")
    public:
		SSkinPNGX():m_bVert(FALSE)
        {

        }

        SOUI_ATTRS_BEGIN()
            ATTR_IMAGEAUTOREF(L"src",m_pngx,FALSE)   //XML文件中指定的图片资源名,(type:name)
			ATTR_CUSTOM(L"delay",OnAttrDelay) //delay, format:10[5],20,30 '[5]' is optional repeat count. 
			ATTR_BOOL(L"vert",m_bVert,FALSE)
        SOUI_ATTRS_END()
	protected:
		virtual int GetStates() const  override;

        virtual SIZE GetSkinSize() const override;

        /**
        * GetFrameDelay
        * @brief    获得指定帧的显示时间
        * @param    int iFrame --  帧号,为-1时代表获得当前帧的延时
        * @return   long -- 延时时间(*10ms)
        * Describe  
        */    
        virtual long GetFrameDelay(int iFrame=-1) const;
        
        /**
        * Draw
        * @brief    绘制指定帧的GIF图
        * @param    IRenderTarget * pRT --  绘制目标
        * @param    LPCRECT rcDraw --  绘制范围
        * @param    DWORD dwState --  绘制状态，这里被解释为帧号
        * @param    BYTE byAlpha --  透明度
        * @return   void
        * Describe  
        */    
        virtual void _DrawByIndex2(IRenderTarget *pRT, LPCRECT rcDraw, int dwState,BYTE byAlpha=0xFF) const override;
    protected:
		HRESULT OnAttrDelay(const SStringW &strValue,BOOL bLoading);
 
		CAutoRefPtr<IBitmap> m_pngx;
		SArray<int>			 m_nDelays;
		BOOL				 m_bVert;
    };
}//end of name space SOUI
