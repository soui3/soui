/*
 * 2022.1.29  by 小新
 */

#include "stdafx.h"
#include "SHexEdit.h"


namespace SOUI
{


	class SHexEditAction {
	public:
		enum ActionType { unknown, input, cut, paste } type;
	protected:
		UINT  position;
		SByteArray replaceData;
		SByteArray insertData;
		SHexEditAction* next;
	public:
		SHexEditAction();
		~SHexEditAction();
		BOOL set(ActionType type, UINT position, const SByteArray& replaceData, const SByteArray& insertData, SHexEditAction* next = 0);
		void setNext(SHexEditAction* next) { this->next = next; }
		BOOL append(const SByteArray& replaceData, const SByteArray& insertData);
		ActionType getType() const { return type; }
		UINT getPosition() const { return position; }
		const SByteArray& getReplaceData() const { return replaceData; }
		UINT getReplaceLen() const { return replaceData.Size(); }
		const SByteArray& getInsertData() const { return insertData; }
		UINT getInsertLen() const { return insertData.Size(); }
		SHexEditAction* getNext() const { return next; }
	};

	SHexEditAction::SHexEditAction()
		: type(unknown), position(0), next(0)
	{
	}

	SHexEditAction::~SHexEditAction()
	{
		delete next;
	}

	BOOL SHexEditAction::set(ActionType type, UINT position, const SByteArray& replaceData, const SByteArray& insertData, SHexEditAction* next)
	{
		this->type = type;
		this->position = position;
		this->replaceData = replaceData;
		this->insertData = insertData;
		this->next = next;
		return TRUE;
	}

	BOOL SHexEditAction::append(const SByteArray& replaceData, const SByteArray& insertData)
	{
		this->replaceData.Append(replaceData);
		this->insertData.Append(insertData);
		return TRUE;
	}


	//=============================================================================


	#define NOSECTION_VAL	    0xffffffff

	// data length that can be handled
	#define MAXHEXEDITLENGTH	0x7ffff000

	// increase capacity by 1 + m_nCapacity/CAPACICTYINCDIVISOR when a character is appended/inserted
	#define CAPACICTYINCDIVISOR 8

	// control-layout customization (low-level)
	#define ADR_DATA_SPACE				15
	#define DATA_ASCII_SPACE			15
	#define CONTROL_BORDER_SPACEH		0
	#define CONTROL_BORDER_SPACEV		0

	// boundaries and special values
	#define MOUSEREP_TIMER_TID			0x400
	#define MOUSEREP_TIMER_ELAPSE		0x5

	// macros
	#define NORMALIZE_SELECTION(beg, end) if(beg>end){UINT tmp = end; end=beg; beg=tmp; }


