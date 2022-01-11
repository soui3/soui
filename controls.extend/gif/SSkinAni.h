#pragma once

#include <interface/SRender-i.h>
namespace SOUI
{
    class SSkinAni : public SSkinObjBase
    {
        SOUI_CLASS_NAME(SSkinObjBase,L"skinani")

		class SAniFrame
		{
		public:
			SAutoRefPtr<IBitmap> pBmp;
			int                  nDelay;
		};
    public:
        SSkinAni():m_nFrames(0),m_iFrame(0),m_bTile(FALSE),m_filterLevel(kLow_FilterLevel)
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

		virtual void _Scale(ISkinObj *pObj, int nScale)
		{
			SSkinObjBase::_Scale(pObj,nScale);
			SSkinAni *pClone = sobj_cast<SSkinAni>(pObj);
			pClone->m_nFrames = m_nFrames;
			pClone->m_iFrame = m_iFrame;
			pClone->m_filterLevel = m_filterLevel;
			pClone->m_bTile = m_bTile;
			pClone->m_rcMargin.left = MulDiv(m_rcMargin.left,nScale,100);
			pClone->m_rcMargin.top = MulDiv(m_rcMargin.top,nScale,100);
			pClone->m_rcMargin.right = MulDiv(m_rcMargin.right,nScale,100);
			pClone->m_rcMargin.bottom = MulDiv(m_rcMargin.bottom,nScale,100);
		}

		LONG GetExpandCode() const{
			return MAKELONG(m_bTile?EM_TILE:EM_STRETCH,m_filterLevel);
		}
	public:
		SOUI_ATTRS_BEGIN()
			ATTR_RECT(L"margin",m_rcMargin,TRUE)
			ATTR_MARGIN(L"margin2",m_rcMargin,TRUE)
			ATTR_BOOL(L"tile",m_bTile,TRUE)
			ATTR_ENUM_BEGIN(L"filterLevel",FilterLevel,TRUE)
				ATTR_ENUM_VALUE(L"none",kNone_FilterLevel)
				ATTR_ENUM_VALUE(L"low",kLow_FilterLevel)
				ATTR_ENUM_VALUE(L"medium",kMedium_FilterLevel)
				ATTR_ENUM_VALUE(L"high",kHigh_FilterLevel)
			ATTR_ENUM_END(m_filterLevel)
		SOUI_ATTRS_END()
	protected:
		virtual void _DrawByIndex2(IRenderTarget *pRT, LPCRECT rcDraw, int iFrame, BYTE byAlpha/*=0xFF*/) const = 0;

	protected:
		int m_nFrames;
        mutable int m_iFrame;
		CRect		m_rcMargin;
		FilterLevel	m_filterLevel;
		BOOL		m_bTile;
    };

}