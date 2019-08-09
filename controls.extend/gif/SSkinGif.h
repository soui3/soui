/********************************************************************
created:	2012/12/27
created:	27:12:2012   14:55
filename: 	SSkinGif.h
file base:	SSkinGif
file ext:	h
author:		huangjianxiong

purpose:	自定义皮肤对象
*********************************************************************/
#pragma once
#include <interface/SSkinobj-i.h>
#include <unknown/obj-ref-impl.hpp>

#include "SSkinAni.h"

namespace Gdiplus
{
class Bitmap;
}

namespace SOUI
{

    /**
    * @class     SSkinGif
    * @brief     GIF图片加载及显示对象
    * 
    * Describe
    */
    class SSkinGif : public SSkinAni
    {
        SOUI_CLASS_NAME(SSkinGif, L"gif")
    public:
        SSkinGif():m_pFrames(NULL)
        {
        }

		~SSkinGif()
		{
			if (m_pFrames) delete[]m_pFrames;
		}

		/**
		* GetFrameDelay
		* @brief    获得指定帧的显示时间
		* @param    int iFrame --  帧号,为-1时代表获得当前帧的延时
		* @return   long -- 延时时间(*10ms)
		* Describe
		*/
		long GetFrameDelay(int iFrame = -1) const;

        //初始化GDI+环境，由于这里需要使用GDI+来解码GIF文件格式
        static BOOL Gdiplus_Startup();
        //退出GDI+环境
        static void Gdiplus_Shutdown();

        /**
         * LoadFromFile
         * @brief    从文件加载GIF
         * @param    LPCTSTR pszFileName --  文件名
         * @return   int -- GIF帧数，0-失败
         * Describe  
         */    
        int LoadFromFile(LPCTSTR pszFileName);

        /**
         * LoadFromMemory
         * @brief    从内存加载GIF
         * @param    LPVOID pBits --  内存地址
         * @param    size_t szData --  内存数据长度
         * @return   int -- GIF帧数，0-失败
         * Describe  
         */    
        int LoadFromMemory(LPVOID pBits,size_t szData);

        SOUI_ATTRS_BEGIN()
            ATTR_CUSTOM(L"src",OnAttrSrc)   //XML文件中指定的图片资源名,(type:name)
        SOUI_ATTRS_END()
    protected:
		virtual void _DrawByIndex2(IRenderTarget *pRT, LPCRECT rcDraw, int dwState, BYTE byAlpha = 0xFF) const  override;

        HRESULT OnAttrSrc(const SStringW &strValue,BOOL bLoading);
        int LoadFromGdipImage(Gdiplus::Bitmap * pImg);

		SAniFrame * m_pFrames;
    };
}//end of name space SOUI
