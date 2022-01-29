#pragma once
#include "SByteArray.h"
#include "core/SPanel.h"

namespace SOUI
{
	class SHexEditAction;

#define EVT_HE_DATACHANGED (EVT_EXTERNAL_BEGIN+300)
	#define EVT_HE_SELCHANGED (EVT_EXTERNAL_BEGIN+301)

	class EventHexEditDataChanged : public TplEventArgs<EventHexEditDataChanged>
	{
		SOUI_CLASS_NAME(EventHexEditDataChanged, L"on_hexedit_data_changed")
	public:
		enum { EventID = EVT_HE_DATACHANGED };
		EventHexEditDataChanged(SObject* pSender) :TplEventArgs<EventHexEditDataChanged>(pSender) {}
	};

	class EventHexEditSelChanged : public TplEventArgs<EventHexEditSelChanged>
	{
		SOUI_CLASS_NAME(EventHexEditSelChanged, L"on_hexedit_sel_changed")
	public:
		enum { EventID = EVT_HE_SELCHANGED };
		UINT m_nSelBegin;
		UINT m_nSelEnd;
		EventHexEditSelChanged(SObject* pSender) :TplEventArgs<EventHexEditSelChanged>(pSender) {}
	};

	class SHexEdit : public SPanel
	{
		SOUI_CLASS_NAME(SListCtrl, L"hexedit")

		SHexEdit();
		virtual           ~SHexEdit();

		void              SetData(const SByteArray& data);
		void              SetData(const BYTE* data, UINT len);
		const SByteArray& GetData() const { return m_xData; }

		bool IsSelection() const;
		bool IsInsert() const;
		bool GetSelection(UINT& nBegin, UINT& nEnd) const;
		void SetBytesPerRow(UINT nBytesPerRow, bool bAuto, bool bUpdate = false);
		void SetShowAddress(bool bShow, bool bUpdate = false);
		void SetShowAscii(bool bShow, bool bUpdate = false);
		void SetSelection(UINT nBegin, UINT nEnd, bool bMakeVisible, bool bUpdate = false);
		void SetReadonly(bool bReadOnly, bool bUpdate = false);
		void SetAddressSize(BYTE nAdrSize, bool bUpdate = false);
		void SetAdrCol(COLORREF tAdrBkgCol, COLORREF tAdrTxtCol, bool bUpdate = false);
		void SetAsciiCol(COLORREF tAsciiBkgCol, COLORREF tAsciiTxtCol, bool bUpdate = false);
		void SetHexCol(COLORREF tHexBkgCol, COLORREF tHexTxtCol, bool bUpdate = false);
		void SetSelectedNoFocusCol(COLORREF tSelectedNoFocusBkgCol, COLORREF tSelectedNoFocusTxtCol, bool bUpdate = false);
		void ReInitialize();
		UINT GetDataSize() const { return m_xData.Size(); }

		void Undo();
		void Redo();
		BOOL CanUndo() { return m_undo != 0; }
		BOOL CanRedo() { return m_redo != 0; }

	protected:
		/**
		 * SHexEdit::OnGetDlgCode
		 * @brief    获取窗口消息码
		 *
		 * Describe  消息响应函数
		 */
		virtual UINT OnGetDlgCode()
		{
			return (SC_WANTALLKEYS | SC_WANTTAB);
		}

		/**
		* SHexEdit::OnPaint
		* @brief    绘制
		* @param    IRenderTarget *pRT -- 绘制设备
		*
		* Describe  消息响应函数
		*/
		void OnPaint(IRenderTarget* pRT);
		void SetSelectedFocusCol(COLORREF tSelectedFousTxtCol, COLORREF tSelectedFousBkgCol, bool bUpdate);
		/**
		 * SHexEdit::OnSetFocus
		 * @brief    设置焦点
		 *
		 * Describe  此函数是消息响应函数
		 */
		void OnSetFocus(SWND wndOld);

		/**
		 * SHexEdit::OnKillFocus
		 * @brief    失去焦点
		 *
		 * Describe  此函数是消息响应函数
		 */
		void OnKillFocus(SWND wndFocus);

		void OnDestroy();
		void OnSize(UINT nType, CSize size);


		void SetScrollbarRanges();
		void CalculatePaintingDetails(IRenderTarget* pRT);
		void PaintAddresses(IRenderTarget* pRT);
		void PaintHexData(IRenderTarget* pRT);
		void PaintAsciiData(IRenderTarget* pRT);
		void SetEditCaretPos(UINT nOffset, bool bHighBits);
		void CreateEditCaret(UINT nCaretHeight, UINT nCaretWidth);
		void DestoyEditCaret();
		void OnTimer(UINT nTimerID);
		void StartMouseRepeat(const CPoint& cPoint, int iDelta, WORD nSpeed);
		void StopMouseRepeat();
		void SetScrollPositionY(UINT nPosition, bool bUpdate = false);
		void SetScrollPositionX(UINT nPosition, bool bUpdate = false);
		void MoveScrollPostionY(int iDelta, bool bUpdate = false);
		void MoveScrollPostionX(int iDelta, bool bUpdate = false);
		void MakeVisible(UINT nBegin, UINT nEnd, bool bUpdate = false);
		void MoveCurrentAddress(int iDeltaAdr, bool bHighBits, bool bIgnoreShift = false);
		void GetAddressFromPoint(const CPoint& cPt, UINT& nAddress, bool& bHighBits);
		BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

