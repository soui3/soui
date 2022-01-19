// ColourPopup.cpp : implementation file
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Extended by Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
// Copyright (c) 1998.
//
// Updated 30 May 1998 to allow any number of colours, and to
//                     make the appearance closer to Office 97. 
//                     Also added "Default" text area.         (CJM)
//
//         13 June 1998 Fixed change of focus bug (CJM)
//         30 June 1998 Fixed bug caused by focus bug fix (D'oh!!)
//                      Solution suggested by Paul Wilkerson.
//
// ColourPopup is a helper class for the colour picker control
// CColourPicker. Check out the header file or the accompanying 
// HTML doc file for details.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Expect bugs.
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file. 

#include "stdafx.h"
#include <math.h>
#include "ColourPopup.h"
#include <ShellAPI.h>
#include <Commdlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEFAULT_BOX_VALUE -3
#define CUSTOM_BOX_VALUE  -2
#define INVALID_COLOUR    -1


namespace SOUI
{
    // To hold the colours and their names
    struct ColourTableEntry{
        COLORREF crColour;
        const TCHAR    *szName;
    } g_colorTable[] = 
{
  /*  { RGB(0x00, 0x00, 0x00),    _T("Black")             },
    { RGB(0xA5, 0x2A, 0x00),    _T("Brown")             },
    { RGB(0x00, 0x40, 0x40),    _T("Dark Olive Green")  },
    { RGB(0x00, 0x55, 0x00),    _T("Dark Green")        },
    { RGB(0x00, 0x00, 0x5E),    _T("Dark Teal")         },
    { RGB(0x00, 0x00, 0x8B),    _T("Dark blue")         },
    { RGB(0x4B, 0x00, 0x82),    _T("Indigo")            },
    { RGB(0x28, 0x28, 0x28),    _T("Dark grey")         },

    { RGB(0x8B, 0x00, 0x00),    _T("Dark red")          },
    { RGB(0xFF, 0x68, 0x20),    _T("Orange")            },
    { RGB(0x8B, 0x8B, 0x00),    _T("Dark yellow")       },
    { RGB(0x00, 0x93, 0x00),    _T("Green")             },
    { RGB(0x38, 0x8E, 0x8E),    _T("Teal")              },
    { RGB(0x00, 0x00, 0xFF),    _T("Blue")              },
    { RGB(0x7B, 0x7B, 0xC0),    _T("Blue-grey")         },
    { RGB(0x66, 0x66, 0x66),    _T("Grey - 40")         },

    { RGB(0xFF, 0x00, 0x00),    _T("Red")               },
    { RGB(0xFF, 0xAD, 0x5B),    _T("Light orange")      },
    { RGB(0x32, 0xCD, 0x32),    _T("Lime")              }, 
    { RGB(0x3C, 0xB3, 0x71),    _T("Sea green")         },
    { RGB(0x7F, 0xFF, 0xD4),    _T("Aqua")              },
    { RGB(0x7D, 0x9E, 0xC0),    _T("Light blue")        },
    { RGB(0x80, 0x00, 0x80),    _T("Violet")            },
    { RGB(0x7F, 0x7F, 0x7F),    _T("Grey - 50")         },

    { RGB(0xFF, 0xC0, 0xCB),    _T("Pink")              },
    { RGB(0xFF, 0xD7, 0x00),    _T("Gold")              },
    { RGB(0xFF, 0xFF, 0x00),    _T("Yellow")            },    
    { RGB(0x00, 0xFF, 0x00),    _T("Bright green")      },
    { RGB(0x40, 0xE0, 0xD0),    _T("Turquoise")         },
    { RGB(0xC0, 0xFF, 0xFF),    _T("Skyblue")           },
    { RGB(0x48, 0x00, 0x48),    _T("Plum")              },
    { RGB(0xC0, 0xC0, 0xC0),    _T("Light grey")        },

    { RGB(0xFF, 0xE4, 0xE1),    _T("Rose")              },
    { RGB(0xD2, 0xB4, 0x8C),    _T("Tan")               },
    { RGB(0xFF, 0xFF, 0xE0),    _T("Light yellow")      },
    { RGB(0x98, 0xFB, 0x98),    _T("Pale green ")       },
    { RGB(0xAF, 0xEE, 0xEE),    _T("Pale turquoise")    },
    { RGB(0x68, 0x83, 0x8B),    _T("Pale blue")         },
    { RGB(0xE6, 0xE6, 0xFA),    _T("Lavender")          },
    { RGB(0xFF, 0xFF, 0xFF),    _T("White")             }*/

	{RGB(0x00,0x00,0x00),	_T("黑色")		},
	{RGB(0xA5,0x2A,0x00),	_T("棕色")		},
	{RGB(0x00,0x40,0x40),	_T("橄榄绿色")	},
	{RGB(0x00,0x55,0x00),	_T("暗绿色")	},
	{RGB(0x00,0x00,0x5E),	_T("深青色")	},
	{RGB(0x00,0x00,0x8B),	_T("深蓝色")	},
	{RGB(0x4B,0x00,0x82),	_T("深紫蓝色")	},
	{RGB(0x28,0x28,0x28),	_T("深灰色")	},

	{RGB(0x8B,0x00,0x00),	_T("深红色")	},
	{RGB(0xFF,0x68,0x20),	_T("桔红色")	},
	{RGB(0x8B,0x8B,0x00),	_T("深黄色")	},
	{RGB(0x00,0x93,0x00),	_T("绿色")		},
	{RGB(0x38,0x8E,0x8E),	_T("青色")		},
	{RGB(0x00,0x00,0xFF),	_T("蓝色")		},
	{RGB(0x7B,0x7B,0xC0),	_T("绿灰色")	},
	{RGB(0x66,0x66,0x66),	_T("灰色")		},

	{RGB(0xFF,0x00,0x00),	_T("红色")		},
	{RGB(0xFF,0xAD,0x5B),	_T("淡桔红色")	},
	{RGB(0x32,0xCD,0x32),	_T("浅绿色")	},
	{RGB(0x3C,0xB3,0x71),	_T("海蓝色")	},
	{RGB(0x7F,0xFF,0xD4),	_T("浅绿色")	},
	{RGB(0x7D,0x9E,0xC0),	_T("淡蓝色")	},
	{RGB(0x80,0x00,0x80),	_T("紫罗兰色")	},
	{RGB(0x7F,0x7F,0x7F),	_T("灰色")		},

	{RGB(0xFF,0xC0,0xCB),	_T("粉红色")	},
	{RGB(0xFF,0xD7,0x00),	_T("金色")		},
	{RGB(0xFF,0xFF,0x00),	_T("黄色")		},
	{RGB(0x00,0xFF,0x00),	_T("亮绿色")	},
	{RGB(0x40,0xE0,0xD0),	_T("绿松色")	},
	{RGB(0xC0,0xFF,0xFF),	_T("天蓝色")	},
	{RGB(0x48,0x00,0x48),	_T("梅红")	},
	{RGB(0xC0,0xC0,0xC0),	_T("淡灰色")	},

	{RGB(0xFF,0xE4,0xE1),	_T("玫瑰色")	},
	{RGB(0xD2,0xB4,0x8C),	_T("棕褐色")	},
	{RGB(0xFF,0xFF,0xE0),	_T("淡黄色")	},
	{RGB(0x98,0xFB,0x98),	_T("浅绿色")	},
	{RGB(0xAF,0xEE,0xEE),	_T("浅绿宝石色")},
	{RGB(0x68,0x83,0x8B),	_T("浅蓝色")	},
	{RGB(0xE6,0xE6,0xFA),	_T("淡紫色")	},
	{RGB(0xFF,0xFF,0xFF),	_T("白色")		},
};

const    int            KNumColours = sizeof(g_colorTable)/sizeof(ColourTableEntry);

/////////////////////////////////////////////////////////////////////////////
// CColourPopup

CColourPopup::CColourPopup(HWND hOwner,IColorPicker *pColorPicker)
:m_hOwner(hOwner)
,m_pColorPicker(pColorPicker)
{
    Initialise();
    m_pColorPicker->GetMsgLoop()->AddMessageFilter(this);
}

void CColourPopup::Initialise()
{
    m_nNumColumns       = 0;
    m_nNumRows          = 0;
    m_nBoxSize          = 18;
    m_nMargin           = ::GetSystemMetrics(SM_CXEDGE);
    m_nCurrentSel       = INVALID_COLOUR;
    m_nChosenColourSel  = INVALID_COLOUR;
    m_crColour          = m_crInitialColour = RGB(0,0,0);
	m_crDef				= CLR_DEFAULT;

    // Idiot check: Make sure the colour square is at least 5 x 5;
    if (m_nBoxSize - 2*m_nMargin - 2 < 5) m_nBoxSize = 5 + 2*m_nMargin + 2;

    // Create the font
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
    m_Font=CreateFontIndirect(&(ncm.lfMessageFont));

}

CColourPopup::~CColourPopup()
{
    m_pColorPicker->GetMsgLoop()->RemoveMessageFilter(this);
    DeleteObject(m_Font);
	m_pColorPicker = NULL;
}

BOOL CColourPopup::Create(CPoint p, COLORREF crColour,
                          LPCTSTR szDefaultText /* = NULL */,
                          LPCTSTR szCustomText  /* = NULL */)
{
    m_crColour = m_crInitialColour = crColour;

	if (!SNativeWnd::CreateWindow(NULL, WS_VISIBLE|WS_POPUP, WS_EX_TOOLWINDOW,
                        p.x, p.y, 100, 100, // size updated soon
                        m_hOwner, NULL))
        return FALSE;

    // Store the Custom text
    if (szCustomText != NULL) 
        m_strCustomText = szCustomText;

    // Store the Default Area text
    if (szDefaultText != NULL) 
        m_strDefaultText = szDefaultText;
        
    // Set the window size
    SetWindowSize();

    // Create the tooltips
    CreateToolTips();

    // Find which cell (if any) corresponds to the initial colour
    FindCellFromColour(crColour);

    // Capture all mouse events for the life of this window
    SetCapture();

    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CColourPopup message handlers

// For tooltips
BOOL CColourPopup::PreTranslateMessage(MSG* pMsg) 
{
    m_ToolTip.RelayEvent(pMsg);
	return FALSE;
}

// If an arrow key is pressed, then move the selection
void CColourPopup::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    int row = GetRow(m_nCurrentSel),
        col = GetColumn(m_nCurrentSel);

    if (nChar == VK_DOWN) 
    {
        if (row == DEFAULT_BOX_VALUE) 
            row = col = 0; 
        else if (row == CUSTOM_BOX_VALUE)
        {
            if (m_strDefaultText.GetLength())
                row = col = DEFAULT_BOX_VALUE;
            else
                row = col = 0;
        }
        else
        {
            row++;
            if (GetIndex(row,col) < 0)
            {
                if (m_strCustomText.GetLength())
                    row = col = CUSTOM_BOX_VALUE;
                else if (m_strDefaultText.GetLength())
                    row = col = DEFAULT_BOX_VALUE;
                else
                    row = col = 0;
            }
        }
        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_UP) 
    {
        if (row == DEFAULT_BOX_VALUE)
        {
            if (m_strCustomText.GetLength())
                row = col = CUSTOM_BOX_VALUE;
            else
           { 
                row = GetRow(KNumColours-1); 
                col = GetColumn(KNumColours-1); 
            }
        }
        else if (row == CUSTOM_BOX_VALUE)
        { 
            row = GetRow(KNumColours-1); 
            col = GetColumn(KNumColours-1); 
        }
        else if (row > 0) row--;
        else /* row == 0 */
        {
            if (m_strDefaultText.GetLength())
                row = col = DEFAULT_BOX_VALUE;
            else if (m_strCustomText.GetLength())
                row = col = CUSTOM_BOX_VALUE;
            else
            { 
                row = GetRow(KNumColours-1); 
                col = GetColumn(KNumColours-1); 
            }
        }
        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_RIGHT) 
    {
        if (row == DEFAULT_BOX_VALUE) 
            row = col = 0; 
        else if (row == CUSTOM_BOX_VALUE)
        {
            if (m_strDefaultText.GetLength())
                row = col = DEFAULT_BOX_VALUE;
            else
                row = col = 0;
        }
        else if (col < m_nNumColumns-1) 
            col++;
        else 
        { 
            col = 0; row++;
        }

        if (GetIndex(row,col) == INVALID_COLOUR)
        {
            if (m_strCustomText.GetLength())
                row = col = CUSTOM_BOX_VALUE;
            else if (m_strDefaultText.GetLength())
                row = col = DEFAULT_BOX_VALUE;
            else
                row = col = 0;
        }

        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_LEFT) 
    {
        if (row == DEFAULT_BOX_VALUE)
        {
            if (m_strCustomText.GetLength())
                row = col = CUSTOM_BOX_VALUE;
            else
           { 
                row = GetRow(KNumColours-1); 
                col = GetColumn(KNumColours-1); 
            }
        }
        else if (row == CUSTOM_BOX_VALUE)
        { 
            row = GetRow(KNumColours-1); 
            col = GetColumn(KNumColours-1); 
        }
        else if (col > 0) col--;
        else /* col == 0 */
        {
            if (row > 0) { row--; col = m_nNumColumns-1; }
            else 
            {
                if (m_strDefaultText.GetLength())
                    row = col = DEFAULT_BOX_VALUE;
                else if (m_strCustomText.GetLength())
                    row = col = CUSTOM_BOX_VALUE;
                else
                { 
                    row = GetRow(KNumColours-1); 
                    col = GetColumn(KNumColours-1); 
                }
            }
        }
        ChangeSelection(GetIndex(row, col));
    }

    if (nChar == VK_ESCAPE) 
    {
        m_crColour = m_crInitialColour;
        EndSelection(IDOK);
        return;
    }

    if (nChar == VK_RETURN || nChar == VK_SPACE)
    {
        EndSelection(IDCANCEL);
        return;
    }

}

void CColourPopup::OnPaint(HDC ) 
{
    PAINTSTRUCT ps;
    HDC hdc=::BeginPaint(m_hWnd,&ps);

    // Draw the Default Area text
    if (m_strDefaultText.GetLength())
        DrawCell(hdc, DEFAULT_BOX_VALUE);
 
    // Draw colour cells
    for (int i = 0; i < KNumColours; i++)
        DrawCell(hdc, i);
    
    // Draw custom text
    if (m_strCustomText.GetLength())
        DrawCell(hdc, CUSTOM_BOX_VALUE);

    // Draw raised window edge (ex-window style WS_EX_WINDOWEDGE is sposed to do this,
    // but for some reason isn't
    CRect rect;
    GetClientRect(rect);
    ::DrawEdge(hdc,rect, EDGE_RAISED, BF_RECT);

    ::EndPaint(m_hWnd,&ps);
}

void CColourPopup::OnMouseMove(UINT nFlags, CPoint point) 
{
    int nNewSelection = INVALID_COLOUR;

    // Translate points to be relative raised window edge
    point.x -= m_nMargin;
    point.y -= m_nMargin;

    // First check we aren't in text box
    if (m_strCustomText.GetLength() && m_CustomTextRect.PtInRect(point))
        nNewSelection = CUSTOM_BOX_VALUE;
    else if (m_strDefaultText.GetLength() && m_DefaultTextRect.PtInRect(point))
        nNewSelection = DEFAULT_BOX_VALUE;
    else
    {
        // Take into account text box
        if (m_strDefaultText.GetLength()) 
            point.y -= m_DefaultTextRect.Height();  

        // Get the row and column
        nNewSelection = GetIndex(point.y / m_nBoxSize, point.x / m_nBoxSize);

        // In range? If not, default and exit
        if (nNewSelection < 0 || nNewSelection >= KNumColours)
        {
            return;
        }
    }

    // OK - we have the row and column of the current selection (may be CUSTOM_BOX_VALUE)
    // Has the row/col selection changed? If yes, then redraw old and new cells.
    if (nNewSelection != m_nCurrentSel)
        ChangeSelection(nNewSelection);

}

// End selection on LButtonUp
void CColourPopup::OnLButtonUp(UINT nFlags, CPoint point) 
{    
	ClientToScreen(&point);
    if (m_WindowRect.PtInRect(point))
        EndSelection(IDOK);
    else
        EndSelection(IDCANCEL);
}

/////////////////////////////////////////////////////////////////////////////
// CColourPopup implementation

int CColourPopup::GetIndex(int row, int col) const
{ 
    if ((row == CUSTOM_BOX_VALUE || col == CUSTOM_BOX_VALUE) && m_strCustomText.GetLength())
        return CUSTOM_BOX_VALUE;
    else if ((row == DEFAULT_BOX_VALUE || col == DEFAULT_BOX_VALUE) && m_strDefaultText.GetLength())
        return DEFAULT_BOX_VALUE;
    else if (row < 0 || col < 0 || row >= m_nNumRows || col >= m_nNumColumns)
        return INVALID_COLOUR;
    else
    {
        if (row*m_nNumColumns + col >= KNumColours)
            return INVALID_COLOUR;
        else
            return row*m_nNumColumns + col;
    }
}

int CColourPopup::GetRow(int nIndex) const               
{ 
    if (nIndex == CUSTOM_BOX_VALUE && m_strCustomText.GetLength())
        return CUSTOM_BOX_VALUE;
    else if (nIndex == DEFAULT_BOX_VALUE && m_strDefaultText.GetLength())
        return DEFAULT_BOX_VALUE;
    else if (nIndex < 0 || nIndex >= KNumColours)
        return INVALID_COLOUR;
    else
        return nIndex / m_nNumColumns; 
}

int CColourPopup::GetColumn(int nIndex) const            
{ 
    if (nIndex == CUSTOM_BOX_VALUE && m_strCustomText.GetLength())
        return CUSTOM_BOX_VALUE;
    else if (nIndex == DEFAULT_BOX_VALUE && m_strDefaultText.GetLength())
        return DEFAULT_BOX_VALUE;
    else if (nIndex < 0 || nIndex >= KNumColours)
        return INVALID_COLOUR;
    else
        return nIndex % m_nNumColumns; 
}

void CColourPopup::FindCellFromColour(COLORREF crColour)
{
    if (crColour == m_crDef && m_strDefaultText.GetLength())
    {
        m_nChosenColourSel = DEFAULT_BOX_VALUE;
        return;
    }

    for (int i = 0; i < KNumColours; i++)
    {
        if (GetColour(i) == crColour)
        {
            m_nChosenColourSel = i;
            return;
        }
    }

    if (m_strCustomText.GetLength())
        m_nChosenColourSel = CUSTOM_BOX_VALUE;
    else
        m_nChosenColourSel = INVALID_COLOUR;
}

// Gets the dimensions of the colour cell given by (row,col)
BOOL CColourPopup::GetCellRect(int nIndex, const LPRECT& rect)
{
    if (nIndex == CUSTOM_BOX_VALUE)
    {
        ::SetRect(rect, 
                  m_CustomTextRect.left,  m_CustomTextRect.top,
                  m_CustomTextRect.right, m_CustomTextRect.bottom);
        return TRUE;
    }
    else if (nIndex == DEFAULT_BOX_VALUE)
    {
        ::SetRect(rect, 
                  m_DefaultTextRect.left,  m_DefaultTextRect.top,
                  m_DefaultTextRect.right, m_DefaultTextRect.bottom);
        return TRUE;
    }

    if (nIndex < 0 || nIndex >= KNumColours)
        return FALSE;

    rect->left = GetColumn(nIndex) * m_nBoxSize + m_nMargin;
    rect->top  = GetRow(nIndex) * m_nBoxSize + m_nMargin;

    // Move everything down if we are displaying a default text area
    if (m_strDefaultText.GetLength()) 
        rect->top += (m_nMargin + m_DefaultTextRect.Height());

    rect->right = rect->left + m_nBoxSize;
    rect->bottom = rect->top + m_nBoxSize;

    return TRUE;
}

BOOL GetTextExtent(HDC hdc,LPCTSTR lpszString, int nCount, LPSIZE lpSize)
{
    if(nCount == -1)
        nCount = lstrlen(lpszString);
    return ::GetTextExtentPoint32(hdc, lpszString, nCount, lpSize);
}

// Works out an appropriate size and position of this window
void CColourPopup::SetWindowSize()
{
    CSize TextSize;

    // If we are showing a custom or default text area, get the font and text size.
    if (m_strCustomText.GetLength() || m_strDefaultText.GetLength())
    {
        HDC hdc = GetDC();
        HGDIOBJ pOldFont =  SelectObject(hdc,m_Font);

        // Get the size of the custom text (if there IS custom text)
        TextSize = CSize(0,0);
        if (m_strCustomText.GetLength())
            GetTextExtent(hdc,m_strCustomText,m_strCustomText.GetLength(),&TextSize);

        // Get the size of the default text (if there IS default text)
        if (m_strDefaultText.GetLength())
        {
            CSize DefaultSize;
			GetTextExtent(hdc,m_strDefaultText,m_strDefaultText.GetLength(),&DefaultSize);
            if (DefaultSize.cx > TextSize.cx) TextSize.cx = DefaultSize.cx;
            if (DefaultSize.cy > TextSize.cy) TextSize.cy = DefaultSize.cy;
        }

        SelectObject(hdc,pOldFont);
        TextSize += CSize(2*m_nMargin,2*m_nMargin);

        // Add even more space to draw the horizontal line
        TextSize.cy += 2*m_nMargin + 2;

        ReleaseDC(hdc);
    }

    // Get the number of columns and rows
    //m_nNumColumns = (int) sqrt((double)m_nNumColours);    // for a square window (yuk)
    m_nNumColumns = 8;
    m_nNumRows = KNumColours / m_nNumColumns;
    if (KNumColours % m_nNumColumns) m_nNumRows++;

    // Get the current window position, and set the new size
    CRect rect;
    GetWindowRect(rect);

    m_WindowRect.SetRect(rect.left, rect.top, 
                         rect.left + m_nNumColumns*m_nBoxSize + 2*m_nMargin,
                         rect.top  + m_nNumRows*m_nBoxSize + 2*m_nMargin);

    // if custom text, then expand window if necessary, and set text width as
    // window width
    if (m_strDefaultText.GetLength()) 
    {
        if (TextSize.cx > m_WindowRect.Width())
            m_WindowRect.right = m_WindowRect.left + TextSize.cx;
        TextSize.cx = m_WindowRect.Width()-2*m_nMargin;

        // Work out the text area
        m_DefaultTextRect.SetRect(m_nMargin, m_nMargin, 
                                  m_nMargin+TextSize.cx, 2*m_nMargin+TextSize.cy);
        m_WindowRect.bottom += m_DefaultTextRect.Height() + 2*m_nMargin;
    }

    // if custom text, then expand window if necessary, and set text width as
    // window width
    if (m_strCustomText.GetLength()) 
    {
        if (TextSize.cx > m_WindowRect.Width())
            m_WindowRect.right = m_WindowRect.left + TextSize.cx;
        TextSize.cx = m_WindowRect.Width()-2*m_nMargin;

        // Work out the text area
        m_CustomTextRect.SetRect(m_nMargin, m_WindowRect.Height(), 
                                 m_nMargin+TextSize.cx, 
                                 m_WindowRect.Height()+m_nMargin+TextSize.cy);
        m_WindowRect.bottom += m_CustomTextRect.Height() + 2*m_nMargin;
   }

    // Need to check it'll fit on screen: Too far right?
    CSize ScreenSize(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
    if (m_WindowRect.right > ScreenSize.cx)
        m_WindowRect.OffsetRect(-(m_WindowRect.right - ScreenSize.cx), 0);

    // Too far left?
    if (m_WindowRect.left < 0)
        m_WindowRect.OffsetRect( -m_WindowRect.left, 0);

    // Bottom falling out of screen?
    if (m_WindowRect.bottom > ScreenSize.cy)
    {
    }

    // Set the window size and position
    MoveWindow2(m_WindowRect, TRUE);
}

void CColourPopup::CreateToolTips()
{
    // Create the tool tip
    if (!m_ToolTip.Create(m_hWnd,0)) return;
	
    // Add a tool for each cell
    for (int i = 0; i < KNumColours; i++)
    {
        CRect rect;
         if (!GetCellRect(i, rect)) continue;
         m_ToolTip.AddTool(m_hWnd, GetColourName(i), rect, 1);
    }
}

void CColourPopup::ChangeSelection(int nIndex)
{
    HDC hdc = GetDC();

    if (nIndex > KNumColours)
        nIndex = CUSTOM_BOX_VALUE; 

    if ((m_nCurrentSel >= 0 && m_nCurrentSel < KNumColours) ||
        m_nCurrentSel == CUSTOM_BOX_VALUE || m_nCurrentSel == DEFAULT_BOX_VALUE)
    {
        // Set Current selection as invalid and redraw old selection (this way
        // the old selection will be drawn unselected)
        int OldSel = m_nCurrentSel;
        m_nCurrentSel = INVALID_COLOUR;
        DrawCell(hdc, OldSel);
    }

    // Set the current selection as row/col and draw (it will be drawn selected)
    m_nCurrentSel = nIndex;
    DrawCell(hdc, m_nCurrentSel);

    // Store the current colour
     if (m_nCurrentSel == CUSTOM_BOX_VALUE) m_crColour=m_crInitialColour;
	 else if(m_nCurrentSel == DEFAULT_BOX_VALUE) m_crColour=m_crDef;
	 else m_crColour=GetColour(m_nCurrentSel);
	 if(m_pColorPicker)
		m_pColorPicker->OnColorChanged(m_crColour);

     ReleaseDC(hdc);
}

void CColourPopup::EndSelection(int nMessage)
{
    ReleaseCapture();

    // If custom text selected, perform a custom colour selection
    if (nMessage != IDCANCEL && m_nCurrentSel == CUSTOM_BOX_VALUE)
    {
		ShowWindow(SW_HIDE);
		MyChooseColor(m_crColour);
    }

    if (nMessage == IDCANCEL)
        m_crColour = m_crInitialColour;

	if(m_pColorPicker)
		m_pColorPicker->OnColorEnd(nMessage==IDCANCEL,m_crColour);

    DestroyWindow();
}


void FillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF clr)
{
    COLORREF clrOld = ::SetBkColor(hdc, clr);
    if(clrOld != CLR_INVALID)
    {
        ::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
        ::SetBkColor(hdc, clrOld);
    }
}

void FillSolidRect(HDC hdc, int x,int y,int w,int h, COLORREF clr)
{
    RECT rc={x,y,x+w,y+h};
    FillSolidRect(hdc,&rc,clr);
}

void Rectangle(HDC hdc, LPCRECT pRc)
{
    ::Rectangle(hdc,pRc->left,pRc->top,pRc->right,pRc->bottom);
}

void CColourPopup::DrawCell(HDC hdc, int nIndex)
{
    // For the Custom Text area
    if (m_strCustomText.GetLength() && nIndex == CUSTOM_BOX_VALUE)
    {
        // The extent of the actual text button
        CRect TextButtonRect = m_CustomTextRect;
        TextButtonRect.top += 2*m_nMargin;

        // Fill background
        FillSolidRect(hdc,TextButtonRect, ::GetSysColor(COLOR_3DFACE));

        // Draw horizontal line
        FillSolidRect(hdc,m_CustomTextRect.left+2*m_nMargin, m_CustomTextRect.top,
                           m_CustomTextRect.Width()-4*m_nMargin, 1, ::GetSysColor(COLOR_3DSHADOW));
        FillSolidRect(hdc,m_CustomTextRect.left+2*m_nMargin, m_CustomTextRect.top+1,
                           m_CustomTextRect.Width()-4*m_nMargin, 1, ::GetSysColor(COLOR_3DHILIGHT));

        TextButtonRect.DeflateRect(1,1);
        // fill background
        if (m_nChosenColourSel == nIndex && m_nCurrentSel != nIndex)
            FillSolidRect(hdc,TextButtonRect, ::GetSysColor(COLOR_3DLIGHT));
        else
            FillSolidRect(hdc,TextButtonRect, ::GetSysColor(COLOR_3DFACE));

        // Draw button
        if (m_nCurrentSel == nIndex) 
            DrawEdge(hdc,TextButtonRect, BDR_RAISEDINNER, BF_RECT);
        else if (m_nChosenColourSel == nIndex)
            DrawEdge(hdc,TextButtonRect, BDR_SUNKENOUTER, BF_RECT);

        // Draw custom text
        HGDIOBJ pOldFont= SelectObject(hdc,m_Font);
        SetBkMode(hdc,TRANSPARENT);
        DrawText(hdc,m_strCustomText,m_strCustomText.GetLength(), TextButtonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc,pOldFont);

        return;
    }        

    // For the Default Text area
    if (m_strDefaultText.GetLength() && nIndex == DEFAULT_BOX_VALUE)
    {
        // Fill background
        FillSolidRect(hdc,m_DefaultTextRect, ::GetSysColor(COLOR_3DFACE));

        // The extent of the actual text button
        CRect TextButtonRect = m_DefaultTextRect;
        TextButtonRect.DeflateRect(1,1);

        // fill background
        if (m_nChosenColourSel == nIndex && m_nCurrentSel != nIndex)
            FillSolidRect(hdc,TextButtonRect, ::GetSysColor(COLOR_3DLIGHT));
        else
            FillSolidRect(hdc,TextButtonRect, ::GetSysColor(COLOR_3DFACE));

        // Draw thin line around text
        CRect LineRect = TextButtonRect;
        LineRect.DeflateRect(2*m_nMargin,2*m_nMargin);
        HPEN pen = CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
        HGDIOBJ pOldPen = SelectObject(hdc,pen);
        SelectObject(hdc,(HBRUSH)GetStockObject(NULL_BRUSH));
        Rectangle(hdc,LineRect);
        SelectObject(hdc,pOldPen);

        // Draw button
        if (m_nCurrentSel == nIndex) 
            DrawEdge(hdc,TextButtonRect, BDR_RAISEDINNER, BF_RECT);
        else if (m_nChosenColourSel == nIndex)
            DrawEdge(hdc,TextButtonRect, BDR_SUNKENOUTER, BF_RECT);

        // Draw custom text
        HFONT pOldFont = (HFONT)SelectObject(hdc,m_Font);
        SetBkMode(hdc,TRANSPARENT);
        DrawText(hdc,m_strDefaultText,m_strDefaultText.GetLength(), TextButtonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc,pOldFont);

        return;
    }        

    CRect rect;
    if (!GetCellRect(nIndex, rect)) return;

    // fill background
    if (m_nChosenColourSel == nIndex && m_nCurrentSel != nIndex)
        FillSolidRect(hdc,rect, ::GetSysColor(COLOR_3DHILIGHT));
    else
        FillSolidRect(hdc,rect, ::GetSysColor(COLOR_3DFACE));

    // Draw button
    if (m_nCurrentSel == nIndex) 
        DrawEdge(hdc,rect, BDR_RAISEDINNER, BF_RECT);
    else if (m_nChosenColourSel == nIndex)
        DrawEdge(hdc,rect, BDR_SUNKENOUTER, BF_RECT);

    HBRUSH brush = CreateSolidBrush(RGB(GetRValue(GetColour(nIndex)), 
                            GetGValue(GetColour(nIndex)), 
                            GetBValue(GetColour(nIndex)) ));
    HPEN   pen= CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));

    HBRUSH pOldBrush =  (HBRUSH)SelectObject(hdc,brush);
    HPEN   pOldPen   =    (HPEN)SelectObject(hdc,pen);

    // Draw the cell colour
    rect.DeflateRect(m_nMargin+1, m_nMargin+1);
    Rectangle(hdc,rect);

    // restore DC and cleanup
    SelectObject(hdc,pOldBrush);
    SelectObject(hdc,pOldPen);
    DeleteObject(brush);
    DeleteObject(pen);

}

void CColourPopup::OnKillFocus(HWND hNewWnd) 
{
    ReleaseCapture();
}


// KillFocus problem fix suggested by Paul Wilkerson.
void CColourPopup::OnActivateApp(BOOL bActive, DWORD hTask) 
{
	DefWindowProc();
	// If Deactivating App, cancel this selection
	if (!bActive)	EndSelection(IDCANCEL);
}

void CColourPopup::OnFinalMessage( HWND hWnd )
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

BOOL CColourPopup::MyChooseColor(COLORREF &cr)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = m_hWnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = cr;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (!ChooseColor(&cc)) return FALSE;

	cr=cc.rgbResult;
	return TRUE;
}

COLORREF CColourPopup::GetColour( int nIndex )
{
    return g_colorTable[nIndex].crColour;
}

LPCTSTR CColourPopup::GetColourName( int nIndex )
{
    return g_colorTable[nIndex].szName;
}

}//end of namespace