	const TCHAR tabHexCharacters[16] = {
		'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };

	SHexEdit::SHexEdit() :
		m_nAdrSize(8),
		m_nBytesPerRow(16),
		m_bHasCaret(false),
		m_bHighBits(true),
		m_bCaretAscii(false),
		m_bReadOnly(false),
		m_overwriteMode(false),
		m_nHighlightedBegin(NOSECTION_VAL),
		m_nHighlightedEnd(NOSECTION_VAL),
		m_nSelectionBegin(NOSECTION_VAL),
		m_nSelectionEnd(NOSECTION_VAL),
		m_nCurrentAddress(0),
		m_bAutoBytesPerRow(false),
		m_bRecalc(true),
		m_nScrollPostionX(0),
		m_nScrollRangeX(0),
		m_nScrollPostionY(0),
		m_nScrollRangeY(0),
		m_bShowAscii(true),
		m_bShowAddress(true),
		m_nMouseRepSpeed(0),
		m_iMouseRepDelta(0),
		m_nMouseRepCounter(0),
		m_bIsMouseRepActive(false),
		m_cDragRect(0, 0, 0, 0),
		m_undo(0),
		m_redo(0),
		m_dwZeroAddr(0)
	{
		m_bFocusable = TRUE;
		m_tPaintDetails.nCharacterWidth = 0;
		m_bCaretAtLineEnd = false;
		
		m_tAdrTxtCol = RGBA(0x00, 0x00, 0xBF, 0xFF);
		m_tAdrBkgCol = RGBA(0xFF, 0xF8, 0xF0, 0xFF);
		m_tHexBkgCol = m_tAdrBkgCol;
		m_tAsciiBkgCol = m_tAdrBkgCol;
		m_tHexTxtCol = RGBA(0x00, 0x00, 0x00, 0xFF);
		m_tAsciiTxtCol = RGBA(0x00, 0x00, 0x00, 0xFF);

		m_tSelectedNoFocusTxtCol = RGBA(114, 114, 114, 255);
		m_tSelectedNoFocusBkgCol = RGBA(194, 222, 244, 255);

		m_tSelectedFousTxtCol = GetSysColor(COLOR_HIGHLIGHTTEXT) | 0xff000000;
		m_tSelectedFousBkgCol = GetSysColor(COLOR_HIGHLIGHT) | 0xff000000;

		m_pFont = SFontPool::getSingleton().GetFont(L"face:consolas", 1);

		GetEventSet()->addEvent(EVENTID(EventHexEditDataChanged));
		GetEventSet()->addEvent(EVENTID(EventHexEditSelChanged));
	}

	SHexEdit::~SHexEdit()
	{
		delete m_undo;
		delete m_redo;
	}

	void SHexEdit::OnDestroy()
	{
		__super::OnDestroy();
	}

	void SHexEdit::OnSize(UINT nType, CSize size)
	{
		__super::OnSize(nType, size);
		m_bRecalc = true;
		SetScrollbarRanges();
	}

	void SHexEdit::ReInitialize()
	{
		m_nSelectingBeg = NOSECTION_VAL;
		m_nSelectingEnd = NOSECTION_VAL;
		m_nSelectionBegin = NOSECTION_VAL;
		m_nSelectionEnd = NOSECTION_VAL;
		m_nHighlightedBegin = NOSECTION_VAL;
		m_nHighlightedEnd = NOSECTION_VAL;
		m_xData.Clear();
		delete m_undo;
		delete m_redo;
		m_undo = 0;
		m_redo = 0;
		m_bRecalc = true;
	}

	void SHexEdit::SetData(const SByteArray& data)
	{
		ReInitialize();
		m_xData = data;
		m_bRecalc = true;

		SetEditCaretPos(0, true);

		EventHexEditDataChanged evt(this);
		FireEvent(evt);
	}

	void SHexEdit::SetData(const BYTE* data, UINT len)
	{
		ReInitialize();
		if (data && len > 0)
			m_xData.Append(data, len);
		m_bRecalc = true;

		SetEditCaretPos(0, true);

		EventHexEditDataChanged evt(this);
		FireEvent(evt);
	}

	bool SHexEdit::IsSelection() const
	{
		return (m_nSelectionEnd != NOSECTION_VAL) && (m_nSelectionBegin != NOSECTION_VAL);
	}

	bool SHexEdit::IsInsert() const
	{
		return !m_overwriteMode;
	}

	bool SHexEdit::GetSelection(UINT& nBegin, UINT& nEnd) const
	{
		if (IsSelection()) {
			nBegin = m_nSelectionBegin;
			nEnd = m_nSelectionEnd;
			return true;
		}
		nBegin = NOSECTION_VAL;
		nEnd = NOSECTION_VAL;
		return false;
	}

	void SHexEdit::SetBytesPerRow(UINT nBytesPerRow, bool bAuto, bool bUpdate)
	{
		if ((m_bAutoBytesPerRow != bAuto) || (m_nBytesPerRow != nBytesPerRow)) {
			m_bAutoBytesPerRow = bAuto;
			m_nBytesPerRow = nBytesPerRow;
			m_bRecalc = true;

			SetEditCaretPos(m_nCurrentAddress, m_bHighBits);
			if (bUpdate) {
				Invalidate();
			}
		}
	}

	void SHexEdit::SetShowAddress(bool bShow, bool bUpdate)
	{
		if (m_bShowAddress != bShow) {
			m_bShowAddress = bShow;
			m_bRecalc = true;

			SetEditCaretPos(m_nCurrentAddress, m_bHighBits);
			if (bUpdate) {
				Invalidate();
			}
		}
	}

	void SHexEdit::SetShowAscii(bool bShow, bool bUpdate)
	{
		if (m_bShowAscii != bShow) {
			m_bShowAscii = bShow;
			m_bRecalc = true;

			SetEditCaretPos(m_nCurrentAddress, m_bHighBits);
			if (bUpdate) {
				Invalidate();
			}
		}
	}

	void SHexEdit::SetSelection(UINT nBegin, UINT nEnd, bool bMakeVisible, bool bUpdate)
	{
		if ((m_nSelectionEnd != nEnd) || (m_nSelectionBegin != nBegin)) {
			if ((nEnd >= GetDataSize()) && (nEnd != NOSECTION_VAL)) {
				nEnd = GetDataSize() - 1;
			}
			if ((nBegin >= GetDataSize()) && (nBegin != NOSECTION_VAL)) {
				nBegin = GetDataSize() - 1;
			}
			m_nSelectionEnd = nEnd;
			m_nSelectionBegin = nBegin;
			if (bMakeVisible && nEnd != NOSECTION_VAL && nBegin != NOSECTION_VAL) {
				MakeVisible(m_nSelectionBegin, m_nSelectionEnd, false);
			}
			if (bUpdate) {
				Invalidate();
			}
		}
	}

	void SHexEdit::SetReadonly(bool bReadOnly, bool bUpdate)
	{
		m_bReadOnly = bReadOnly;
		if (bUpdate) {
			Invalidate();
		}
	}

	void SHexEdit::SetAddressSize(BYTE nAdrSize, bool bUpdate)
	{
		m_nAdrSize = nAdrSize;
		m_bRecalc = true;

		SetEditCaretPos(m_nCurrentAddress, m_bHighBits);
		if (bUpdate) {
			Invalidate();
		}
	}

	void SHexEdit::SetAdrCol(COLORREF tAdrBkgCol, COLORREF tAdrTxtCol, bool bUpdate)
	{
		m_tAdrBkgCol = tAdrBkgCol;
		m_tAdrTxtCol = tAdrTxtCol;
		if (bUpdate) {
			Invalidate();
		}
	}

	void SHexEdit::SetAsciiCol(COLORREF tAsciiBkgCol, COLORREF tAsciiTxtCol, bool bUpdate)
	{
		m_tAsciiBkgCol = tAsciiBkgCol;
		m_tAsciiTxtCol = tAsciiTxtCol;
		if (bUpdate) {
			Invalidate();
		}
	}

	void SHexEdit::SetHexCol(COLORREF tHexBkgCol, COLORREF tHexTxtCol, bool bUpdate)
	{
		m_tHexBkgCol = tHexBkgCol;
		m_tHexTxtCol = tHexTxtCol;
		if (bUpdate) {
			Invalidate();
		}
	}

	void SHexEdit::SetSelectedNoFocusCol(COLORREF tSelectedNoFocusBkgCol, COLORREF tSelectedNoFocusTxtCol, bool bUpdate)
	{
		m_tSelectedNoFocusBkgCol = tSelectedNoFocusBkgCol;
		m_tSelectedNoFocusTxtCol = tSelectedNoFocusTxtCol;
		if (bUpdate) {
			Invalidate();
		}
	}

	void SHexEdit::SetSelectedFocusCol(COLORREF tSelectedFousTxtCol, COLORREF tSelectedFousBkgCol, bool bUpdate)
	{
		m_tSelectedFousTxtCol = tSelectedFousTxtCol;
		m_tSelectedFousBkgCol = tSelectedFousBkgCol;
		if (bUpdate) {
			Invalidate();
		}
	}

	void SHexEdit::OnSetFocus(SWND wndOld)
	{
		SetEditCaretPos(m_nCurrentAddress, m_bHighBits);
		__super::OnSetFocus(wndOld);
	}

	void SHexEdit::OnKillFocus(SWND wndFocus)
	{
		DestoyEditCaret();
		__super::OnKillFocus(wndFocus);
	}

	void SHexEdit::OnPaint(IRenderTarget* pRT)
	{
		SPainter painter;
		BeforePaint(pRT, painter);

		pRT->SelectObject(m_pFont);
		if (m_bRecalc)
			CalculatePaintingDetails(pRT);

		pRT->FillSolidRect(m_tPaintDetails.cPaintingRect, m_tAdrBkgCol);

		if (m_bShowAddress)
			PaintAddresses(pRT);
		PaintHexData(pRT);
		if (m_bShowAscii)
			PaintAsciiData(pRT);

		AfterPaint(pRT, painter);
	}


	void SHexEdit::SetScrollbarRanges()
	{
		SCROLLINFO tScrollInfo;
		memset(&tScrollInfo, 0, sizeof(SCROLLINFO));
		tScrollInfo.cbSize = sizeof(SCROLLINFO);
		if (m_nScrollRangeY > 0) {
			ShowScrollBar(SSB_VERT, TRUE);
			EnableScrollBar(SSB_VERT, TRUE);
			tScrollInfo.fMask = SIF_ALL;
			tScrollInfo.nPage = m_tPaintDetails.nFullVisibleLines;
			tScrollInfo.nMax = m_nScrollRangeY + tScrollInfo.nPage - 1;
			if (m_nScrollPostionY > m_nScrollRangeY) {
				m_nScrollPostionY = m_nScrollRangeY;
			}
			tScrollInfo.nPos = m_nScrollPostionY;
			SetScrollInfo(tScrollInfo, TRUE);
			SetScrollPos(SSB_VERT, m_nScrollPostionY, TRUE);
		}
		else {
			ShowScrollBar(SSB_VERT, FALSE);
		}
		if (m_nScrollRangeX > 0) {
			EnableScrollBar(SSB_HORZ, TRUE);
			ShowScrollBar(SSB_HORZ, TRUE);
			tScrollInfo.fMask = SIF_ALL;
			tScrollInfo.nPage = m_tPaintDetails.cPaintingRect.Width();
			tScrollInfo.nMax = m_nScrollRangeX + tScrollInfo.nPage - 1;
			if (m_nScrollPostionX > m_nScrollRangeX) {
				m_nScrollPostionX = m_nScrollRangeX;
			}
			tScrollInfo.nPos = m_nScrollPostionX;
			SetScrollInfo(tScrollInfo, FALSE);
			SetScrollPos(SSB_HORZ, m_nScrollPostionX, TRUE);
		}
		else {
			ShowScrollBar(SSB_HORZ, FALSE);
		}
	}

	void SHexEdit::CalculatePaintingDetails(IRenderTarget* pRT)
	{
		m_bRecalc = false;

		// Get size information
		if (m_tPaintDetails.nCharacterWidth == 0)
		{
			SIZE sz;
			pRT->MeasureText(L"D", 1, &sz);

			m_tPaintDetails.nCharacterWidth = sz.cx;
			m_tPaintDetails.nLineHeight = sz.cy;
		}
		// count of visible lines
		GetClientRect(m_tPaintDetails.cPaintingRect);
		if (true) {
			m_tPaintDetails.cPaintingRect.InflateRect(-CONTROL_BORDER_SPACEH, -CONTROL_BORDER_SPACEV,
				-CONTROL_BORDER_SPACEH, -CONTROL_BORDER_SPACEV);
			if (m_tPaintDetails.cPaintingRect.right < m_tPaintDetails.cPaintingRect.left) {
				m_tPaintDetails.cPaintingRect.right = m_tPaintDetails.cPaintingRect.left;
			}
			if (m_tPaintDetails.cPaintingRect.bottom < m_tPaintDetails.cPaintingRect.top) {
				m_tPaintDetails.cPaintingRect.bottom = m_tPaintDetails.cPaintingRect.top;
			}
		}
		m_tPaintDetails.nVisibleLines = m_tPaintDetails.cPaintingRect.Height() / m_tPaintDetails.nLineHeight;
		m_tPaintDetails.nLastLineHeight = m_tPaintDetails.cPaintingRect.Height() % m_tPaintDetails.nLineHeight;
		if (m_tPaintDetails.nLastLineHeight > 0) {
			m_tPaintDetails.nFullVisibleLines = m_tPaintDetails.nVisibleLines;
			m_tPaintDetails.nVisibleLines++;
		}
		else {
			m_tPaintDetails.nFullVisibleLines = m_tPaintDetails.nVisibleLines;
			m_tPaintDetails.nLastLineHeight = m_tPaintDetails.nLineHeight;
		}

		// position & size of the address
		if (m_bShowAddress) {
			m_tPaintDetails.nAddressPos = 0;
			m_tPaintDetails.nAddressLen = ADR_DATA_SPACE + m_tPaintDetails.nCharacterWidth * m_nAdrSize;
		}
		else {
			m_tPaintDetails.nAddressPos = 0;
			m_tPaintDetails.nAddressLen = 0;
		}

		// Calculate how many bytes per line we can display, when this is automatically calculated
		if (m_bAutoBytesPerRow) {
			int iFreeSpace = m_tPaintDetails.cPaintingRect.Width() - m_tPaintDetails.nAddressLen;
			if (m_bShowAscii) {
				iFreeSpace -= DATA_ASCII_SPACE;
				if (iFreeSpace < 0) {
					m_tPaintDetails.nBytesPerRow = 1;
				}
				else {
					m_tPaintDetails.nBytesPerRow = iFreeSpace / (4 * m_tPaintDetails.nCharacterWidth); // 2(HEXDATA)+1(Space)+1(Ascii) = 4
					if ((iFreeSpace % (4 * m_tPaintDetails.nCharacterWidth)) >= (3 * m_tPaintDetails.nCharacterWidth)) {
						m_tPaintDetails.nBytesPerRow++; // we actually only need n-1 spaces not n (n = nBytesPerRow)
					}
				}
			}
			else {
				if (iFreeSpace < 0) {
					m_tPaintDetails.nBytesPerRow = 1;
				}
				else {
					m_tPaintDetails.nBytesPerRow = iFreeSpace / (3 * m_tPaintDetails.nCharacterWidth); // 2(HEXDATA)+1(Space) = 3
					if ((iFreeSpace % (3 * m_tPaintDetails.nCharacterWidth)) >= (2 * m_tPaintDetails.nCharacterWidth)) {
						m_tPaintDetails.nBytesPerRow++; // we actually only need n-1 spaces not n (n = nBytesPerRow)
					}
				}
			}
			//remark: m_nBytesPerRow=0 is a valid thing... (not very lucky thing, but valid)
		}
		else {
			m_tPaintDetails.nBytesPerRow = m_nBytesPerRow;
		}

		if (m_tPaintDetails.nBytesPerRow == 0) {
			m_tPaintDetails.nBytesPerRow = 1;
		}

		// position & size of the hex-data
		m_tPaintDetails.nHexPos = m_tPaintDetails.nAddressPos + m_tPaintDetails.nAddressLen;
		m_tPaintDetails.nHexLen = (m_tPaintDetails.nBytesPerRow * 2 + m_tPaintDetails.nBytesPerRow - 1) * m_tPaintDetails.nCharacterWidth;
		//2(HEXData) + 1(Space) (only n-1 spaces needed)
		int iWidth = m_tPaintDetails.nHexPos + m_tPaintDetails.nHexLen;
		m_tPaintDetails.nHexLen += DATA_ASCII_SPACE;

		// position & size of the ascii-data
		if (m_bShowAscii) {
			m_tPaintDetails.nAsciiPos = m_tPaintDetails.nHexPos + m_tPaintDetails.nHexLen;
			m_tPaintDetails.nAsciiLen = m_tPaintDetails.nBytesPerRow * m_tPaintDetails.nCharacterWidth;
			iWidth = m_tPaintDetails.nAsciiPos + m_tPaintDetails.nAsciiLen;
			iWidth++;
		}
		else {
			m_tPaintDetails.nAsciiPos = 0;
			m_tPaintDetails.nAsciiLen = 0;
		}

		// calculate scrollranges	
		// Y-Bar
		UINT nTotalLines;
		nTotalLines = (GetDataSize() + m_tPaintDetails.nBytesPerRow - 1) / m_tPaintDetails.nBytesPerRow;
		if (nTotalLines > m_tPaintDetails.nFullVisibleLines) {
			m_nScrollRangeY = nTotalLines - m_tPaintDetails.nFullVisibleLines;
		}
		else {
			m_nScrollRangeY = 0;
		}
		if (m_nScrollPostionY > m_nScrollRangeY) {
			m_nScrollPostionY = m_nScrollRangeY;
		}

		// X-Bar
		if (iWidth > m_tPaintDetails.cPaintingRect.Width()) {
			m_nScrollRangeX = iWidth - m_tPaintDetails.cPaintingRect.Width();
		}
		else {
			m_nScrollRangeX = 0;
		}
		if (m_nScrollPostionX > m_nScrollRangeX) {
			m_nScrollPostionX = m_nScrollRangeX;
		}

		SetScrollbarRanges();
	}

	void SHexEdit::PaintAddresses(IRenderTarget* pRT)
	{
		UINT nAdr;
		UINT nEndAdr;
		SStringT cAdrFormatString;
		CRect cAdrRect(m_tPaintDetails.cPaintingRect);
		_TCHAR pBuf[32];

		// create the format string
		cAdrFormatString.Format(_T("%%0%uX"), m_nAdrSize);

		// the Rect for painting & background
		cAdrRect.left += m_tPaintDetails.nAddressPos - m_nScrollPostionX;
		cAdrRect.right = cAdrRect.left + m_tPaintDetails.nAddressLen - ADR_DATA_SPACE; // without border
		pRT->FillSolidRect(cAdrRect, m_tAdrBkgCol);
		cAdrRect.bottom = cAdrRect.top + m_tPaintDetails.nLineHeight;

		// start & end-address
		nAdr = m_nScrollPostionY * m_tPaintDetails.nBytesPerRow;
		nEndAdr = nAdr + m_tPaintDetails.nVisibleLines * m_tPaintDetails.nBytesPerRow;
		if (nEndAdr >= GetDataSize()) {
			nEndAdr = GetDataSize();
		}

		//  paint
		pRT->SetTextColor(m_tAdrTxtCol);
		for (; nAdr < nEndAdr; nAdr += m_tPaintDetails.nBytesPerRow) {
			_sntprintf(pBuf, 32, (LPCTSTR)cAdrFormatString, nAdr + m_dwZeroAddr); // slightly faster then CString::Format
			pRT->DrawText(pBuf, -1, (LPRECT)cAdrRect, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
			cAdrRect.OffsetRect(0, m_tPaintDetails.nLineHeight);
		}
	}

	void SHexEdit::PaintHexData(IRenderTarget* pRT)
	{
		if (GetDataSize() == 0) {
			return;
		}

		UINT nAdr;
		UINT nEndAdr;
		UINT nSelectionCount = 0;
		TCHAR pBuf[1024];
		TCHAR* pBufPtr;
		TCHAR* pSelectionBufPtrBegin;
		TCHAR* pSelectionBufPtrEnd;

		BYTE* pDataPtr;
		BYTE* pEndDataPtr;
		BYTE* pEndLineDataPtr;
		BYTE* pSelectionPtrBegin;
		BYTE* pSelectionPtrEnd;

		CRect cHexRect(m_tPaintDetails.cPaintingRect);

		// prepare the buffer for the formated hex-data
		memset(pBuf, 0, m_tPaintDetails.nBytesPerRow * 3);	// fill with spaces
		pBuf[m_tPaintDetails.nBytesPerRow * 3 - 1] = '\0';		// zero-terminate

		// the Rect for painting & background
		cHexRect.left += m_tPaintDetails.nHexPos - m_nScrollPostionX;
		cHexRect.right = cHexRect.left + m_tPaintDetails.nHexLen - DATA_ASCII_SPACE;
		pRT->FillSolidRect(cHexRect, m_tHexBkgCol);
		cHexRect.bottom = cHexRect.top + m_tPaintDetails.nLineHeight;

		// selection (pointers)
		if ((m_nSelectionBegin != NOSECTION_VAL) && (m_nSelectionEnd != NOSECTION_VAL)) {
			pSelectionPtrBegin = m_xData.GetData(m_nSelectionBegin);
			pSelectionPtrEnd = m_xData.GetData(m_nSelectionEnd);
		}
		else {
			pSelectionPtrBegin = NULL;
			pSelectionPtrEnd = NULL;
		}

		// start & end-address (& pointers)
		nAdr = m_nScrollPostionY * m_tPaintDetails.nBytesPerRow;
		nEndAdr = nAdr + m_tPaintDetails.nVisibleLines * m_tPaintDetails.nBytesPerRow;
		if (nEndAdr >= GetDataSize()) {
			nEndAdr = GetDataSize() - 1;
		}
		pDataPtr = m_xData.GetData(nAdr);
		pEndDataPtr = m_xData.GetData(nEndAdr);

		//  paint
		while (pDataPtr < pEndDataPtr + 1) {
			pEndLineDataPtr = pDataPtr + m_tPaintDetails.nBytesPerRow;
			if (pEndLineDataPtr > pEndDataPtr) {
				pEndLineDataPtr = pEndDataPtr + 1;
			}
			pSelectionBufPtrBegin = NULL;
			pSelectionBufPtrEnd = NULL;

			if ((pDataPtr >= pSelectionPtrBegin) && (pDataPtr <= pSelectionPtrEnd)) {
				pSelectionBufPtrBegin = pBuf;
			}

			for (pBufPtr = pBuf; pDataPtr < pEndLineDataPtr; ++pDataPtr) {
				if (pDataPtr == pSelectionPtrBegin) {
					pSelectionBufPtrBegin = pBufPtr;
				}
				if (pDataPtr == pSelectionPtrEnd) {
					if (pSelectionBufPtrBegin == NULL) {
						pSelectionBufPtrBegin = pBuf;
					}
					pSelectionBufPtrEnd = pBufPtr + 2;
				}

				*pBufPtr++ = tabHexCharacters[*pDataPtr >> 4];
				*pBufPtr++ = tabHexCharacters[*pDataPtr & 0xf];
				*pBufPtr++ = ' ';
			}
			*--pBufPtr = '\0';
			// set end-pointers

			if (pSelectionBufPtrEnd == NULL) {
				pSelectionBufPtrEnd = pBufPtr;
			}
			// first draw all normal
			pRT->SetTextColor(m_tHexTxtCol);
			pRT->DrawText(pBuf, -1, (LPRECT)cHexRect, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);

			// selection
			if (pSelectionBufPtrBegin != NULL) {
				BOOL bHasFocus = IsFocused();

				CRect cRect(cHexRect);
				cRect.left += (pSelectionBufPtrBegin - pBuf) * m_tPaintDetails.nCharacterWidth;
				cRect.right -= (pBuf - 1 + m_tPaintDetails.nBytesPerRow * 3 - pSelectionBufPtrEnd) * m_tPaintDetails.nCharacterWidth;
				CRect cSelectionRect(cRect);
				cSelectionRect.InflateRect(0, -1, +1, +1);
				*pSelectionBufPtrEnd = '\0'; // set "end-mark"
				pRT->FillSolidRect(cSelectionRect, !m_bCaretAscii ? m_tSelectedFousBkgCol : m_tSelectedNoFocusBkgCol);
				pRT->SetTextColor(!m_bCaretAscii ? m_tSelectedFousTxtCol : m_tSelectedNoFocusTxtCol);
				pRT->DrawText(pSelectionBufPtrBegin, -1, (LPRECT)cRect, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
				*pSelectionBufPtrEnd = ' '; // restore the buffer
			}
			cHexRect.OffsetRect(0, m_tPaintDetails.nLineHeight);
		}
	}

	void SHexEdit::PaintAsciiData(IRenderTarget* pRT)
	{
		if (GetDataSize() == 0) {
			return;
		}

		UINT nAdr;
		UINT nEndAdr;
		CRect cAsciiRect(m_tPaintDetails.cPaintingRect);
		char pBuf[512];
		char* pBufPtr;

		BYTE* pDataPtr;
		BYTE* pDataPtrEnd;

		BYTE* pSelectionPtrBegin;
		BYTE* pSelectionPtrEnd;
		BYTE* pEndDataPtr;
		char* pSelectionBufPtrBegin;
		char* pSelectionBufPtrEnd;


		memset(pBuf, 0, m_tPaintDetails.nBytesPerRow + 1);

		// the Rect for painting & background
		cAsciiRect.left += m_tPaintDetails.nAsciiPos - m_nScrollPostionX;
		cAsciiRect.right = cAsciiRect.left + m_tPaintDetails.nAsciiLen;
		pRT->FillSolidRect(cAsciiRect, m_tAsciiBkgCol);
		cAsciiRect.bottom = cAsciiRect.top + m_tPaintDetails.nLineHeight;

		//highlighting section	
		// selection (pointers)
		if ((m_nSelectionBegin != NOSECTION_VAL) && (m_nSelectionEnd != NOSECTION_VAL)) {
			pSelectionPtrBegin = m_xData.GetData(m_nSelectionBegin);
			pSelectionPtrEnd = m_xData.GetData(m_nSelectionEnd);
		}
		else {
			pSelectionPtrBegin = NULL;
			pSelectionPtrEnd = NULL;
		}
		//highlighting section

		// start & end-address
		nAdr = m_nScrollPostionY * m_tPaintDetails.nBytesPerRow;
		nEndAdr = nAdr + m_tPaintDetails.nVisibleLines * m_tPaintDetails.nBytesPerRow;
		if (nEndAdr >= GetDataSize()) {
			nEndAdr = GetDataSize() - 1;
		}

		pDataPtr = m_xData.GetData(nAdr);
		pEndDataPtr = m_xData.GetData(nEndAdr);

		//  paint

		for (; nAdr <= nEndAdr; nAdr += m_tPaintDetails.nBytesPerRow) {
			pDataPtrEnd = pDataPtr + m_tPaintDetails.nBytesPerRow;
			if (pDataPtrEnd > pEndDataPtr) {
				pDataPtrEnd = pEndDataPtr + 1;
			}

			pSelectionBufPtrBegin = NULL;
			pSelectionBufPtrEnd = NULL;

			if ((pDataPtr >= pSelectionPtrBegin) && (pDataPtr <= pSelectionPtrEnd)) {
				pSelectionBufPtrBegin = pBuf;
			}

			for (pBufPtr = pBuf; pDataPtr < pDataPtrEnd; ++pDataPtr, ++pBufPtr) {

				if (pDataPtr == pSelectionPtrBegin) {
					pSelectionBufPtrBegin = pBufPtr;
				}
				if (pDataPtr == pSelectionPtrEnd) {
					if (pSelectionBufPtrBegin == NULL) {
						pSelectionBufPtrBegin = pBuf;
					}
					pSelectionBufPtrEnd = pBufPtr + 1;
				}

				*pBufPtr = isprint(*pDataPtr) ? (char)*pDataPtr : '.';
				// 支持汉字的显示, 会导致Ascii区对齐计算不正确
				//*pBufPtr = (*pDataPtr >= 0x20) ? (char)*pDataPtr : '.';	
			}
			*pBufPtr = '\0';

			// set end-pointers
			if (pSelectionBufPtrEnd == NULL) {
				pSelectionBufPtrEnd = pBufPtr;
			}

			pRT->SetTextColor(m_tAsciiTxtCol);
			SStringT outText = S_CA2T(pBuf);
			pRT->DrawText(outText, -1, (LPRECT)cAsciiRect, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);

			//highlighted section now

		   //// selection
			if (pSelectionBufPtrBegin != NULL) {
				BOOL bHasFocus = IsFocused();

				CRect cRect(cAsciiRect);
				cRect.left += (pSelectionBufPtrBegin - pBuf) * m_tPaintDetails.nCharacterWidth;
				cRect.right -= (pBuf + m_tPaintDetails.nBytesPerRow - pSelectionBufPtrEnd) * m_tPaintDetails.nCharacterWidth;
				CRect cSelectionRect(cRect);
				cSelectionRect.InflateRect(0, -1, +1, +1);
				*pSelectionBufPtrEnd = '\0'; // set "end-mark"
				pRT->FillSolidRect(cSelectionRect, m_bCaretAscii ? m_tSelectedFousBkgCol : m_tSelectedNoFocusBkgCol);
				pRT->SetTextColor(m_bCaretAscii ? m_tSelectedFousTxtCol : m_tSelectedNoFocusTxtCol);
				SStringT outText = S_CA2T(pSelectionBufPtrBegin);
				pRT->DrawText(outText, -1, (LPRECT)cRect, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
				*pSelectionBufPtrEnd = ' '; // restore the buffer
			}

			cAsciiRect.OffsetRect(0, m_tPaintDetails.nLineHeight);
		}
	}

	void SHexEdit::SetEditCaretPos(UINT nOffset, bool bHighBits)
	{
		m_nCurrentAddress = nOffset;
		m_bHighBits = bHighBits;

		if (GetDataSize() == 0) {
			return;
		}
		if (m_nCurrentAddress >= GetDataSize())
			m_nCurrentAddress--;

		if (m_nCurrentAddress < m_nScrollPostionY * m_tPaintDetails.nBytesPerRow
			|| (m_nCurrentAddress >= (m_nScrollPostionY + m_tPaintDetails.nVisibleLines) * m_tPaintDetails.nBytesPerRow)) {
			// not in the visible range
			DestoyEditCaret();
			return;
		}

		UINT nRelAdr = m_nCurrentAddress - m_nScrollPostionY * m_tPaintDetails.nBytesPerRow;
		UINT nRow = nRelAdr / m_tPaintDetails.nBytesPerRow;
		UINT nColumn = nRelAdr % m_tPaintDetails.nBytesPerRow;
		UINT nCarretHeight;
		UINT nCarretWidth = m_tPaintDetails.nCharacterWidth;
		if (nRow == m_tPaintDetails.nVisibleLines - 1) {
			// last row can be only half visible
			nCarretHeight = m_tPaintDetails.nLastLineHeight;
		}
		else {
			nCarretHeight = m_tPaintDetails.nLineHeight;
		}

		UINT xpos = m_tPaintDetails.cPaintingRect.left - m_nScrollPostionX;
		if (m_bCaretAscii)
			xpos += m_tPaintDetails.nAsciiPos + nColumn * m_tPaintDetails.nCharacterWidth;
		else
			xpos += m_tPaintDetails.nHexPos + (nColumn * 3 + (bHighBits ? 0 : 1)) * m_tPaintDetails.nCharacterWidth;
		if (m_bCaretAtLineEnd)
			xpos += (bHighBits ? 2 : 1) * m_tPaintDetails.nCharacterWidth;

		UINT ypos = m_tPaintDetails.cPaintingRect.top + 1 + nRow * m_tPaintDetails.nLineHeight;
		CPoint cCarretPoint(xpos, ypos);

		if ((cCarretPoint.x + (short)m_tPaintDetails.nCharacterWidth <= m_tPaintDetails.cPaintingRect.left - 2)
			|| (cCarretPoint.x > m_tPaintDetails.cPaintingRect.right)) {
			// we can't see it
			DestoyEditCaret();
			return;
		}
		if (cCarretPoint.x < m_tPaintDetails.cPaintingRect.left - 2) {
			nCarretWidth -= m_tPaintDetails.cPaintingRect.left - 2 - cCarretPoint.x;
			cCarretPoint.x = m_tPaintDetails.cPaintingRect.left - 2;
		}
		if (cCarretPoint.x + (int)nCarretWidth > (int)m_tPaintDetails.cPaintingRect.right + 2) {
			nCarretWidth = m_tPaintDetails.cPaintingRect.right + 2 - cCarretPoint.x;
		}

		m_CaretX = cCarretPoint.x;
		m_CaretY = cCarretPoint.y;
		CreateEditCaret(m_overwriteMode ? 2 : nCarretHeight - 1, m_overwriteMode ? nCarretWidth : 1); // FIXME: hardcoded insert caret width 2
		SetCaretPos(m_CaretX, m_overwriteMode ? m_CaretY + nCarretHeight - 3 : m_CaretY);
		ShowCaret(TRUE);
	}

	void SHexEdit::CreateEditCaret(UINT nCaretHeight, UINT nCaretWidth)
	{
		if (!m_bHasCaret || (nCaretHeight != m_nCurCaretHeight)
			|| (nCaretWidth != m_nCurCaretWidth)) {
			m_bHasCaret = true;
			m_nCurCaretHeight = nCaretHeight;
			m_nCurCaretWidth = nCaretWidth;
			CreateCaret(NULL, m_nCurCaretWidth, m_nCurCaretHeight);
		}
	}

	void SHexEdit::DestoyEditCaret() {
		m_bHasCaret = false;
		ShowCaret(FALSE);
	}

	void SHexEdit::OnTimer(UINT nTimerID)
	{
		if (m_bIsMouseRepActive && (nTimerID == MOUSEREP_TIMER_TID)) {
			if (m_nMouseRepCounter > 0) {
				m_nMouseRepCounter--;
			}
			else {
				m_nMouseRepCounter = m_nMouseRepSpeed;
				MoveScrollPostionY(m_iMouseRepDelta, false);
				GetAddressFromPoint(m_cMouseRepPoint, m_nCurrentAddress, m_bHighBits);
				SetEditCaretPos(m_nCurrentAddress, m_bHighBits);
				m_nSelectingEnd = m_nCurrentAddress;
				m_nSelectionBegin = m_nSelectingBeg;
				m_nSelectionEnd = m_nSelectingEnd;
				NORMALIZE_SELECTION(m_nSelectionBegin, m_nSelectionEnd);
				Invalidate();
			}
		}
	}

	void SHexEdit::StartMouseRepeat(const CPoint& cPoint, int iDelta, WORD nSpeed)
	{
		m_cMouseRepPoint = cPoint;
		m_nMouseRepSpeed = nSpeed;
		m_iMouseRepDelta = iDelta;
		if (!m_bIsMouseRepActive) {
			m_bIsMouseRepActive = true;
			m_nMouseRepCounter = nSpeed;
			SetTimer2(MOUSEREP_TIMER_TID, MOUSEREP_TIMER_ELAPSE);
		}
	}

	void SHexEdit::StopMouseRepeat()
	{
		if (m_bIsMouseRepActive) {
			m_bIsMouseRepActive = false;
			KillTimer2(MOUSEREP_TIMER_TID);
		}
	}

	void SHexEdit::SetScrollPositionY(UINT nPosition, bool bUpdate)
	{
		if (nPosition > m_nScrollRangeY) {
			nPosition = m_nScrollRangeY;
		}
		SetScrollPos(SSB_VERT, (int)nPosition, TRUE);
		if ((nPosition != m_nScrollPostionY) && bUpdate) {
			m_nScrollPostionY = nPosition;
			Invalidate();
		}
		SetEditCaretPos(m_nCurrentAddress, m_bHighBits);
		m_nScrollPostionY = nPosition;
	}

	void SHexEdit::SetScrollPositionX(UINT nPosition, bool bUpdate)
	{
		if (nPosition > m_nScrollRangeX) {
			nPosition = m_nScrollRangeX;
		}
		SetScrollPos(SSB_HORZ, (int)nPosition, TRUE);
		if ((nPosition != m_nScrollPostionX) && bUpdate) {
			m_nScrollPostionX = nPosition;
			Invalidate();
			SetEditCaretPos(m_nCurrentAddress, m_bHighBits);
		}
		m_nScrollPostionX = nPosition;
	}

	void SHexEdit::MoveScrollPostionY(int iDelta, bool bUpdate)
	{
		if (iDelta > 0) {
			SetScrollPositionY(m_nScrollPostionY + iDelta, bUpdate);
		}
		else {
			int iPositon = (int)m_nScrollPostionY;
			iPositon -= (-iDelta);
			if (iPositon < 0) {
				iPositon = 0;
			}
			SetScrollPositionY((UINT)iPositon, bUpdate);
		}
	}

	void SHexEdit::MoveScrollPostionX(int iDelta, bool bUpdate)
	{
		if (iDelta > 0) {
			SetScrollPositionX(m_nScrollPostionX + iDelta, bUpdate);
		}
		else {
			int iPositon = (int)m_nScrollPostionX;
			iPositon -= (-iDelta);
			if (iPositon < 0) {
				iPositon = 0;
			}
			SetScrollPositionX((UINT)iPositon, bUpdate);
		}
	}

	void SHexEdit::MakeVisible(UINT nBegin, UINT nEnd, bool bUpdate)
	{
		UINT nAdrBeg = m_nScrollPostionY * m_tPaintDetails.nBytesPerRow;
		UINT nFullBytesPerScreen = m_tPaintDetails.nFullVisibleLines * m_tPaintDetails.nBytesPerRow;
		UINT nAdrEnd = nAdrBeg + nFullBytesPerScreen;
		UINT nLength = nEnd - nBegin;
		if ((nBegin > nAdrBeg) || (nEnd < nAdrEnd)) {
			// don't do anything when it's simply not possible to see everything and
			// we already see one ful page.
			if (nLength > nFullBytesPerScreen) {
				if (nAdrBeg < nBegin) {
					SetScrollPositionY(nBegin / m_tPaintDetails.nBytesPerRow, false);
				}
				else if (nAdrEnd > nEnd) {
					SetScrollPositionY((nEnd - nFullBytesPerScreen + m_tPaintDetails.nBytesPerRow) / m_tPaintDetails.nBytesPerRow, false);
				}
			}
			else {
				if (nAdrBeg > nBegin) {
					SetScrollPositionY(nBegin / m_tPaintDetails.nBytesPerRow, false);
				}
				else if (nAdrEnd < nEnd) {
					SetScrollPositionY((nEnd - nFullBytesPerScreen + m_tPaintDetails.nBytesPerRow) / m_tPaintDetails.nBytesPerRow, false);
				}
			}
		}

		int iLineX = (int)((nBegin % m_tPaintDetails.nBytesPerRow) * 3 * m_tPaintDetails.nCharacterWidth + m_tPaintDetails.nHexPos + m_tPaintDetails.cPaintingRect.left) - (int)m_nScrollPostionX;
		int iLineX2 = (int)((2 + (nEnd % m_tPaintDetails.nBytesPerRow) * 3) * m_tPaintDetails.nCharacterWidth + m_tPaintDetails.nHexPos + m_tPaintDetails.cPaintingRect.left) - (int)m_nScrollPostionX;
		if (iLineX > iLineX2) {
			int iTemp = iLineX;
			iLineX = iLineX2;
			iLineX2 = iTemp;
		}
		if ((iLineX <= m_tPaintDetails.cPaintingRect.left) && (iLineX2 >= m_tPaintDetails.cPaintingRect.right)) {
			// nothing to do here...
		}
		else if (iLineX < m_tPaintDetails.cPaintingRect.left) {
			SetScrollPositionX(m_nScrollPostionX + iLineX - m_tPaintDetails.cPaintingRect.left, false);
		}
		else if (iLineX2 >= m_tPaintDetails.cPaintingRect.right) {
			SetScrollPositionX(m_nScrollPostionX + iLineX2 - m_tPaintDetails.cPaintingRect.Width(), false);
		}

		if (bUpdate) {
			Invalidate();
		}
	}

	void SHexEdit::MoveCurrentAddress(int iDeltaAdr, bool bHighBits, bool bIgnoreShift)
	{
		bool bExtendSelection = !bIgnoreShift && (GetKeyState(VK_SHIFT) & 0x80000000) == 0x80000000;

		if (GetDataSize() == 0) {
			return;
		}

		UINT nAddress = m_nCurrentAddress;

		if (!bExtendSelection) {
			m_nSelectingBeg = NOSECTION_VAL;
			m_nSelectionBegin = NOSECTION_VAL;
			m_nSelectingEnd = NOSECTION_VAL;
			m_nSelectionEnd = NOSECTION_VAL;
		}
		if (iDeltaAdr >= 0) {
			// go down
			if (nAddress + iDeltaAdr >= GetDataSize()) {
				// we reached the end
				nAddress = GetDataSize() - 1;
				bHighBits = false;
				m_bCaretAtLineEnd = true;
			}
			else {
				nAddress += iDeltaAdr;
			}
		}
		else if (iDeltaAdr < 0) {
			if ((UINT)(-iDeltaAdr) <= nAddress) {
				if (m_bCaretAtLineEnd && iDeltaAdr == -1)
				{
					m_bCaretAtLineEnd = false;
				}else
					nAddress -= (UINT)(-iDeltaAdr);
				if (m_bCaretAtLineEnd && ((nAddress + 1) % m_tPaintDetails.nBytesPerRow))
				{
					m_bCaretAtLineEnd = false;
					if ((UINT)(-iDeltaAdr) == m_tPaintDetails.nBytesPerRow)
					{
						nAddress++;
						bHighBits = true;
					}
				}
			}
			else {
				nAddress = 0;
				bHighBits = true;
				m_bCaretAtLineEnd = false;
			}
		}
		if (bExtendSelection && (m_nSelectingBeg != NOSECTION_VAL)) {
			m_nSelectingEnd = nAddress;
			m_nSelectionBegin = m_nSelectingBeg;
			m_nSelectionEnd = m_nSelectingEnd;
			NORMALIZE_SELECTION(m_nSelectionBegin, m_nSelectionEnd);
		}
		MakeVisible(nAddress, nAddress, true);
		SetEditCaretPos(nAddress, bHighBits);
	}


	void SHexEdit::GetAddressFromPoint(const CPoint& cPt, UINT& nAddress, bool& bHighBits)
	{
		m_bCaretAtLineEnd = false;
		bool bAscii = false;
		CPoint cPoint(cPt);
		cPoint.x += m_nScrollPostionX;
		cPoint.x += (m_tPaintDetails.nCharacterWidth >> 1);
		cPoint.y -= m_tPaintDetails.cPaintingRect.top;
		cPoint.x -= m_tPaintDetails.cPaintingRect.left + CONTROL_BORDER_SPACEH + 3; // 3 determined experimentally :-(

		if (cPoint.y < 0) {
			cPoint.y = 0;
		}
		else if (cPoint.y > (int)(m_tPaintDetails.nVisibleLines * m_tPaintDetails.nLineHeight)) {
			cPoint.y = m_tPaintDetails.nVisibleLines * m_tPaintDetails.nLineHeight;
		}

		if ((int)cPoint.x < (int)m_tPaintDetails.nHexPos) {
			cPoint.x = 0;
		}
		else if ((int)cPoint.x > (int)(m_tPaintDetails.nAsciiPos)) {
			cPoint.x -= m_tPaintDetails.nAsciiPos;
			bAscii = true;
		}
		else {
			cPoint.x -= m_tPaintDetails.nHexPos;
			if (cPoint.x >= (LONG)m_tPaintDetails.nHexLen - DATA_ASCII_SPACE)
			{
				cPoint.x = (LONG)m_tPaintDetails.nHexLen - DATA_ASCII_SPACE - 1;
				m_bCaretAtLineEnd = true;
			}
		}

		UINT nRow = cPoint.y / m_tPaintDetails.nLineHeight;
		UINT nCharColumn = cPoint.x / m_tPaintDetails.nCharacterWidth;
		UINT nColumn;

		if (bAscii)
			nColumn = nCharColumn;
		else
			nColumn = nCharColumn / 3;

		bHighBits = nCharColumn % 3 == 0;
		nAddress = nColumn + (nRow + m_nScrollPostionY) * m_tPaintDetails.nBytesPerRow;

		if (nAddress >= GetDataSize()) {
			nAddress = GetDataSize() - 1;
			m_bCaretAtLineEnd = true;
			if ((int)nAddress < 0)
				nAddress = 0;
			bHighBits = false;
		}

		m_bCaretAscii = bAscii;
	}

	BOOL SHexEdit::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		MoveScrollPostionY(-(zDelta / WHEEL_DELTA), true);
		return TRUE;
	}


	BOOL SHexEdit::OnScroll(BOOL bVertical, UINT uCode, int nPos)
	{
		if (m_dragSb != SSB_NULL)
		{
			if (bVertical)
				OnVScroll(uCode, nPos, NULL);
			else
				OnHScroll(uCode, nPos, NULL);
		}
		else
			__super::OnScroll(bVertical, uCode, nPos);
		return TRUE;
	}

	void SHexEdit::OnVScroll(UINT nSBCode, UINT nPos, HWND)
	{
		if (GetDataSize() == 0) {
			return;
		}

		switch (nSBCode) {
		case SB_LINEDOWN:
			MoveScrollPostionY(1, true);
			break;

		case SB_LINEUP:
			MoveScrollPostionY(-1, true);
			break;

		case SB_PAGEDOWN:
			MoveScrollPostionY(m_tPaintDetails.nFullVisibleLines, true);
			break;

		case SB_PAGEUP:
			MoveScrollPostionY(-(int)m_tPaintDetails.nFullVisibleLines, true);
			break;

		case SB_THUMBTRACK:
			// Windows only allows 16Bit track-positions in the callback message.
			// MFC hides this by providing a 32-bit value (nobody expects to
			// be an invalid value) which is unfortunately casted from a 16Bit value.
			// -- MSDN gives a hint (in the API-documentation) about this problem
			// -- and a solution as well. We should use GetScrollInfo here to receive
			// -- the correct 32-Bit value when our scrollrange exceeds the 16bit range
			// -- to keep it simple, I decided to always do it like this
			SetScrollPositionY(nPos, true);
			break;
		}
	}

	void SHexEdit::OnHScroll(UINT nSBCode, UINT nPos, HWND)
	{
		if (GetDataSize() == 0) {
			return;
		}

		switch (nSBCode) {
		case SB_LINEDOWN:
			MoveScrollPostionX(m_tPaintDetails.nCharacterWidth, true);
			break;

		case SB_LINEUP:
			MoveScrollPostionX(-(int)m_tPaintDetails.nCharacterWidth, true);
			break;

		case SB_PAGEDOWN:
			MoveScrollPostionX(m_tPaintDetails.cPaintingRect.Width(), true);
			break;

		case SB_PAGEUP:
			MoveScrollPostionX(-(int)m_tPaintDetails.cPaintingRect.Width(), true);
			break;

		case SB_THUMBTRACK:
			SetScrollPositionX(nPos, true);
			break;
		}
	}

	void SHexEdit::OnMouseMove(UINT nFlags, CPoint point)
	{
		if (GetDataSize() == 0) {
			return;
		}
		if ((nFlags & MK_LBUTTON) && (m_nSelectingBeg != NOSECTION_VAL)) {
			// first make a self built drag-detect (one that doesn't block)
			if (!m_cDragRect.PtInRect(point)) {
				m_cDragRect = CRect(-1, -1, -1, -1); // when once, out, kill it...
			}
			else {
				return; // okay, still not draging
			}
			if (!m_tPaintDetails.cPaintingRect.PtInRect(point)) {
				int iRepSpeed = 0;
				int iDelta = 0;
				if (point.y < m_tPaintDetails.cPaintingRect.top) {
					iDelta = -1;
					iRepSpeed = (int)m_tPaintDetails.cPaintingRect.top + 1 - (int)point.y;
				}
				else if (point.y > m_tPaintDetails.cPaintingRect.bottom) {
					iDelta = 1;
					iRepSpeed = (int)point.y - (int)m_tPaintDetails.cPaintingRect.bottom + 1;
				}
				if (iDelta != 0) {
					iRepSpeed /= 5;
					if (iRepSpeed > 5) {
						iRepSpeed = 6;
					}
					StartMouseRepeat(point, iDelta, (short)(7 - iRepSpeed));
				}
				m_cMouseRepPoint = point; // make sure we always have the latest point
			}
			else {
				StopMouseRepeat();
			}
			GetAddressFromPoint(point, m_nCurrentAddress, m_bHighBits);
			SetEditCaretPos(m_nCurrentAddress, m_bHighBits);
			m_nSelectingEnd = m_nCurrentAddress;
			m_nSelectionBegin = m_nSelectingBeg;
			m_nSelectionEnd = m_nSelectingEnd;
			NORMALIZE_SELECTION(m_nSelectionBegin, m_nSelectionEnd);
			Invalidate();
		}
	}

	void SHexEdit::OnLButtonUp(UINT nFlags, CPoint pt)
	{
		if (GetCapture() == m_swnd) {
			ReleaseCapture();

			EventHexEditSelChanged evt(this);
			evt.m_nSelBegin = m_nSelectionBegin;
			evt.m_nSelEnd = m_nSelectionEnd;
			FireEvent(evt);
		}
		StopMouseRepeat(); // in case it's started, otherwise it doesn't matter either
		Invalidate();
	}

	void SHexEdit::OnLButtonDown(UINT nFlags, CPoint pt)
	{
		__super::OnLButtonDown(nFlags, pt);
		GetContainer()->OnSetSwndFocus(m_swnd);

		GetAddressFromPoint(pt, m_nCurrentAddress, m_bHighBits);
		SetEditCaretPos(m_nCurrentAddress, m_bHighBits);

		int iDragCX = GetSystemMetrics(SM_CXDRAG);
		int iDragCY = GetSystemMetrics(SM_CYDRAG);
		m_cDragRect = CRect(pt.x - (iDragCX >> 1), pt.y - (iDragCY >> 1),
			pt.x + (iDragCX >> 1) + (iDragCX & 1), //(we don't want to loose a pixel, when it's so small)
			pt.y + (iDragCY >> 1) + (iDragCY & 1));

		m_nSelectingEnd = NOSECTION_VAL;
		m_nSelectionBegin = NOSECTION_VAL;
		m_nSelectingEnd = NOSECTION_VAL;
		m_nSelectingBeg = m_nCurrentAddress;
		SetCapture();
	}

	void SHexEdit::OnLButtonDbClick(UINT nFlags, CPoint pt)
	{
	}

	std::string SHexEdit::GetClipboardText()
	{
		std::string text;
		HGLOBAL hGlobal;		// Global memory handle
		LPSTR lpszData;			// Pointer to clipboard data
		unsigned long nSize;	// Size of clipboard data

		OpenClipboard(NULL);

		if (IsClipboardFormatAvailable(CF_TEXT))
			hGlobal = GetClipboardData(CF_TEXT);
		else
		{
			CloseClipboard();
			return text;
		}

		if (hGlobal == NULL) {
			CloseClipboard();
			return text;
		}

		lpszData = (LPSTR)GlobalLock(hGlobal);
		nSize = GlobalSize(hGlobal);

		text = lpszData;

		GlobalUnlock(hGlobal);
		CloseClipboard();

		return text;
	}

	int SHexEdit::SetClipboard(LPCSTR lpszBuffer)
	{
		HGLOBAL hGlobal;		// Global memory handle
		LPSTR lpszData;			// Pointer to clipboard data
		unsigned long nSize;	// Size of clipboard data

		// First, open the clipboard. OpenClipboard() takes one
		// parameter, the handle of the window that will temporarily
		// be it's owner. If NULL is passed, the current process
		// is assumed. After opening, empty the clipboard so we
		// can put our text on it.
		OpenClipboard(NULL);
		EmptyClipboard();

		// Get the size of the string in the buffer that was
		// passed into the function, so we know how much global
		// memory to allocate for the string.
		nSize = strlen(lpszBuffer);

		// Allocate the memory for the string.
		hGlobal = GlobalAlloc(GMEM_ZEROINIT, nSize + 1);

		// If we got any error during the memory allocation,
		// we have been returned a NULL handle.
		if (hGlobal == NULL) {
			CloseClipboard();
			return FALSE;
		}

		// Now we have a global memory handle to the text
		// stored on the clipboard. We have to lock this global
		// handle so that we have access to it.
		lpszData = (LPSTR)GlobalLock(hGlobal);

		// Now, copy the text from the buffer into the allocated
		// global memory pointer.
		for (UINT i = 0; i < nSize + 1; ++i)
			*(lpszData + i) = *(lpszBuffer + i);

		// Now, simply unlock the global memory pointer,
		// set the clipboard data type and pointer,
		// and close the clipboard.
		GlobalUnlock(hGlobal);
		SetClipboardData(CF_TEXT, hGlobal);
		CloseClipboard();

		return TRUE;
	}

	void SHexEdit::OnEditCopy()
	{
		if (!IsSelection())
			return;

		SByteArray selData = m_xData.Mid(m_nSelectionBegin, m_nSelectionEnd - m_nSelectionBegin + 1);
		if (m_bCaretAscii)
			SetClipboard(selData.ToString().c_str());
		else
			SetClipboard(selData.ToHex(' ').ToString().c_str());
	}

	static bool isHexSeparator(BYTE b)
	{
		switch (b) {
		case '\t':
		case '\r':
		case '\n':
		case ':':
		case ' ':
			return true;
		default:
			return false;
		}
	}

	static UINT countHexDigits(const char* data, UINT ndata)
	{
		UINT n = 0;
		while (ndata > 0) {
			if (isxdigit(*data))
				n++;
			else if ((n & 1) == 0 && isHexSeparator(*data))
				; // skip white space etc between bytes
			else
				return -1;
			data++;
			ndata--;
		}
		return n;
	}

	void SHexEdit::OnEditPaste()
	{
		if (m_bReadOnly)
			return;

		std::string clipText = GetClipboardText();
		if (clipText.empty())
			return;

		UINT nPasteAdr = m_nCurrentAddress;
		UINT nReplaceLength = 0;
		if (IsSelection()) {
			nPasteAdr = m_nSelectionBegin;
			nReplaceLength = m_nSelectionEnd - m_nSelectionBegin + 1;
			if (m_nSelectionEnd >= GetDataSize())
				nReplaceLength--;
		}

		bool doHexdecode = false;
		int nHexDigits = countHexDigits(clipText.c_str(), clipText.length()); // < 0 or odd if non valid hex encoding
		if (nHexDigits > 0 && nHexDigits % 2 == 0) {
			doHexdecode = true;
		}

		SByteArray pasteData;
		if (doHexdecode)
			pasteData.FromHexString(clipText);
		else
			pasteData.Append((BYTE*)clipText.c_str(), clipText.length());

		SaveUndoAction(SHexEditAction::paste, nPasteAdr, m_xData.Mid(nPasteAdr, nReplaceLength), pasteData);

		if (nReplaceLength > 0)
			m_xData.Remove(nPasteAdr, nReplaceLength);

		m_xData.Insert(nPasteAdr, pasteData);
		m_bRecalc = true;

		SetEditCaretPos(nPasteAdr + pasteData.Size(), true);
		m_nSelectingBeg = NOSECTION_VAL;
		m_nSelectionBegin = NOSECTION_VAL;
		m_nSelectingEnd = NOSECTION_VAL;
		m_nSelectionEnd = NOSECTION_VAL;

		Invalidate();

		EventHexEditDataChanged evt(this);
		FireEvent(evt);
	}

	void SHexEdit::OnEditCut()
	{
		if (m_bReadOnly)
			return;

		if (IsSelection()) {
			UINT nCutLength = m_nSelectionEnd - m_nSelectionBegin + 1;
			if (m_nSelectionEnd >= GetDataSize())
				nCutLength--;

			if (nCutLength > 0)
			{
				SaveUndoAction(SHexEditAction::cut, m_nSelectionBegin, m_xData.Mid(m_nSelectionBegin, nCutLength), SByteArray());

				m_xData.Remove(m_nSelectionBegin, nCutLength);
				m_bRecalc = true;

				SetEditCaretPos(m_nSelectionBegin, true);
				m_nSelectingBeg = NOSECTION_VAL;
				m_nSelectionBegin = NOSECTION_VAL;
				m_nSelectingEnd = NOSECTION_VAL;
				m_nSelectionEnd = NOSECTION_VAL;

				Invalidate();
				EventHexEditDataChanged evt(this);
				FireEvent(evt);
			}
		}
	}

	bool SHexEdit::PrepareReplace(UINT pos, const SByteArray& oldData, const SByteArray& newData)
	{
		if (oldData.Size() > 0)
			m_xData.Remove(pos, oldData.Size());
		if (newData.Size() > 0)
			m_xData.Insert(pos, newData);
		
		if (oldData.Size() != newData.Size()) {
			m_bRecalc = true;
		}
		//m_bRecalc = true;
		MakeVisible(pos + newData.Size(), pos + newData.Size(), true);
		SetSelection(NOSECTION_VAL, NOSECTION_VAL, true, false);
		SetEditCaretPos(pos + newData.Size(), true);
		Invalidate();

		EventHexEditDataChanged evt(this);
		FireEvent(evt);

		return true;
	}

	void SHexEdit::OnEditClear()
	{
		OnDelete(VK_DELETE);
	}
	void SHexEdit::OnEditSelectAll()
	{
		if (GetDataSize() > 0) {
			SetSelection(0, GetDataSize() - 1, false, true);
		}
	}

	void SHexEdit::OnDelete(WPARAM wParam)
	{
		if (m_bReadOnly || GetDataSize() == 0)
			return;
		if (IsSelection()) {
			OnEditCut();
		}
		else {
			if (wParam == VK_BACK) {
				if (m_nCurrentAddress == 0)
					return;
				m_nCurrentAddress--;
			}			 
			UINT delAddress = m_nCurrentAddress;
			if (m_bCaretAtLineEnd)
				delAddress++;
			if (delAddress == GetDataSize())
				return;

			SaveUndoAction(SHexEditAction::cut, delAddress, m_xData.Mid(delAddress, 1), SByteArray());

			m_xData.Remove(delAddress, 1);
			MoveCurrentAddress(0, true);
			Invalidate();
			
			EventHexEditDataChanged evt(this);
			FireEvent(evt);
		}
	}

	bool SHexEdit::OnEditInput(WORD nInput)
	{
		if ((nInput > 255) || m_bReadOnly) {
			return false;
		}

		BYTE nValue = 255;
		char nKey = (char)tolower((char)nInput);
		if (m_bCaretAscii) {
			if (nInput <= 256)
				nValue = (BYTE)nInput;
		}
		else if ((nKey >= 'a') && (nKey <= 'f')) {
			nValue = nKey - (BYTE)'a' + (BYTE)0xa;
		}
		else if ((nKey >= '0') && (nKey <= '9')) {
			nValue = nKey - (BYTE)'0';
		}
		else
			return false;


		UINT replaceLen = 0;
		if (m_nCurrentAddress >= GetDataSize() || (!m_overwriteMode && (m_bCaretAscii || m_bHighBits))) {
			if (m_nCurrentAddress >= GetDataSize())
				m_bHighBits = true;
			m_xData.Insert(m_nCurrentAddress, 0);
			m_bRecalc = true;
		}
		else if (m_nCurrentAddress < GetDataSize()) { // we are overwriting 1 char
			replaceLen = 1;
		}
		if (!m_bCaretAscii)
			if (m_bHighBits)
				nValue = (nValue << 4) | (m_xData[m_nCurrentAddress] & 0x0f);
			else
				nValue = (m_xData[m_nCurrentAddress] & 0xf0) | nValue;
		SaveUndoAction(SHexEditAction::input, m_nCurrentAddress, m_xData.Mid(m_nCurrentAddress, replaceLen), SByteArray(&nValue, 1));
		m_xData[m_nCurrentAddress] = nValue;
		if (m_bCaretAscii)
			MoveCurrentAddress(1, true, true);
		else if (m_bHighBits)
			MoveCurrentAddress(0, false, true);
		else
			MoveCurrentAddress(1, true, true);
		Invalidate();

		EventHexEditDataChanged evt(this);
		FireEvent(evt);
		return true;
	}

	void SHexEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		bool bIsControl = (GetKeyState(VK_CONTROL) & 0x80000000) == 0x80000000;
		if (bIsControl)
			return;

		if (nChar != VK_TAB)
			OnEditInput((WORD)nChar);
	}

	BOOL SHexEdit::PressKeyDown(UINT nChar)
	{
		bool bIsShift = (GetKeyState(VK_SHIFT) & 0x80000000) == 0x80000000;
		bool bIsControl = (GetKeyState(VK_CONTROL) & 0x80000000) == 0x80000000;

		if (bIsControl)
		{
			switch (nChar) {
			case 'X': // ctrl + x: cut
				OnEditCut();
				return TRUE;
			case 'C': // ctrl + c: copy
				OnEditCopy();
				return TRUE;
			case 'V': // ctrl + v: paste
				OnEditPaste();
				return TRUE;
			case 'A': // ctrl + a: select all
				OnEditSelectAll();
				return TRUE;
			case 'Z':
				Undo();
				return TRUE;
			case 'Y':
				Redo();
				return TRUE;
			case VK_HOME:
				MoveCurrentAddress(-0x8000000, true);
				return TRUE;
			case VK_END:
				MoveCurrentAddress(0x7ffffff, true);
				return TRUE;
			}
		}
		else
		{
			if (bIsShift && (m_nSelectingBeg == NOSECTION_VAL)) {
				// start with selecting
				m_nSelectingBeg = m_nCurrentAddress;
			}
			switch (nChar) {
			case VK_DOWN:
				MoveCurrentAddress(m_tPaintDetails.nBytesPerRow, m_bHighBits);
				return TRUE;
			case VK_UP:
				if (m_nCurrentAddress > m_tPaintDetails.nBytesPerRow)
					MoveCurrentAddress(-(int)m_tPaintDetails.nBytesPerRow, m_bHighBits);
				return TRUE;
			case VK_RIGHT:
				if (IsSelection()) {
					MoveCurrentAddress(m_nSelectionEnd - m_nCurrentAddress + 1, 1);
				}
				else if (m_bCaretAscii) {
					MoveCurrentAddress(1, true);
				}
				else if (m_bHighBits) {
					// offset stays the same, caret moves to low-byte
					MoveCurrentAddress(0, false);
				}
				else {
					MoveCurrentAddress(1, true);
				}
				return TRUE;
			case VK_LEFT:
				if (IsSelection()) {
					MoveCurrentAddress(m_nSelectionBegin - m_nCurrentAddress - 1, m_nSelectionBegin == 0);
				}
				else if (m_bCaretAscii) {
					MoveCurrentAddress(-1, true);
				}
				else if (!m_bHighBits && !m_bCaretAtLineEnd) {
					// offset stays the same, caret moves to high-byte
					MoveCurrentAddress(0, true);
				}
				else {
					MoveCurrentAddress(-1, false);
				}
				return TRUE;
			case VK_PRIOR:
				MoveCurrentAddress(-(int)(m_tPaintDetails.nBytesPerRow * (m_tPaintDetails.nVisibleLines - 1)), m_bHighBits);
				return TRUE;
			case VK_NEXT:
				MoveCurrentAddress(m_tPaintDetails.nBytesPerRow * (m_tPaintDetails.nVisibleLines - 1), m_bHighBits);
				return TRUE;
			case VK_HOME:
				if (m_nCurrentAddress == GetDataSize())
					m_nCurrentAddress--;
				MoveCurrentAddress(-(int)((m_nCurrentAddress) % m_tPaintDetails.nBytesPerRow), true);
				return TRUE;
			case VK_END:
				m_bCaretAtLineEnd = true;
				MoveCurrentAddress(m_tPaintDetails.nBytesPerRow - 1 - (m_nCurrentAddress % m_tPaintDetails.nBytesPerRow), false);
				return TRUE;
			case VK_INSERT:
				m_overwriteMode = !m_overwriteMode;
				SetEditCaretPos(m_nCurrentAddress, m_bHighBits);
				Invalidate();
				return TRUE;

			case VK_TAB:
				if (m_bShowAscii && !bIsShift) {
					m_bCaretAscii = !m_bCaretAscii;
					SetEditCaretPos(m_nCurrentAddress, m_bHighBits);
					Invalidate();
				}
				return TRUE;

			case VK_DELETE:
			case VK_BACK:
				OnDelete(nChar);
				return TRUE;
			}
		}

		return FALSE;
	}

	void SHexEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		SetMsgHandled(PressKeyDown(nChar));
	}

	void SHexEdit::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if (GetKeyState(VK_MENU) & 0x8000) OnKeyDown(nChar, nRepCnt, nFlags);
		else SetMsgHandled(FALSE);
	}

