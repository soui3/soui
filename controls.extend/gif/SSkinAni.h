#pragma once

#include <interface/SRender-i.h>
namespace SOUI
{
    class SSkinAni : public SSkinObjBase
    {
        SOUI_CLASS_NAME(SSkinAni,L"skinani")

		class SAniFrame
		{
		public:
			SAutoRefPtr<IBitmap> pBmp;
			int                  nDelay;
		};
    public:
        SSkinAni():m_nFrames(0),m_iFrame(0)
        {

        }

        virtual ~SSkinAni()
        {
        }

		virtual long GetFrameDelay(int iFrame=-1) const = 0;

		virtual int LoadFromFile(LPCTSTR pszFileName) { return 0; }

        /**
         * LoadFromMemory
         * @brief    从内存加载APNG
         * @param    LPVOID pBits --  内存地址
         * @param    size_t szData --  内存数据长度
         * @return   int -- APNG帧数，0-失败
         * Describe  
         */    
        virtual int LoadFromMemory(LPVOID pBits,size_t szData) { return 0; }

		/**
        * ActiveNextFrame
        * @brief    激活下一帧
        * @return   void 
        * Describe  
        */    
        void ActiveNextFrame()
        {
            if(m_nFrames>1)
            {
                m_iFrame++;
                if(m_iFrame==m_nFrames) m_iFrame=0;
                SelectActiveFrame(m_iFrame);
            }
        }

        /**
        * SelectActiveFrame
        * @brief    激活指定帧
        * @param    int iFrame --  帧号
        * @return   void
        * Describe  
        */    
        void SelectActiveFrame(int iFrame) const
        {
            if(m_nFrames>1 && iFrame<m_nFrames)
            {
                m_iFrame = iFrame;
            }
        }

		virtual void _DrawByIndex(IRenderTarget *pRT, LPCRECT rcDraw, int iFrame, BYTE byAlpha/*=0xFF*/) const
		{
			if (iFrame != -1) SelectActiveFrame(iFrame);
			_DrawByIndex2(pRT, rcDraw, m_iFrame, byAlpha);
		}

	protected:
		virtual void _DrawByIndex2(IRenderTarget *pRT, LPCRECT rcDraw, int iFrame, BYTE byAlpha/*=0xFF*/) const = 0;

	protected:
		int m_nFrames;
        mutable int m_iFrame;

    };

}