		void         OnMouseMove(UINT nFlags, CPoint pt);
		void         OnLButtonUp(UINT nFlags, CPoint pt);
		void         OnLButtonDown(UINT nFlags, CPoint pt);
		void         OnLButtonDbClick(UINT nFlags, CPoint pt);
		std::string  GetClipboardText();
		int          SetClipboard(LPCSTR lpszBuffer);
		void         OnEditCopy();
		void         OnEditPaste();
		void         OnEditCut();
		bool         PrepareReplace(UINT pos, const SByteArray& oldData, const SByteArray& newData);
		bool         PrepareReplace(UINT pos, UINT oldlen, UINT newlen);
		void         OnEditClear();
		void         OnEditSelectAll();
		void         OnDelete(WPARAM wParam);
		bool         OnEditInput(WORD nInput);
		BOOL         PressKeyDown(UINT nChar);
		void         OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
		void         OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		void         OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		virtual BOOL OnScroll(BOOL bVertical, UINT uCode, int nPos);
		void         OnVScroll(UINT nSBCode, UINT nPos, HWND);
		void         OnHScroll(UINT nSBCode, UINT nPos, HWND);

		BOOL		 SaveUndoAction(UINT type, UINT position, const SByteArray& replaceData, const SByteArray& insertData);

	protected:
		SByteArray m_xData;

		struct PAINTINGDETAILS {
			UINT nFullVisibleLines;
			UINT nLastLineHeight;
			UINT nVisibleLines;
			UINT nLineHeight;
			UINT nCharacterWidth;
			UINT nBytesPerRow;
			UINT nHexPos;
			UINT nHexLen;
			UINT nAsciiPos;
			UINT nAsciiLen;
			UINT nAddressPos;
			UINT nAddressLen;
			CRect cPaintingRect;
		};

		PAINTINGDETAILS m_tPaintDetails;

		COLORREF m_tAdrBkgCol;
		COLORREF m_tAdrTxtCol;
		COLORREF m_tAsciiBkgCol;
		COLORREF m_tAsciiTxtCol;
		COLORREF m_tHexBkgCol;
		COLORREF m_tHexTxtCol;
		COLORREF m_tSelectedNoFocusTxtCol;
		COLORREF m_tSelectedNoFocusBkgCol;
		COLORREF m_tSelectedFousTxtCol;
		COLORREF m_tSelectedFousBkgCol;

		UINT  m_nBytesPerRow;
		UINT  m_nAdrSize;
		bool  m_bAutoBytesPerRow;
		DWORD m_dwZeroAddr;
		UINT m_nHighlightedEnd;
		UINT m_nHighlightedBegin;
		UINT m_nSelectionBegin;
		UINT m_nSelectionEnd;
		UINT m_nCurrentAddress;
		CRect m_cDragRect;
		UINT m_nCurCaretHeight;
		UINT m_nCurCaretWidth;
		int m_CaretX, m_CaretY;                 // graphics position of the cursor

		bool m_bHasCaret;
		bool m_bRecalc;
		bool m_bHighBits; // caret on high nibble
		bool m_bCaretAscii;	// true: caret in ASCII area; false: caret in hex area

		CPoint m_cMouseRepPoint;
		int m_iMouseRepDelta;
		WORD m_nMouseRepSpeed;
		WORD m_nMouseRepCounter;
		bool m_bIsMouseRepActive;

		bool m_overwriteMode;
		bool m_bShowAddress;
		bool m_bShowAscii;
		bool m_bReadOnly;

		UINT m_nScrollPostionY;
		UINT m_nScrollRangeY;
		UINT m_nScrollPostionX;
		UINT m_nScrollRangeX;
		UINT m_nSelectingBeg;
		UINT m_nSelectingEnd;

		SOUI::IFontPtr m_pFont;
		SHexEditAction* m_undo;
		SHexEditAction* m_redo;
	protected:

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_DESTROY(OnDestroy)
			MSG_WM_SIZE(OnSize)
			MSG_WM_SETFOCUS_EX(OnSetFocus)
			MSG_WM_KILLFOCUS_EX(OnKillFocus)
			MSG_WM_MOUSEWHEEL(OnMouseWheel)
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONUP(OnLButtonUp)
			MSG_WM_LBUTTONDBLCLK(OnLButtonDbClick)
			MSG_WM_CHAR(OnChar)
			MSG_WM_KEYDOWN(OnKeyDown)
			MSG_WM_SYSKEYDOWN(OnSysKeyDown)
			MSG_WM_TIMER2(OnTimer)
			SOUI_MSG_MAP_END()

	protected:
		HRESULT OnAttrAddrBkColor(const SStringW& strValue, BOOL bLoading);
		HRESULT OnAttrHexBkColor(const SStringW& strValue, BOOL bLoading);
		HRESULT OnAttrAsciiBkColor(const SStringW& strValue, BOOL bLoading);

	public:
		SOUI_ATTRS_BEGIN()
			ATTR_INT(L"AddrSize", m_nAdrSize, 8)
			ATTR_INT(L"BytesPerRow", m_bReadOnly, 16)
			ATTR_BOOL(L"AutoBytesPerRow", m_bAutoBytesPerRow, FALSE)
			ATTR_BOOL(L"readOnly", m_bReadOnly, FALSE)
			ATTR_CUSTOM(L"AddrBkColor", OnAttrAddrBkColor)
			ATTR_CUSTOM(L"HexBkColor", OnAttrHexBkColor)
			ATTR_CUSTOM(L"AsciiBkColor", OnAttrAsciiBkColor)
			SOUI_ATTRS_END()

	};

}