	HRESULT SHexEdit::OnAttrAddrBkColor(const SStringW& strValue, BOOL bLoading)
	{
		m_tAdrBkgCol = GETCOLOR(strValue);
		if (!bLoading)
		{
		}
		return S_OK;
	}

	HRESULT SHexEdit::OnAttrHexBkColor(const SStringW& strValue, BOOL bLoading)
	{
		m_tHexBkgCol = GETCOLOR(strValue);
		if (!bLoading)
		{
		}
		return S_OK;
	}

	HRESULT SHexEdit::OnAttrAsciiBkColor(const SStringW& strValue, BOOL bLoading)
	{
		m_tAsciiBkgCol = GETCOLOR(strValue);
		if (!bLoading)
		{
		}
		return S_OK;
	}

	BOOL SHexEdit::SaveUndoAction(UINT type, UINT position, const SByteArray& replaceData, const SByteArray& insertData)
	{
		delete m_redo; // m_pData will change invalidating m_redo
		m_redo = NULL;
		if (type == SHexEditAction::input && m_undo != 0 && m_undo->getType() == SHexEditAction::input &&
			m_undo->getPosition() + m_undo->getInsertLen() == position) {
			// merge input actions
			if (!m_undo->append(replaceData, insertData)) {
				//cant save undo
				return FALSE;
			}
		}
		else {
			// new action can't be merged with previous
			SHexEditAction* action = new SHexEditAction();
			if (action == NULL ||
				!action->set(SHexEditAction::input, position, replaceData, insertData, m_undo)) {
				delete action;
				//cant save undo
				return FALSE;
			}
			m_undo = action;
		}
		return TRUE;
	}

	void SHexEdit::Undo()
	{
		if (!CanUndo())
			return;

		SHexEditAction* action = m_undo;		

		// replace insertData/insertLen at position with replaceData/replaceLen
		if (!PrepareReplace(action->getPosition(), action->getInsertData(), action->getReplaceData())) {
			return;
		}		

		// move head of m_undo to m_redo
		m_undo = action->getNext();
		action->setNext(m_redo);
		m_redo = action;
	}

	void SHexEdit::Redo()
	{
		if (!CanRedo())
			return;

		SHexEditAction* action = m_redo;

		// replace replaceData/replaceLen at position with insertData/insertLen
		if (!PrepareReplace(action->getPosition(), action->getReplaceData(), action->getInsertData())) {
			// cant redo
			return;
		}		

		// move head of m_redo to m_undo
		m_redo = action->getNext();
		action->setNext(m_undo);
		m_undo = action;
	}
}