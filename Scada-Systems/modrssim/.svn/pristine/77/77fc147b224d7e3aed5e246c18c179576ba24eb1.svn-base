/****************************************************************************
FileEditCtrl.cpp : implementation file for the CFileEditCtrl control class
written by PJ Arends
pja@telus.net

For updates check http://www3.telus.net/pja/CFileEditCtrl.htm

-----------------------------------------------------------------------------
This code is provided as is, with no warranty as to it's suitability or usefulness
in any application in which it may be used. This code has not been tested for
UNICODE builds, nor has it been tested on a network ( with UNC paths ).

This code may be used in any way you desire. This file may be redistributed by any
means as long as it is not sold for profit, and providing that this notice and the
authors name are included.

If any bugs are found and fixed, a note to the author explaining the problem and
fix would be nice.
-----------------------------------------------------------------------------


****************************************************************************/

#include "stdafx.h"
#include "FileEditCtrl.h"

// see http://www.codeproject.com/tools/imageviewer.asp
//#define ACTIVATE_VIEWER
//#include <ImageViewer.h>

#ifndef __ATLCONV_H__
#include <atlconv.h>   // for T2COLE() macro in DereferenceLinks() function
#endif // __ATLCONV_H__

#ifndef _INC_CDERR
#include <cderr.h>     // for FNERR_BUFFERTOSMALL
#endif // _INC_CDERR

// resource.h is only needed for the string table resources. If you do not put the
// FEC_IDS_* strings in a string table, you do not need to include resource.h
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// These strings should be entered into a string table resource with the 
// FEC_IDS_* identifiers defined here, although this class will work
// properly if they are not.
//
// string usage:
//
// FEC_IDS_ALLFILES         - default file filter for CFileDialog
// FEC_IDS_BUTTONTIP        - Text for the browse button tooltip
// FEC_IDS_FILEDIALOGTITLE  - default dialog caption for CFileDialog
// FEC_IDS_SEPARATOR        - character used to seperate files when FEC_MULTIPLE flag is used
// FEC_IDS_NOFILE           - Error message for DDV_FileEditCtrl() when no files or folders are entered
// FEC_IDS_NOTEXIST         - Error message for DDV_FileEditCtrl() when the specified file or folder could not be found
// FEC_IDS_NOTFILE          - Error message for DDV_FileEditCtrl() when the specified file is actually a folder
// FEC_IDS_NOTFOLDER        - Error message for DDV_FileEditCtrl() when the specified folder is actually a file
// FEC_IDS_OKBUTTON         - Text for the 'OK' (Open) button on CFileDialog
//

// FEC_IDS_ALLFILES will be defined in resource.h if these strings
// are in a string table resource
#if !defined FEC_IDS_ALLFILES
#define FEC_NORESOURCESTRINGS /* so this class knows how to handle these strings */
#define FEC_IDS_ALLFILES        _T("All Files (*.*)|*.*||")
#define FEC_IDS_BUTTONTIP       _T("Browse")
#define FEC_IDS_FILEDIALOGTITLE _T("Browse for File")
#define FEC_IDS_SEPARATOR       _T(";")
#define FEC_IDS_NOFILE          _T("Enter an existing file.")
#define FEC_IDS_NOTEXIST        _T("%s does not exist.")
#define FEC_IDS_NOTFILE         _T("%s is not a file.")
#define FEC_IDS_NOTFOLDER       _T("%s is not a folder.")
#define FEC_IDS_OKBUTTON        _T("OK")
#endif

/////////////////////////////////////////////////////////////////////////////
// Button states
#define BTN_UP          0x01
#define BTN_DOWN        0x02
#define BTN_DISABLED    0x04
#define BTN_FLAT        0x08

/////////////////////////////////////////////////////////////////////////////
// ToolTip IDs
#define ID_BUTTONTIP    1
#define ID_CLIENTTIP    2

/////////////////////////////////////////////////////////////////////////////
// Brush sizes for drawing the button background
#define BRUSHWIDTH      8
#define BRUSHHEIGHT     8

/////////////////////////////////////////////////////////////////////////////
// Helper functions

/////////////////////////////////////////////////////////////////////////////
//
//  IsWindow  (Global function)
//    Checks if the given window is active
//
//  Parameters :
//    pWnd [in] - points to the CWnd object to check
//
//  Returns :
//    TRUE if the window is active
//    FALSE if not
//
/////////////////////////////////////////////////////////////////////////////

BOOL IsWindow(CWnd *pWnd)
{
    if (!pWnd)
        return FALSE;
    return ::IsWindow(pWnd->m_hWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CFileEditCtrl

IMPLEMENT_DYNAMIC(CFileEditCtrl, CEdit)

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl constructor  (public member function)
//    Initializes all the internal variables
//
//  Parameters :
//    bAutoDelete [in] - Auto delete flag
//
//  Returns :
//    Nothing
//
//  Note :
//    If bAutoDelete is TRUE, this class object will be deleted
//    when it's window is destroyed (in CFileEditCtrl::PostNCDestroy).
//    The only time this should be used is when the control is
//    created dynamicly in the
//    DDX_FileEditCtrl(CDataExchange*,int,CString&,DWORD) function.
//
/////////////////////////////////////////////////////////////////////////////

CFileEditCtrl::CFileEditCtrl(BOOL bAutoDelete /* = FALSE */)
{
    m_bAutoDelete       = bAutoDelete;
    m_bButtonLeft       = (DWORD)~0;  // 0xFFFFFFFF
    m_bMouseCaptured    = FALSE;
    m_bTextChanged      = TRUE;
    m_dwFlags           = 0;
    m_dwImageDrawFlags  = 0;
    m_Files.RemoveAll();
    m_nButtonState      = BTN_UP;
    m_nButtonWidth      = -1;
    m_pBROWSEINFO       = NULL;
    m_pCFileDialog      = NULL;
    m_rcButtonRect.SetRectEmpty();
    m_szCaption.Empty();
    m_szClientTip.Empty();
    m_szFolder.Empty();

#ifdef AFX_PJAIMAGE_H__F15965B0_B05A_11D4_B625_A1459D96AB20__INCLUDED_
    m_pButtonImage      = NULL;
#endif
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl destructor  (public member function)
//    cleans up internal data variables
//
//  Parameters :
//    None
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

CFileEditCtrl::~CFileEditCtrl()
{
    if (m_pBROWSEINFO)
        delete m_pBROWSEINFO;
    if (m_pCFileDialog)
        delete m_pCFileDialog;

#ifdef AFX_PJAIMAGE_H__F15965B0_B05A_11D4_B625_A1459D96AB20__INCLUDED_
    if (m_pButtonImage)
        delete m_pButtonImage;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Message Map

BEGIN_MESSAGE_MAP(CFileEditCtrl, CEdit)
ON_CONTROL_REFLECT_EX(EN_CHANGE, OnChange)
ON_MESSAGE(EM_SETREADONLY, OnSetReadOnly)
ON_MESSAGE(WM_SETTEXT, OnSetText)
ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnTTNNeedText)
ON_WM_DROPFILES()
ON_WM_ENABLE()
ON_WM_KEYDOWN()
ON_WM_KILLFOCUS()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
ON_WM_NCCALCSIZE()
ON_WM_NCHITTEST()
ON_WM_NCLBUTTONDBLCLK()
ON_WM_NCLBUTTONDOWN()
ON_WM_NCMOUSEMOVE()
ON_WM_NCPAINT()
ON_WM_SETFOCUS()
ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileEditCtrl member functions

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::AddFile  (protected member function)
//    Adds the specified file to the m_Files list. Removes the path info
//    if it is the same as the path in m_szFolder
//
//  Parameters :
//    FileName [in] - The file to add
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::AddFile(CString File)
{
    DWORD Flags = GetFlags();
    if (Flags & FEC_FILE && !(Flags & FEC_NODEREFERENCELINKS))
    {   // resolve shortcuts (*.lnk or *.pif)
        CString Ext = File.Right(4);
        Ext.MakeLower();
        if (_T(".lnk") == Ext || _T(".pif") == Ext)
            DereferenceLink(File);
    }
    int FolderLength = m_szFolder.GetLength();
    if (File.Left(FolderLength) == m_szFolder)
        File.Delete(0, FolderLength);
    m_Files.AddTail(File);
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::ButtonClicked  (protected member function)
//    Called when the user clicks on the browse button.
//
//  Parameters :
//    None
//
//  Returns :
//    Nothing
//
//  Note :
//    Sends a WM_NOTIFY message to the parent window both before and after the dialogs have run.
//    The NMHDR parameter points to a FEC_NOTIFY structure.
//
//    Before : Sends the FEC_NM_PREBROWSE notification. Returning non-zero aborts this function.
//    After  : Sends the FEC_NM_POSTBROWSE notification. (update Mar 28, 2002 - now sent by called functions)
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::ButtonClicked()
{
    CWnd *pParent = GetParent();
    if (IsWindow(pParent))
    {
        FEC_NOTIFY notify(this, FEC_NM_PREBROWSE);
        if (pParent->SendMessage (WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&notify) != 0)
            return; // SendMessage returned nonzero, do not show dialog
    }

    DWORD Flags = GetFlags();

    if (Flags & FEC_FOLDER)
        FECBrowseForFolder();
    else if (Flags & (FEC_FILEOPEN | FEC_FILESAVEAS))
        FECOpenFile();
    else
        ASSERT (FALSE);  // control flags not properly set (should never get here)
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::Create  (public member function)
//    Creates the CFileEditCtrl in any window.
//
//  Parameters :
//    dwFlags        [in] - CFileEditCtrl flags ( FEC_* )
//    dwExStyle      [in] - Windows extended styles ( WS_EX_* )
//    lpszWindowName [in] - The initial text in the control
//    dwStyle        [in] - Windows and Edit control styles ( WS_* and ES_* )
//    rect           [in] - The position and size of the control
//    pParentWnd     [in] - Pointer to the control's parent window
//    nID            [in] - the control's ID
//
//  Returns :
//    TRUE if the control was successfully created
//    FALSE if not
//
//  Note :
//    See the FileEditCtrl.h file for descriptions of the flags used
//
/////////////////////////////////////////////////////////////////////////////

BOOL CFileEditCtrl::Create(DWORD dwFlags,
                           DWORD dwExStyle,
                           LPCTSTR lpszWindowName,
                           DWORD dwStyle,
                           const RECT& rect,
                           CWnd* pParentWnd,
                           UINT nID) 
{
    BOOL bResult = CreateEx(dwExStyle,
                            _T("EDIT"),
                            lpszWindowName,
                            dwStyle,
                            rect,
                            pParentWnd,
                            nID);
    if (bResult)
    {   // call CFileEditCtrl::SetFlags() to initialize the internal data structures
        bResult = SetFlags(dwFlags);
        if (bResult)
        {   // set the font to the font used by the parent window
            if (IsWindow(pParentWnd))
                SetFont(pParentWnd->GetFont());
        }
        else   // SetFlags() failed - destroy the window
            DestroyWindow();
    }
    return bResult;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::DereferenceLinks  (protected member function)
//    Gets the file path name pointed to by shortcut (*.lnk or *.pif) file
//
//  Parameters :
//    FileName [in]  : the shortcut file to be dereferenced
//             [out] : if successful, the complete path name of the file the
//                     shortcut points to, or unchanged if not successful
//
//  Returns :
//    TRUE if the link was dereferenced
//    FALSE if not
//
//  Note :
//    Thanks to Michael Dunn for his article "Introduction to COM - What It Is and How to Use It."
//    found at http://www.codeproject.com/com/comintro.asp
//
/////////////////////////////////////////////////////////////////////////////

BOOL CFileEditCtrl::DereferenceLink(CString &FileName)
{
    BOOL ret = FALSE;       // assume failure
    IShellLink *pIShellLink;
    IPersistFile *pIPersistFile;

    // initialize the COM libraries
    CoInitialize (NULL);

    // create an IShellLink object
    HRESULT hr = CoCreateInstance(CLSID_ShellLink,
                                  NULL,
                                  CLSCTX_INPROC_SERVER,
                                  IID_IShellLink,
                                  (void **) &pIShellLink);
    if (SUCCEEDED (hr))
    {
        // get the IPersistFile interface for this IShellLink object
        hr = pIShellLink->QueryInterface(IID_IPersistFile, (void **)&pIPersistFile);
        if (SUCCEEDED (hr))
        {
            int len = FileName.GetLength();
            WCHAR *pWFile = new WCHAR[len + 1];

            USES_CONVERSION;
            wcscpy(pWFile, T2COLE(FileName));

            // open and read the .lnk or .pif file
            hr = pIPersistFile->Load(pWFile, 0);
            if (SUCCEEDED(hr))
            {
                TCHAR buffer[_MAX_PATH];
                // get the file path name 
                hr = pIShellLink->GetPath(buffer, _MAX_PATH, NULL, 0 /*SLGP_UNCPRIORITY*/);
                if (SUCCEEDED(hr))
                {
                    FileName = buffer;
                    ret = TRUE;
                }
            }
            delete[] pWFile;
        }

        // release the IShellLink interface
        pIShellLink->Release();
    }

    // close the COM libraries
    CoUninitialize();

    return ret;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::DrawButton  (protected member function)
//    Draws the button on the control
//
//  Parameters :
//    nButtonState [in] - the state of the button ( Up, Down, or Disabled )
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::DrawButton(int nButtonState)
{   // if the button is too small, do not draw it
    if (m_rcButtonRect.Width() < 3 || m_rcButtonRect.Height() < 3)
        return;

    ASSERT(IsWindow(this));

    // if the control is disabled, ensure the button is drawn disabled
    if (GetStyle() & WS_DISABLED)
        nButtonState = BTN_DISABLED;

    nButtonState |= m_nButtonState & BTN_FLAT;

    CRect DrawRect(m_rcButtonRect);
    DrawRect.OffsetRect(0 - m_rcButtonRect.left, 0 - m_rcButtonRect.top);

    CWindowDC WndDC(this);     // get the DC for drawing

    // Create a memory DC to prevent flicker
    CDC MemDC;
    MemDC.CreateCompatibleDC(&WndDC);
    int savedDC = MemDC.SaveDC();
    CBitmap Bitmap;
    Bitmap.CreateCompatibleBitmap(&WndDC, m_rcButtonRect.Width(), m_rcButtonRect.Height());
    MemDC.SelectObject(&Bitmap);

    // use HS_DIAGCROSS pattern brush to test brush alignment
    CBrush theBrush(/*HS_DIAGCROSS,*/ GetSysColor(COLOR_3DFACE));     // the colour of the button background
    CPoint BrushOrg;
    CPoint Origin = WndDC.GetBrushOrg();
    BrushOrg.x = (BRUSHWIDTH - (m_rcButtonRect.left - Origin.x) % BRUSHWIDTH);
    BrushOrg.y = (BRUSHHEIGHT - (m_rcButtonRect.top - Origin.y) % BRUSHHEIGHT);
    MemDC.SetBrushOrg(BrushOrg);

    MemDC.SelectObject(&theBrush);
    MemDC.FillRect(&DrawRect, &theBrush);

    if (nButtonState & BTN_DOWN)
    {
        // draw the border
        CPen thePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
        CPen *pOldPen = MemDC.SelectObject(&thePen);
        MemDC.Rectangle(DrawRect);
        MemDC.SelectObject(pOldPen);
        thePen.DeleteObject();

#ifdef AFX_PJAIMAGE_H__F15965B0_B05A_11D4_B625_A1459D96AB20__INCLUDED_
        if (m_pButtonImage)
        {   // draw the image
            if (DrawRect.Width() > 5 && DrawRect.Height() > 5)
                m_pButtonImage->DrawImage(&MemDC,
                3,
                3,
                DrawRect.Width() - 5,
                DrawRect.Height() - 5,
                m_dwImageDrawFlags);
        }
        else   // draw the dots
#endif
            DrawDots(&MemDC, GetSysColor(COLOR_BTNTEXT), 1);
    }
    else    // draw button as up
    {
        // draw the border
        if (nButtonState & BTN_FLAT)
        {
            CPen thePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW));
            CPen *pOldPen = MemDC.SelectObject(&thePen);
            if (m_rcButtonRect.left < 4) // button on left side of control
                MemDC.Rectangle(DrawRect.left - 1, DrawRect.top - 1, DrawRect.right, DrawRect.bottom + 1);
            else
                MemDC.Rectangle(DrawRect.left, DrawRect.top - 1, DrawRect.right + 1, DrawRect.bottom + 1);
            MemDC.SelectObject(pOldPen);
            thePen.DeleteObject();
        }
        else
            MemDC.DrawEdge(DrawRect, EDGE_RAISED, BF_RECT);

        if (nButtonState & BTN_DISABLED)
        {
#ifdef AFX_PJAIMAGE_H__F15965B0_B05A_11D4_B625_A1459D96AB20__INCLUDED_
            if (m_pButtonImage)
            {   // draw the image
                if (DrawRect.Width() > 5 && DrawRect.Height() > 5)
                    m_pButtonImage->DrawImage(&MemDC,
                    2,
                    2,
                    DrawRect.Width() - 5,
                    DrawRect.Height() - 5,
                    PJAI_DISABLED | m_dwImageDrawFlags);
            }
            else
#endif
            {   // draw the dots
                DrawDots(&MemDC, GetSysColor(COLOR_3DHILIGHT), 1);
                DrawDots(&MemDC, GetSysColor(COLOR_3DSHADOW));
            }
        }
        else if (nButtonState & BTN_UP)
        {
#ifdef AFX_PJAIMAGE_H__F15965B0_B05A_11D4_B625_A1459D96AB20__INCLUDED_
            if (m_pButtonImage)
            {   // draw the image
                if (DrawRect.Width() > 5 && DrawRect.Height() > 5)
                    m_pButtonImage->DrawImage(&MemDC,
                    2,
                    2,
                    DrawRect.Width() - 5,
                    DrawRect.Height() - 5,
                    (((m_nButtonState & BTN_FLAT) && (GetFlags() & FEC_GRAYSCALE)) ? PJAI_GRAYSCALE : 0) | m_dwImageDrawFlags);
            }
            else   // draw the dots
#endif
            {
                COLORREF dotcolour = GetSysColor(COLOR_BTNTEXT);
                if ((m_nButtonState & BTN_FLAT) && (GetFlags() & FEC_GRAYSCALE))
                {
                    int grayscale = (((GetBValue(dotcolour) * 11) + (GetGValue(dotcolour) * 59) + (GetRValue(dotcolour) * 30)) / 100);
                    dotcolour = RGB(grayscale, grayscale, grayscale);
                }
                DrawDots(&MemDC, dotcolour);
            }
        }
        else
        {
            ASSERT(FALSE);  // Invalid nButtonState
        }
    }

//  ShowDC(MemDC);

    // Blit the button image onto the screen
    WndDC.BitBlt(m_rcButtonRect.left,
        m_rcButtonRect.top,
        m_rcButtonRect.Width(),
        m_rcButtonRect.Height(),
        &MemDC,
        0,
        0,
        SRCCOPY);

    // clean up the memory DC
    MemDC.RestoreDC(savedDC);
    Bitmap.DeleteObject();

    // update m_nButtonState
    m_nButtonState = nButtonState;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::DrawDots  (protected member function)
//    Draws the dots on the button
//
//  Parameters :
//    pDC     [in] - Pointer to the device context needed for drawing
//    CR      [in] - The colour of the dots
//    nOffset [in] - How far down and to the right to shift the dots
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::DrawDots(CDC *pDC, COLORREF CR, int nOffset /* = 0 */)
{
    int width = m_rcButtonRect.Width();         // width of the button
    div_t divt = div(width, 4);
    int delta = 3;//divt.quot;                      // space between dots
    int left = width / 3 - delta - (divt.rem ? 0 : 1); // left side of first dot
    left+=2;
    width = width / 10;                         // width and height of one dot
    int top = m_rcButtonRect.Height() / 3;// - width / 2;  // top of dots
    top*=2;
    left += nOffset;                            // draw dots shifted? (for button pressed)
    top += nOffset;
    // draw the dots
    if (width < 2)
    {
        pDC->SetPixelV(left, top, CR);
        left += delta;
        pDC->SetPixelV(left, top, CR);
        left += delta;
        pDC->SetPixelV(left, top, CR);
    }
    else
    {
        CPen thePen(PS_SOLID, 1, CR);           // set the dot colour
        CPen *pOldPen = pDC->SelectObject(&thePen);
        CBrush theBrush(CR);
        CBrush *pOldBrush = pDC->SelectObject(&theBrush);
        pDC->Ellipse(left, top, left + width, top + width);
        left += delta;
        pDC->Ellipse(left, top, left + width, top + width);
        left += delta;
        pDC->Ellipse(left, top, left + width, top + width);
        pDC->SelectObject(pOldBrush);           // reset the DC
        theBrush.DeleteObject();
        pDC->SelectObject(pOldPen);
        thePen.DeleteObject();
    }
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::ExpandWildCards  (protected member function)
//    resolves any wildcards ('*' and/or '?') found in the file name
//    calls AddFile() to add the files to the m_Files list
//
//  Parameters :
//    FileName [in] - the file name
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::ExpandWildCards(const CString &FileName)
{
    DWORD Flags = GetFlags();
    if (!(Flags & FEC_WILDCARDS) || -1 == FileName.FindOneOf(_T("*?")))
    {   // wildcards not permitted or not found
        AddFile(FileName);
        return;
    }
    CString Temp;
    CString Path;
    if (_T('\\') == FileName[0] || (FileName.GetLength() > 1 && _T(':') == FileName[1]))
        Temp = FileName;
    else
        Temp = m_szFolder + FileName;

    _tfullpath(Path.GetBuffer(_MAX_PATH), Temp, _MAX_PATH);
    Path.ReleaseBuffer();
    CFileFind cff;
    BOOL Finding = cff.FindFile(Path);
    while (Finding)
    {
        Finding = cff.FindNextFile();
        Path = cff.GetFilePath();
        if (!cff.IsDirectory())
        {
            AddFile(Path);
            if (!(Flags & FEC_MULTIPLE))
                break;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::FECBrowseForFolder  (protected member function)
//    Set up and call SHBrowseForFolder().
//    Update the control to the users selection
//
//  Parameters :
//    None
//
//  Returns :
//    TRUE if user clicks OK in SHBrowseForFolder dialog
//    FALSE otherwise
//
/////////////////////////////////////////////////////////////////////////////

BOOL CFileEditCtrl::FECBrowseForFolder()
{
    BOOL bReturnValue = FALSE;

#if defined _DEBUG
    if (m_pBROWSEINFO->lpfn == FECFolderProc)
        ASSERT((CWnd *)m_pBROWSEINFO->lParam == this);
#endif

    ITEMIDLIST *idl = SHBrowseForFolder(m_pBROWSEINFO);
    if (idl)  // SHBrowseForFolder returned successfully
    {
        CString Buffer = _T("");

        BOOL result = SHGetPathFromIDList(idl, Buffer.GetBuffer(_MAX_PATH));  // get path string from ITEMIDLIST
        Buffer.ReleaseBuffer();

        if (result)
        {
            if (GetFlags() & FEC_TRAILINGSLASH) // add a trailing slash if it is not already there
            {
                if (_T('\\') != Buffer[Buffer.GetLength() - 1])
                   Buffer+= _T("\\");
            }

            LRESULT retval = 0;
            CWnd *pParent = GetParent();
            if (IsWindow(pParent))
            {                                   // notify parent window that the control text is about to be updated
                FEC_NOTIFY notify(this, FEC_NM_POSTBROWSE);
                notify.pNewText = &Buffer;
                retval = pParent->SendMessage (WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&notify);
            }

            if (0 == retval)
            {
                SetWindowText(Buffer);          // update edit control text
                bReturnValue = TRUE;
            }
        }

        LPMALLOC lpm;
        if (SHGetMalloc(&lpm) == NOERROR)
            lpm->Free(idl);                     // free memory returned by SHBrowseForFolder
    }
    else
    {   // notify parent window that ShBrowseForFolder was cancelled
        CWnd *pParent = GetParent();
        if (IsWindow(pParent))
        {
            FEC_NOTIFY notify(this, FEC_NM_POSTBROWSE);
            pParent->SendMessage (WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&notify);
        }
    }

    SetFocus();                                 // ensure focus returns to this control
    return bReturnValue;
}

/////////////////////////////////////////////////////////////////////////////
//
//  FECFolderProc  (Global CALLBACK function)
//    This is the default callback procedure for the SHBrowseForFolder function.
//    It sets the initial selection to the directory specified in the edit control
//
//  Parameters :
//    hwnd   [in] - Handle of the SHBrowseForFolder dialog
//    nMsg   [in] - Message to be handled
//    lpData [in] - the lparam member of the BROWSEINFO structure, must be a pointer
//                  to the CFileEditCtrl
//
//  Returns :
//    zero
//
//  Note :
//    See 'SHBrowseForFolder' in MSDN for more info on the callback function for SHBrowseForFolder()
//
/////////////////////////////////////////////////////////////////////////////

int CALLBACK FECFolderProc(HWND hWnd, UINT nMsg, LPARAM, LPARAM lpData)
{
    if (BFFM_INITIALIZED == nMsg)
    {   // ensure we are passed a pointer to the CFileEditCtrl
        CWnd *pWnd = (CWnd *)lpData;
        ASSERT (pWnd->IsKindOf(RUNTIME_CLASS(CFileEditCtrl)));
        CFileEditCtrl* pCFEC = (CFileEditCtrl *)pWnd;

        CString szPath;
        POSITION pos = pCFEC->GetStartPosition();
        if (pos)
        {
            szPath = pCFEC->GetNextPathName(pos);
            if (_T("\\\\") != szPath.Left(2)) // SHBrowseForFolder does not like UNC path
            {
                int len = szPath.GetLength() - 1;
                if (2 != len && _T('\\') == szPath[len])
                    szPath.Delete(len); // remove trailing slash (SHBrowseForFolder does not like it)
                ::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)szPath);
            }
        }
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::FECOpenFile  (protected member function)
//    Set up the CFECFileDialog and call CFECFileDialog::DoModal().
//    Update the control to the users selection
//
//  Parameters :
//    None
//
//  Returns :
//    TRUE if the user clicked the OK button in the CFileDialog
//    FALSE otherwise
//
/////////////////////////////////////////////////////////////////////////////

BOOL CFileEditCtrl::FECOpenFile()
{
    BOOL bReturnValue = FALSE;
    BOOL bDirectory = TRUE;                     // assume user of this class has set the initial directory
    TCHAR lpstrDirectory[_MAX_PATH] = _T("");
    if (NULL == m_pCFileDialog->m_ofn.lpstrInitialDir)  // user has not set the initial directory
    {                                                   // flag it, and set the initial directory
        bDirectory = FALSE;                             // to the directory in the edit control
        POSITION pos = GetStartPosition();
        if (pos)
        {
            _tcscpy(lpstrDirectory, GetNextPathName(pos));
            
            DWORD attrib = GetFileAttributes(lpstrDirectory);
            if (((0xFFFFFFFF != attrib) && (!(attrib & FILE_ATTRIBUTE_DIRECTORY)))
                || ((0xFFFFFFFF == attrib) && (!(m_pCFileDialog->m_ofn.Flags & OFN_FILEMUSTEXIST))))
                // if ((file exists && is not a folder) || (does not exist && does not have to exist))
            {
                // set the filename editbox in CFileDialog to the name of the
                // first file in the control
                TCHAR Name[_MAX_FNAME];
                TCHAR Ext[_MAX_EXT];
                _tsplitpath(lpstrDirectory, NULL, NULL, Name, Ext);
                _tcscat(Name, Ext);
                _tcscpy(m_pCFileDialog->m_ofn.lpstrFile, Name);
            }
            else
                // empty the filename edit box
                _tcscpy(m_pCFileDialog->m_ofn.lpstrFile, _T(""));
            
            // Start browsing in the correct folder
            GetValidFolder(lpstrDirectory);
            m_pCFileDialog->m_ofn.lpstrInitialDir = lpstrDirectory;
        }
    }

    if (IDOK == m_pCFileDialog->DoModal())      // Start the CFECFileDialog
    {                                           // user clicked OK, enter files selected into edit control
        CString szFileSeparator;
#if defined FEC_NORESOURCESTRINGS
        szFileSeparator = FEC_IDS_SEPARATOR;
#else
        szFileSeparator.LoadString(FEC_IDS_SEPARATOR);
#endif
        ASSERT(szFileSeparator.GetLength() == 1);   // must be one character only
        szFileSeparator += _T(" ");
        CString szPath;
        POSITION pos = m_pCFileDialog->GetStartPosition();
        if (pos)                                // first file has complete path
            szPath = m_pCFileDialog->GetNextPathName(pos);
        TCHAR lpstrName[_MAX_FNAME];
        TCHAR lpstrExt[_MAX_EXT];
        CString szTempPath;
        while (pos)
        {                                       // remaining files are name and extension only
            szTempPath = m_pCFileDialog->GetNextPathName(pos);
            _tsplitpath(szTempPath, NULL, NULL, lpstrName, lpstrExt);
            szPath += szFileSeparator + lpstrName + lpstrExt;
        }

        LRESULT result = 0;
        CWnd *pParent = GetParent();
        if (IsWindow(pParent))
        {                                       // notify parent window that the control text is about to be updated
            FEC_NOTIFY notify(this, FEC_NM_POSTBROWSE);
            notify.pNewText = &szPath;
            result = pParent->SendMessage (WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&notify);
        }
        
        if (0 == result)
        {                                       // update the window text
            SetWindowText(szPath);
            bReturnValue = TRUE;
        }
    }
    else
    {                                           // CFileDialog cancelled
        CWnd *pParent = GetParent();
        if (IsWindow(pParent))
        {                                       // notify parent window
            FEC_NOTIFY notify(this, FEC_NM_POSTBROWSE);
            pParent->SendMessage (WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&notify);
        }
    }

    if (!bDirectory)
        m_pCFileDialog->m_ofn.lpstrInitialDir = NULL;

    SetFocus();                                 // ensure focus returns to this control
    return bReturnValue;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::FillBuffers  (protected member function)
//    Fills the m_szFolder and m_Files member variables
//
//  Parameters :
//    None
//
//  Returns :
//    Nothing
//
//  Note :
//    The m_szFolder and m_Files member variables are used by the GetStartPosition()
//    and GetNextPathName() functions to retrieve the file names entered by the user.
//
//    If the user entered a folder, m_Files will contain the complete path for the
//    folder, and m_szFolder will be empty
//
//    If the user entered multiple files, m_szFolder will contain the drive and folder
//    path of the first file entered, and m_Files will contain all the files. The files
//    may contain any complete or relative paths. Any relative paths will be evaluated
//    as being relative to the path contained in m_szFolder.
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::FillBuffers()
{
    ASSERT(IsWindow(this));                     // Control window must exist
#if defined FEC_NORESOURCESTRINGS
    m_szFolder = FEC_IDS_SEPARATOR;
#else
    m_szFolder.LoadString(FEC_IDS_SEPARATOR);
#endif
    TCHAR chSeparator = m_szFolder[0];          // get the character used to seperate the files

    m_szFolder.Empty();                         // empty the buffers of old data
    m_Files.RemoveAll(); // Empty();

    int len = GetWindowTextLength();
    if (!len)                                   // no files entered, leave buffers empty
        return;
    LPTSTR lpstrWindow = new TCHAR[len + 1];    // working buffer
    GetWindowText(lpstrWindow, len + 1);
    LPTSTR lpstrStart = lpstrWindow;            // points to the first character in a file name
    LPTSTR lpstrEnd = NULL;                     // points to the next separator character
    LPTSTR lpstrNext = NULL;
    while (*lpstrStart == chSeparator || _istspace(*lpstrStart))
        lpstrStart = CharNext(lpstrStart);      // skip over leading spaces and separator characters
    if (!*lpstrStart)
    {                                           // no files entered, leave buffers empty
        delete lpstrWindow;
        return;
    }
    lpstrEnd = _tcschr(lpstrStart, chSeparator);// find separator character
    if (lpstrEnd)
    {
        lpstrNext = CharNext(lpstrEnd);
        *lpstrEnd = 0;                          // mark end of string
    }
    LPTSTR temp = lpstrStart + _tcslen(lpstrStart);// - 1;
    temp = CharPrev(lpstrStart, temp);
    while (_istspace(*temp))                    // remove trailing white spaces from string
    {
        *temp = 0;
        temp = CharPrev(lpstrStart, temp);
    }
    DWORD dwFlags = GetFlags();
    CString File;
    if (dwFlags & FEC_FOLDER)
    {
        _tfullpath(File.GetBuffer(_MAX_PATH), lpstrStart, _MAX_PATH); // get absolute path
        File.ReleaseBuffer();
        int len = File.GetLength();
        if (dwFlags & FEC_TRAILINGSLASH)        // add a trailing slash if it is not already there
        {
            if (_T('\\') != File[len - 1])
                File += _T("\\");
        }
        else                                    // remove the trailing slash if it is there
        {
            if (3 != len && _T('\\') == File[len - 1])
                File.Delete(len - 1);
        }
        AddFile(File);
        delete lpstrWindow;
        return;
    }
    _TCHAR drive[_MAX_DRIVE];
    _TCHAR folder[_MAX_DIR];
    _TCHAR file[_MAX_FNAME];
    _TCHAR ext[_MAX_EXT];
    _tsplitpath(lpstrStart, drive, folder, file, ext);
    m_szFolder = (CString)drive + folder;       // drive and directory placed in m_szFolder
    File = (CString)file + ext;
    ExpandWildCards(File);
    if (dwFlags & FEC_MULTIPLE)
    {
        lpstrStart = lpstrNext;                 // reset to the start of the next string
        while (lpstrEnd)
        {   // add the rest of the files as they have been typed (include any path information)
            while (*lpstrStart == chSeparator || _istspace(*lpstrStart))
                lpstrStart = CharNext(lpstrStart); // remove leading spaces and separator characters
            if (!*lpstrStart)                   // last file was followed by spaces and/or separator characters,
                break;                          // there are no more files entered
            lpstrEnd = _tcschr(lpstrStart, chSeparator); // find next separator character
            if (lpstrEnd)
            {
                lpstrNext = CharNext(lpstrEnd);
                *lpstrEnd = 0;                  // mark end of string
            }
            temp = lpstrStart + _tcslen(lpstrStart);
            temp = CharPrev(lpstrStart, temp);
            while (_istspace(*temp))            // remove trailing white spaces from string
            {
                *temp = 0;
                temp = CharPrev(lpstrStart, temp);
            }
            ExpandWildCards(lpstrStart);
            lpstrStart = lpstrNext;             // reset to the start of the next string
        }
    }
    delete lpstrWindow;                         // delete working buffer
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::GetBrowseInfo  (public member function)
//    Retrieve a pointer to the BROWSEINFO structure
//
//  Parameters :
//    None
//
//  Returns :
//    A pointer to the BROWSEINFO structure if the FEC_FOLDER flag was set
//    NULL otherwise
//
//  Note :
//    If the default SHBrowseForFolder settings do not fit your use, Use the pointer
//    returned by this function to set up the SHBrowseForFolder using your own settings
//    
/////////////////////////////////////////////////////////////////////////////

BROWSEINFO* CFileEditCtrl::GetBrowseInfo() const
{
    return m_pBROWSEINFO;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::GetButtonWidth  (public member function)
//    Retrieves the width, in pixels, of the browse button 
//
//  Parameters :
//    None
//
//  Returns :
//    The width of the browse button
//
/////////////////////////////////////////////////////////////////////////////

int CFileEditCtrl::GetButtonWidth()
{
    return m_rcButtonRect.Width();
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::GetFlags  (public member function)
//    Retrieves the current flags
//
//  Parameters :
//    None
//
//  Returns :
//    the current flags
//
//  Note :
//    See the FileEditCtrl.h file for descriptions of the flags used
//    Because some flags can be changed via GetOpenFileName(), always use this
//    function to get the current state of the flags. Do not use m_dwFlags directly.
//
/////////////////////////////////////////////////////////////////////////////

DWORD CFileEditCtrl::GetFlags()
{
    DWORD Flags = m_dwFlags;
    if (m_pCFileDialog)
    {   // coordinate the FEC_* flags with the OFN_* flags
        if (m_pCFileDialog->m_ofn.Flags & OFN_NODEREFERENCELINKS)
            Flags |= FEC_NODEREFERENCELINKS;
        else
            Flags &= ~FEC_NODEREFERENCELINKS;

        if (m_pCFileDialog->m_ofn.Flags & OFN_ALLOWMULTISELECT)
            Flags |= FEC_MULTIPLE;
        else
            Flags &= ~FEC_MULTIPLE;
    }
    return Flags;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::GetNextPathName  (public member function)
//    Returns the file name at the specified position in the buffer.
//
//  Parameters :
//    pos [in]  - The position of the file name to retrieve
//        [out] - the position of the next file name
//
//  Returns :
//    The complete path name of the file or folder
//
//  Note :
//    The starting position is retrieved using the GetStartPosition() function.
//    pos will be set to NULL when there are no more files
//
/////////////////////////////////////////////////////////////////////////////

CString CFileEditCtrl::GetNextPathName(POSITION &pos)
{
    ASSERT(pos);                                // pos must not be NULL
    CString ReturnString;
    CString Temp = m_Files.GetNext(pos);
    
    if (0 == Temp.GetLength())
        Temp = m_szFolder;
    else if ((Temp.GetLength() > 1 && _T(':') != Temp[1])   // not drive
              && _T('\\') != Temp[0])           // nor current drive nor UNC
        Temp.Insert(0, m_szFolder);
    
    _tfullpath(ReturnString.GetBuffer(_MAX_PATH), Temp, _MAX_PATH); // get absolute path from any relative paths
    ReturnString.ReleaseBuffer();

    DWORD Flags = GetFlags();
    
    if (Flags & FEC_FILE)
    {
        Temp = ReturnString.Right(4);
        Temp.MakeLower();
        if ((_T(".lnk") == Temp || _T(".pif") == Temp) && !(Flags & FEC_NODEREFERENCELINKS))
            // resolve shortcuts (*.lnk  or *.pif files)
            DereferenceLink(ReturnString);
    }
    
    return ReturnString;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::GetOpenFileName  (public member function)
//    Retrieves a pointer to the OPENFILENAME structure
//
//  Parameters :
//    None
//
//  Returns :
//    A pointer to the OPENFILENAME structure if the FEC_FILE flag was set
//    NULL otherwise
//
//  Note :
//    If the default CFileDialog settings do not fit your use, Use the pointer
//    returned by this function to set up the CFileDialog using your own settings
//
/////////////////////////////////////////////////////////////////////////////

OPENFILENAME* CFileEditCtrl::GetOpenFileName() const
{
    if (m_pCFileDialog)
        return (&m_pCFileDialog->m_ofn);
    return NULL;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::GetStartPosition  (public member function)
//    If the control is active, calls FillBuffers() if the text has changed
//    returns the position of the first file in the buffers
//
//  Parameters :
//    None
//
//  Returns :
//    A MFC POSITION structure that points to the first file in the control
//
//  Note :
//    Use this function to get the starting position for the GetNextPathName() function
//
/////////////////////////////////////////////////////////////////////////////

POSITION CFileEditCtrl::GetStartPosition()
{
    if (IsWindow(this) && m_bTextChanged)
    {
        FillBuffers();
        m_bTextChanged = FALSE;
    }
    return m_Files.GetHeadPosition();
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::GetValidFolder  (protected member function)
//    Removes all files and nonexistant folders from the given path
//    Adds a slash to the end of the path string if it is not already there
//
//  Parameters :
//    Path [in]  - The path to check
//         [out] - The new path
//
//  Returns :
//    TRUE if the original path is valid
//    FALSE if the original path was invalid and has been changed
//
/////////////////////////////////////////////////////////////////////////////

BOOL CFileEditCtrl::GetValidFolder(LPTSTR Path)
{
    CString buffer = Path;
    BOOL valid = TRUE;
    int pos = -1;
    do {
        DWORD attrib = GetFileAttributes(buffer);
        if (0xffffffff != attrib && (attrib & FILE_ATTRIBUTE_DIRECTORY))
        {   // the path is a valid folder
            if (_T('\\') != buffer[buffer.GetLength() - 1])
                buffer += _T("\\");
            _tcscpy (Path, buffer);
            return valid;
        }
        valid = FALSE;
        pos = buffer.ReverseFind(_T('\\'));
        if (pos > 0)
        {
            int len = buffer.GetLength();
            buffer.Delete(pos, len - pos);
        }
    } while (pos > 0);

    // no valid folder, set 'Path' to empty string
    *Path = _T('\0');
    return valid;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::ModifyFlags  (public member function)
//    Modifies the control flags
//
//  Parameters :
//    remove [in] - The flags to remove
//    add    [in] - The flags to add
//
//  Returns :
//    TRUE if the flags are successfully modified
//    FALSE if not
//
//  Note :
//    See the FileEditCtrl.h file for descriptions of the flags used
//
/////////////////////////////////////////////////////////////////////////////

BOOL CFileEditCtrl::ModifyFlags(DWORD remove, DWORD add)
{
    DWORD Flags = GetFlags();
    Flags &= ~remove;
    Flags |= add;
    return SetFlags(Flags);
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnChange  (protected member function)
//    Handles the EN_CHANGE message
//    Sets the m_bTextChanged flag
//
//  Parameters :
//    None
//
//  Returns :
//    FALSE
//
//  Note :
//    Returning FALSE allows the parent window to also handle the EN_CHANGE message
//
/////////////////////////////////////////////////////////////////////////////

BOOL CFileEditCtrl::OnChange() 
{
    m_bTextChanged = TRUE;
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnDropFiles  (protected member function)
//    Handles the WM_DROPFILES message
//    Sets the control text to display the files dropped onto the control
//
//  Parameters :
//    hDropInfo [in] - handle to the drop structure supplied by windows
//
//  Returns :
//    Nothing
//
//  Note :
//    The control must have the WS_EX_ACCEPTFILES extended windows
//    style bit set in order for drag and drop to work
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::OnDropFiles(HDROP hDropInfo) 
{
    if (this == GetCapture())
        ReleaseCapture();

    int FolderLength = 0;
    CString szDroppedFiles = _T("");            // the new window text
    CString DropBuffer;                         // buffer to contain the dropped files
    CString szSeparator;
#if defined FEC_NORESOURCESTRINGS
    szSeparator = FEC_IDS_SEPARATOR;            // get the file separator character
#else
    szSeparator.LoadString(FEC_IDS_SEPARATOR);
#endif
    ASSERT(szSeparator.GetLength() == 1);       // must be one character only
    szSeparator += _T(" ");
    DWORD Flags = GetFlags();

    UINT nDropCount = DragQueryFile(hDropInfo, 0xffffffff, NULL, 0);
    if (nDropCount && ((Flags & FEC_FOLDER) || ((Flags & FEC_FILE) && !(Flags & FEC_MULTIPLE))))
        // if (files dropped && (finding folder || (finding files && finding only one file)))
        nDropCount = 1;

    for (UINT x = 0; x < nDropCount; x++)
    {
        DragQueryFile(hDropInfo, x, DropBuffer.GetBuffer(_MAX_PATH), _MAX_PATH);
        DropBuffer.ReleaseBuffer();
        if ((Flags & FEC_FILE) && !(Flags & FEC_NODEREFERENCELINKS))
        {   // resolve any shortcut (*.lnk or *.pif) files
            CString Ext = DropBuffer.Right(4);
            Ext.MakeLower();
            if (_T(".lnk") == Ext || _T(".pif") == Ext)
                DereferenceLink(DropBuffer);
        }
        if (0 == x)
        {   // first file has complete path, get the length of the path
            TCHAR Drive[_MAX_DRIVE];
            TCHAR Path[_MAX_PATH];
            _tsplitpath(DropBuffer, Drive, Path, NULL, NULL);
            FolderLength = _tcslen(Drive) + _tcslen(Path);
        }
        else
        {   // all the rest of the files will drop the path if it
            // is the same as the first file's path
            if (DropBuffer.Left(FolderLength) == szDroppedFiles.Left(FolderLength))
                DropBuffer.Delete(0, FolderLength);
            szDroppedFiles += szSeparator;
        }
        szDroppedFiles += DropBuffer;
    }

    DragFinish(hDropInfo);

    // notify parent window that a drop occured (thanks Remon)
    LRESULT result = 0;
    CWnd *pParent = GetParent();
    if (IsWindow(pParent))
    {
        FEC_NOTIFY notify(this, FEC_NM_DROP);
        notify.pNewText = &szDroppedFiles;
        result = pParent->SendMessage(WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&notify);
    }

    if (0 == result)
        SetWindowText(szDroppedFiles);

}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnEnable  (protected member function)
//    Handles the WM_ENABLE message
//    enables or disables the control, and redraws the button to reflect the new state
//
//  Parameters :
//    bEnable [in] - Enabled flag, TRUE to enable, FALSE to disable
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::OnEnable(BOOL bEnable) 
{
    CEdit::OnEnable(bEnable);
    Redraw(TRUE);
    DrawButton(BTN_UP);
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnKeyDown  (protected member function)
//    Handles the WM_KEYDOWN message
//    Checks for the <CTRL> + 'period' keystroke, calls ButtonClicked() if found
//
//  Parameters :
//    nChar   [in] - The virtual key code
//    nRepCnt [in] - not used here, passed on to the base class
//    nFlags  [in] - not used here, passed on to the base class
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
#ifndef VK_OEM_PERIOD
#define VK_OEM_PERIOD 0xBE
#endif
    if ((VK_OEM_PERIOD == nChar || VK_DECIMAL == nChar) && GetKeyState(VK_CONTROL) < 0)
        ButtonClicked();
    else
        CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnKillFocus  (protected member function)
//    Handles the WM_KILLFOCUS message
//    Redraws the border on a flat control
//
//  Parameters :
//    pNewWnd [in] - not used here, passed on to the base class
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::OnKillFocus(CWnd *pNewWnd)
{
    if (m_bMouseCaptured)
    {
        ReleaseCapture();
        m_bMouseCaptured = FALSE;
        if (!(m_nButtonState & BTN_UP))
            DrawButton(BTN_UP);
    }

    CEdit::OnKillFocus(pNewWnd);
    Redraw(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnLButtonDown  (protected member function)
//    Handles the WM_LBUTTONDOWN message
//    If the user clicks on the button, capture mouse input, set the focus
//    to this control, and draw the button as pressed
//
//  Parameters :
//    nFlags [in] - not used here, passed on to the base class
//    point  [in] - the location of the mouse cursor
//
//  Returns :
//    Nothing
//
//  Note:
//    If the FEC_FLAT flag is used, the mouse is captured in the WM_MOUSEMOVE
//    handler, so we have to use the WM_LBUTTONDOWN message instead of the
//    WM_NCLBUTTONDOWN message.
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::OnLButtonDown(UINT Flags, CPoint point)
{
    CEdit::OnLButtonDown(Flags, point);

    if (GetFlags() & FEC_FLAT && !(m_nButtonState & BTN_DISABLED))
    {
        ClientToScreen(&point);
        if (ScreenPointInButtonRect(point))
        {
            SetCapture();  // should already be captured, but just to make sure
            m_bMouseCaptured = TRUE;
            DrawButton(BTN_DOWN);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnLButtonUp  (protected member function)
//    Handles the WM_LBUTTONUP message
//    Release the mouse capture and draw the button as normal. If the
//    cursor is over the button, call ButtonClicked() 
//
//  Parameters :
//    nFlags [in] - not used here, passed on to the base class
//    point  [in] - the location of the mouse cursor
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
    CEdit::OnLButtonUp(nFlags, point);

    if (m_bMouseCaptured)
    {
        ReleaseCapture();
        m_bMouseCaptured = FALSE;
        if (BTN_UP != m_nButtonState)
            DrawButton(BTN_UP);
        ClientToScreen(&point);
        if (ScreenPointInButtonRect(point))
            ButtonClicked();
    }
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnMouseMove  (protected member function)
//    Handles the WM_MOUSEMOVE message
//    If the mouse has been captured, draws the button as up or down
//    as the mouse moves on or off the button. Also handles drawing the 
//    flat/raised border as the mouse moves off/over the control
//
//  Parameters :
//    nFlags [in] - not used here, passed on to the base class
//    point  [in] - The location of the mouse cursor
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
    CEdit::OnMouseMove(nFlags, point);

    ClientToScreen(&point);
    if (m_bMouseCaptured)
    {
        if (ScreenPointInButtonRect(point))
        {
            if (BTN_DOWN != m_nButtonState)
                DrawButton(BTN_DOWN);
        }
        else if (BTN_UP != m_nButtonState)
            DrawButton(BTN_UP);
    }
    else if (GetFlags() & FEC_FLAT && this != GetFocus())
    {
        if (this != GetCapture())
        {   // capture the mouse, draw 3D border
            SetCapture();
            Redraw(FALSE);
        }
        CRect rc;
        GetWindowRect(&rc);
        if (!rc.PtInRect(point))
        {   // mouse moved off control, release mouse, draw flat border
            ReleaseCapture();
            Redraw(TRUE);
        }
        else
        {   // set the correct mouse cursor
            GetClientRect(&rc);
            ClientToScreen(&rc);
            if (rc.PtInRect(point))
                SetCursor(LoadCursor(NULL, IDC_IBEAM));
            else
                SetCursor(LoadCursor(NULL, IDC_ARROW));
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnNcCalcSize  (protected member function)
//    Handles the WM_NCCALCSIZE message
//    Calculates the size and position of the button and client areas
//
//  Parameters :
//    bCalcValidRects [in] - specifies if the rgrc[1] and rgrc[2] rectangles in the 
//                           NCCALCSIZE_PARAMS structure are valid
//    lpncsp          [in] - Pointer to a NCCALCSIZE_PARAMS structure supplied by windows
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
    CEdit::OnNcCalcSize(bCalcValidRects, lpncsp);

    if (-1 == m_nButtonWidth) // CDB default to 100%
    {
       m_nButtonWidth = (lpncsp->rgrc[0].bottom - lpncsp->rgrc[0].top);
    }
    // if m_nButtonWidth is -1, set the button width to 80% of the control height
    // else set the button width to m_nButtonWidth
    

    int ButtonWidth = (m_nButtonWidth == -1) ? (lpncsp->rgrc[0].bottom - lpncsp->rgrc[0].top) * 8/10 : m_nButtonWidth;

    // set button area equal to client area of edit control
    m_rcButtonRect = lpncsp->rgrc[0];
    
    if (FEC_BUTTONLEFT == m_bButtonLeft)  // draw the button on the left side of the control
    {   // shrink left side of client area by the button width
        lpncsp->rgrc[0].left += ButtonWidth;
        // shrink the button so its right side is at the left side of client area
        m_rcButtonRect.right = lpncsp->rgrc[0].left;
    }
    else   // draw the button on the right side of the control
    {   // shrink right side of client area by the button width
        lpncsp->rgrc[0].right -= ButtonWidth;
        // shrink the button so its left side is at the right side of client area
        m_rcButtonRect.left = lpncsp->rgrc[0].right;
    }
    if (bCalcValidRects)
        // convert button coordinates from parent client coordinates to control window coordinates
        m_rcButtonRect.OffsetRect(-lpncsp->rgrc[1].left, -lpncsp->rgrc[1].top);
    m_rcButtonRect.NormalizeRect();

    if (IsWindow(m_ToolTip))
        m_ToolTip.DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnNcHitTest  (protected member function)
//    Handles the WM_NCHITTEST message
//    Ensures the control gets mouse messages when the mouse cursor is on the button
//
//  Parameters :
//    point [in] - The location of the mouse cursor
//
//  Returns :
//    The HitTest value for the mouse cursor location
//
//  Note :
//    If the mouse is over the button, OnNcHitTest() would normally return
//    HTNOWHERE, and we would not get any mouse messages. So we return 
//    HTBORDER to ensure we get them.
//
/////////////////////////////////////////////////////////////////////////////

UINT CFileEditCtrl::OnNcHitTest(CPoint point) 
{
    UINT where = CEdit::OnNcHitTest(point);
    if (HTNOWHERE == where && ScreenPointInButtonRect(point))
        where = HTBORDER;
    return where;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnNcLButtonDblClk  (protected member function)
//    Handles the WM_NCLBUTTONDBLCLK message
//    Have a double click on the button be treated as two single clicks
//
//  Parameters :
//    nHitTest [in] - not used here, passed on to the base class
//    point    [in] - the location of the mouse cursor
//
//  Returns :
//    Nothing
//
//  Note :
//    When this control sends a FEC_NM_PREBROWSE notification message to its
//    parent window, and the parent window returns a nonzero value, the browse
//    dialogs do not execute. This function makes the button go down and up on
//    the second click of a double click
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::OnNcLButtonDblClk(UINT nHitTest, CPoint point) 
{
    if (ScreenPointInButtonRect (point))
        OnNcLButtonDown(nHitTest, point);
    else
        CEdit::OnNcLButtonDblClk(nHitTest, point);
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnNcLButtonDown  (protected member function)
//    Handles the WM_NCLBUTTONDOWN message
//    If the user clicks on the button, capture mouse input, set the focus
//    to this control, and draw the button as pressed
//
//  Parameters :
//    nHitTest [in] - not used here, passed on to the base class
//    point    [in] - the location of the mouse cursor
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
    CEdit::OnNcLButtonDown(nHitTest, point);

    if (ScreenPointInButtonRect(point) && !(m_nButtonState & BTN_DISABLED))
    {
        SetFocus();
        SetCapture();
        m_bMouseCaptured = TRUE;
        DrawButton(BTN_DOWN);
    }
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnNcMouseMove  (protected member function)
//    Handles the WM_NCMOUSEMOVE message
//    Redraws the flat border as the mouse moves onto the control
//
//  Parameters :
//    nHitTest [in] - not used here, passed on to the base class
//    point    [in] - the location of the mouse cursor
//
//  Returns :
//    Nothing
//
//  Note :
//    Because the mouse is captured, all subsequent moves are handled by OnMouseMove()
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::OnNcMouseMove(UINT nHitTest, CPoint point)
{
    CEdit::OnNcMouseMove(nHitTest, point);

    if (GetFlags() & FEC_FLAT && this != GetFocus())
    {
        SetCapture();
        Redraw(FALSE);
    }
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnNcPaint  (protected member function)
//    Handles the WM_NCPAINT message
//    Redraws the control's border as needed
//
//  Parameters :
//    None
//
//  Returns :
//    Nothing
//
//  Note:
//    If the control is flat, we have to draw a COLOR_WINDOW coloured
//    border around the control
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::OnNcPaint() 
{
    if (m_nButtonState & BTN_FLAT)
    {   // draw the flat border
        CRect WindowRect;
        GetWindowRect(&WindowRect);
        CRect ClientRect;
        GetClientRect(&ClientRect);
        ClientToScreen(&ClientRect);
        int Border = ClientRect.top - WindowRect.top;
            
        CWindowDC DC(this);
        int saved = DC.SaveDC();
        CPen thePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW));  // border colour
        DC.SelectObject(&thePen);
        DC.Rectangle(Border - 1, Border - 1, WindowRect.Width() - Border + 1, WindowRect.Height() - Border + 1);
        DC.RestoreDC(saved);
        DeleteObject(thePen);
    }
    else
        CEdit::OnNcPaint();         // draws the 3D border around the control
    DrawButton(m_nButtonState);     // draw the button in its current state
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnSetFocus  (protected member function)
//    Handles the WM_SETFOCUS message
//    Selects (hilites) all the text in the control
//
//  Parameters :
//    pOldWnd [in] - not used here, passed on to the base class
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::OnSetFocus(CWnd* pOldWnd) 
{
    CEdit::OnSetFocus(pOldWnd);
    Redraw(FALSE);
    SetSel(0, -1);
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnSetReadOnly  (protected member function)
//    Handles the EM_SETREADONLY message
//
//  Parameters :
//    None
//
//  Returns :
//    Whatever the default windows procedure returns
//
//  Note :
//    If the control has the FEC_FLAT flag set, the border must
//    be redrawn when the readonly state is changed
//
/////////////////////////////////////////////////////////////////////////////

LRESULT CFileEditCtrl::OnSetReadOnly(WPARAM, LPARAM)
{
    LRESULT retval = Default();
    Redraw(TRUE);
    return retval;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnSetText  (protected member function)
//    Handles the WM_SETTEXT message
//    Sets the m_bTextChanged flag
//
//  Parameters :
//    None
//
//  Returns :
//    Whatever the default windows procedure returns
//
//  Note :
//    OnChange() does not seem to get called every time a WM_SETTEXT message
//    is sent to this control, so I duplicated it's functionality here.
//
//    CWnd does not have an OnSetText() handler function, so I called Default()
//    to ensure this message is properly handled 
//
/////////////////////////////////////////////////////////////////////////////

LRESULT CFileEditCtrl::OnSetText(WPARAM, LPARAM)
{
    m_bTextChanged = TRUE;
    return Default();
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnSize  (protected member function)
//    Handles the WM_SIZE message
//    Forces a recalculation of the button's and client area's size and position
//    also recalculates the tooltips bounding rectangles
//
//  Parameters :
//    nType [in] - not used here, passed on to the base class
//    cx    [in] - not used here, passed on to the base class
//    cy    [in] - not used here, passed on to the base class
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::OnSize(UINT nType, int cx, int cy) 
{
    CEdit::OnSize(nType, cx, cy);

    SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::OnTTNNeedText  (protected member function)
//    Handles the TTN_NEEDTEXT message from the tooltip control
//    Sets the tooltip text
//
//  Parameters :
//    pTTTStruct [in] - pointer to a TOOLTIPTEXT structure
//
//  Returns :
//    TRUE
//
/////////////////////////////////////////////////////////////////////////////

BOOL CFileEditCtrl::OnTTNNeedText(UINT, NMHDR *pTTTStruct, LRESULT *)
{
    DWORD Flags = GetFlags();
    TOOLTIPTEXT* pTTT = ((TOOLTIPTEXT*)pTTTStruct);
    if ((Flags & FEC_BUTTONTIP) && ID_BUTTONTIP == pTTT->hdr.idFrom)
    {
#if defined FEC_NORESOURCESTRINGS
        pTTT->lpszText = FEC_IDS_BUTTONTIP;
#else
        pTTT->lpszText = MAKEINTRESOURCE(FEC_IDS_BUTTONTIP);
        pTTT->hinst = AfxGetResourceHandle();
#endif
    }
    if ((Flags & FEC_CLIENTTIP) && ID_CLIENTTIP == pTTT->hdr.idFrom)
    {
        pTTT->lpszText = m_szClientTip.GetBuffer(m_szClientTip.GetLength());
        m_szClientTip.ReleaseBuffer();
    }
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::PostNcDestroy  (protected member function)
//    deletes this control object if the m_bAutoDelete flag is set
//
//  Parameters :
//    None
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::PostNcDestroy() 
{
    if (m_bAutoDelete)
        delete this;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::PreTranslateMessage  (public member function)
//    Sets up and passes messages to the tooltip control
//
//  Parameters :
//    pMsg [in] - the current windows message
//
//  Returns :
//    whatever CEdit::PreTranslateMessage() returns
//
/////////////////////////////////////////////////////////////////////////////

BOOL CFileEditCtrl::PreTranslateMessage(MSG* pMsg) 
{   // not using GetFlags() because this is faster
    if (m_dwFlags & (FEC_BUTTONTIP | FEC_CLIENTTIP))
    {
        if (!IsWindow (m_ToolTip))
        {
            // create and activate the tooltip control
            m_ToolTip.Create(this);
            m_ToolTip.Activate(TRUE);
            m_ToolTip.SetFont(GetFont());

            // Setup the button tooltip
            CRect rc(m_rcButtonRect);
            CRect wnd;
            GetWindowRect(&wnd);
            rc.OffsetRect(wnd.left, wnd.top);
            ScreenToClient(&rc);
            m_ToolTip.AddTool(this, LPSTR_TEXTCALLBACK, &rc, ID_BUTTONTIP);

            // Setup the client area tooltip
            GetClientRect(&wnd);
            m_ToolTip.AddTool(this, LPSTR_TEXTCALLBACK, &wnd, ID_CLIENTTIP);
        }
        m_ToolTip.RelayEvent(pMsg);
    }

    return CEdit::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::Redraw   (protected member function)
//    Redraws the control as either flat or 3D
//
//  Parameters :
//    Flat [in] : The control's flat state
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::Redraw(BOOL Flat)
{
    if (IsWindow(this) && GetFlags() & FEC_FLAT)
    {
        if (Flat && this != GetFocus())
        {   // draw flat
            m_nButtonState |= BTN_FLAT;
            // Get parent window to erase the border
            CWnd *pParent = GetParent();
            if (IsWindow(pParent))
            {
                CRect rc;
                GetWindowRect(&rc);
                pParent->ScreenToClient(&rc);
                pParent->RedrawWindow(&rc);
            }
        }
        else
        {   // draw 3D
            m_nButtonState &= ~BTN_FLAT;
            RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW/* | RDW_ERASE*/);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::ScreenPointInButtonRect  (protected member function)
//    determine if the mouse cursor is on the button
//
//  Parameters :
//    point [in] - The location of the mouse cursor in screen coordinates
//
//  Returns :
//    TRUE if the mouse cursor is on the button
//    FALSE if it is not
//
/////////////////////////////////////////////////////////////////////////////

BOOL CFileEditCtrl::ScreenPointInButtonRect(CPoint point)
{
    CRect ControlRect;
    GetWindowRect(&ControlRect);
    // convert point from screen coordinates to window coordinates
    point.Offset(-ControlRect.left, -ControlRect.top);
    return m_rcButtonRect.PtInRect(point);
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::SetButtonImage  (public member function)
//    Set the image to be used on the browse button
//
//  Parameters :
//    hImage      [in] - handle of the image (can be a HBITMAP or HICON)
//    PJAIFlags   [in] - The image flags (see the note below)
//    Transparent [in] - the transparent colour, if CLR_DEFAULT is used, the colour
//                       of the top left pixel is used as the transparent colour
//
//  Returns :
//    TRUE if a new image is set, FALSE if not
//
//  Note :
//    Flags used in the PJAIFlags parameter :
//      PJAI_ICON        - the image is a HICON
//      PJAI_BITMAP      - the image is a HBITMAP
//      PJAI_STRETCHED   - the image will be stretched (or shrunk) to fit on the button
//      PJAI_TRANSPARENT - the HBITMAP image will be drawn transparently. The transparent 
//                         colour is specified using the Transparent parameter.
//      PJAI_AUTODELETE  - the image handle will be deleted and the memory freed when 
//                         when a new image is set or the CFileEditCtrl is deleted. If
//                         this flag is not set, the user of this class is responsible
//                         for deleting the handle.
//
//    The PJAImage.h file must be included (see lines 37 to 41 of FileEditCtrl.h) in
//    order for this function to work.
//
/////////////////////////////////////////////////////////////////////////////

#ifdef AFX_PJAIMAGE_H__F15965B0_B05A_11D4_B625_A1459D96AB20__INCLUDED_
    BOOL CFileEditCtrl::SetButtonImage(HANDLE hImage, DWORD PJAIFlags, COLORREF Transparent/* = CLR_DEFAULT*/)
    {
        BOOL ret = FALSE;
        if (m_pButtonImage)
        {
            delete m_pButtonImage;
            m_pButtonImage = NULL;
        }

        if (hImage)
        {
            m_pButtonImage = new CPJAImage();
            if (m_pButtonImage->SetImage(hImage, PJAIFlags))
            {
                ret = TRUE;
                m_dwImageDrawFlags = PJAIFlags;
                m_pButtonImage->SetTransparentColour(Transparent);
                if (!(m_dwImageDrawFlags & PJAI_STRETCHED))
                    m_dwImageDrawFlags |= PJAI_CENTERED;
            }
        }
        if (IsWindow(this))
            RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
        return ret;
    }
#endif

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::SetButtonWidth  (public member function)
//    Sets the width, in pixels, of the browse button
//
//  Parameters :
//    width [in] - The new width of the button
//
//  Returns :
//    The previous width of the button
//
//  Note :
//    Setting the width to -1 causes the control to use the default width
//    which is calculated to be 80% of it's height
//
/////////////////////////////////////////////////////////////////////////////

int CFileEditCtrl::SetButtonWidth(int width/* = -1*/)
{
    int oldWidth = GetButtonWidth();
    m_nButtonWidth = width;
    SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    return oldWidth;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::SetClientTipText  (public member function)
//    Sets the text to be used by the client area tooltip
//
//  Parameters :
//    text [in] - The text to set
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFileEditCtrl::SetClientTipText(CString text)
{
    m_szClientTip = text;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFileEditCtrl::SetFlags  (public member function)
//    Sets all the internal flags
//    Initializes and sets up the OPENFILENAME or BROWSEINFO structures
//    Forces the control to be redrawn if the button position changes
//
//  Parameters :
//    dwFlags [in] - The flags to set
//
//  Returns :
//    TRUE if successful
//    FALSE if not
//
//  Note :
//    See the FileEditCtrl.h file for descriptions of the flags used
//
/////////////////////////////////////////////////////////////////////////////

BOOL CFileEditCtrl::SetFlags(DWORD dwFlags)
{
    m_bTextChanged = TRUE;

    if (dwFlags & FEC_FOLDER)
    {   // set the control to find folders
        if (dwFlags & (FEC_FILEOPEN | FEC_FILESAVEAS))
        {
            TRACE (_T("CFileEditCtrl::SetFlags() : Can not specify FEC_FILEOPEN or FEC_FILESAVEAS with FEC_FOLDER\n"));
            ASSERT (FALSE);
            return FALSE;
        }
        if (!m_pBROWSEINFO)
        {   // create the BROWSEINFO structure
            m_pBROWSEINFO = new BROWSEINFO();
            if (!m_pBROWSEINFO)
            {
                TRACE (_T("CFileEditCtrl::SetFlags() : Failed to create the BROWSEINFO structure\n"));
                ASSERT (FALSE);
                return FALSE;
            }
            if (m_pCFileDialog)
            {
                delete m_pCFileDialog;          // delete the CFileDialog
                m_pCFileDialog = NULL;
            }
            // set up the BROWSEINFO structure used by SHBrowseForFolder()
            ::ZeroMemory(m_pBROWSEINFO, sizeof(BROWSEINFO));
            m_pBROWSEINFO->hwndOwner = GetSafeHwnd();
            m_pBROWSEINFO->lParam = (LPARAM)this;
            m_pBROWSEINFO->lpfn = FECFolderProc;
            m_pBROWSEINFO->ulFlags = BIF_RETURNONLYFSDIRS;
        }
    }

    else if (dwFlags & (FEC_FILEOPEN | FEC_FILESAVEAS))
    {   // set the control to find files
        if ((dwFlags & FEC_FILEOPEN) && (dwFlags & FEC_FILESAVEAS))
        {
            TRACE (_T("CFileEditCtrl::SetFlags() : Can not specify FEC_FILEOPEN with FEC_FILESAVEAS"));
            ASSERT (FALSE);
            return FALSE;
        }

        BOOL bOpen = dwFlags & FEC_FILEOPEN;

        if (!m_pCFileDialog)
        {   // create the CFileDialog
            CString szFilter;
#if defined FEC_NORESOURCESTRINGS
            szFilter = FEC_IDS_ALLFILES;
#else
            szFilter.LoadString(FEC_IDS_ALLFILES);
#endif
            m_pCFileDialog = new CFECFileDialog(bOpen,
                NULL,
                NULL,
                OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
                szFilter,
                this);
            if (!m_pCFileDialog)
            {
                TRACE (_T("CFileEditCtrl::SetFlags() : Failed to create the CFileDialog\n"));
                ASSERT (FALSE);
                return FALSE;
            }
            if (m_pBROWSEINFO)
            {
                delete m_pBROWSEINFO;               // delete the BROWSEINFO structure
                m_pBROWSEINFO = NULL;
            }
        }
        
        m_pCFileDialog->SetOpenSave(bOpen);

        if (bOpen)
            m_pCFileDialog->m_ofn.Flags |= OFN_FILEMUSTEXIST;
        else
            m_pCFileDialog->m_ofn.Flags |= OFN_OVERWRITEPROMPT;

        if (dwFlags & FEC_MULTIPLE)
            m_pCFileDialog->m_ofn.Flags |= OFN_ALLOWMULTISELECT;
        else
            m_pCFileDialog->m_ofn.Flags &= ~OFN_ALLOWMULTISELECT;
            
        if (dwFlags & FEC_NODEREFERENCELINKS)
            m_pCFileDialog->m_ofn.Flags |= OFN_NODEREFERENCELINKS;
        else
            m_pCFileDialog->m_ofn.Flags &= ~OFN_NODEREFERENCELINKS;

        m_pCFileDialog->m_ofn.hwndOwner = GetSafeHwnd();
#if defined FEC_NORESOURCESTRINGS
        m_szCaption = FEC_IDS_FILEDIALOGTITLE;
#else
        m_szCaption.LoadString(FEC_IDS_FILEDIALOGTITLE);
#endif
        m_pCFileDialog->m_ofn.lpstrTitle = (LPCTSTR)m_szCaption;
    }

    else
    {
        TRACE (_T("CFileEditCtrl::SetFlags() : Must specify either FEC_FILEOPEN, FEC_FILESAVEAS or FEC_FOLDER when setting flags\n"));
        ASSERT (FALSE);
        return FALSE;
    }

    // m_bButtonLeft will be 0xFFFFFFFF the first time SetFlags() is called
    // (dwFlags & FEC_BUTTONLEFT) will be either 0x00000000 or FEC_BUTTONLEFT
    if (m_bButtonLeft != (dwFlags & FEC_BUTTONLEFT))
    {
        // move the button to the desired side of the control
        m_bButtonLeft = dwFlags & FEC_BUTTONLEFT;
        // force a call to OnNcCalcSize to calculate the size and position of the button and client area
        SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        if (m_dwFlags & FEC_FLAT)
            RedrawWindow(); // the text needs to be redrawn if moving the button on a flat control
    }

    // check to see if the FEC_FLAT flag has changed
    // if it has, redraw the control to reflect it's new state
    if ((dwFlags & FEC_FLAT) != (m_dwFlags & FEC_FLAT))
    {
        m_dwFlags |= FEC_FLAT;   // needed to redraw the control's borders 
        Redraw(dwFlags & FEC_FLAT);
    }

    m_dwFlags = dwFlags;
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// DDV_FileEditCtrl & DDX_FileEditCtrl

/////////////////////////////////////////////////////////////////////////////
//
//  DDV_FileEditCtrl  (Global function)
//    Verifies that the files or folders entered actually exist
//
//  Parameters :
//    pDX  [in] - Pointer to the CDataExchange object
//    nIDC [in] - The controls resource ID
//
//  Returns :
//    Nothing
//
//  Note :
//    If the file or folder is invalid, pops up a messagebox informing the user,
//    then sets the focus to the offending CFileEditCtrl
//
/////////////////////////////////////////////////////////////////////////////

void DDV_FileEditCtrl(CDataExchange *pDX, int nIDC)
{
    CWnd *pWnd = pDX->m_pDlgWnd->GetDlgItem(nIDC);
    ASSERT(pWnd);
    if (!pWnd->IsKindOf(RUNTIME_CLASS(CFileEditCtrl)))  // is this control a CFileEditCtrl control?
    {
        TRACE(_T("Control %d not subclassed to CFileEditCtrl, must first call DDX_FileEditCtrl()"), nIDC);
        ASSERT(FALSE);
        AfxThrowNotSupportedException();
    }
    if (!pDX->m_bSaveAndValidate)               // not saving data
        return;
    CFileEditCtrl *pFEC = (CFileEditCtrl *)pWnd;
    pDX->PrepareEditCtrl(nIDC);
    POSITION pos = pFEC->GetStartPosition();
    if (!pos)
    {	// no name entered
        AfxMessageBox(FEC_IDS_NOFILE);
        pDX->Fail();
    }
    while (pos)
    {
        CString szMessage;
        CString szFile = pFEC->GetNextPathName(pos);
        DWORD dwAttribute = GetFileAttributes(szFile);
        if (0xFFFFFFFF == dwAttribute)          // GetFileAttributes() failed
        {                                       // does not exist
            szMessage.Format(FEC_IDS_NOTEXIST, szFile);
            AfxMessageBox(szMessage);
            pDX->Fail();
        }
        if ((pFEC->GetFlags() & FEC_FOLDER) && !(dwAttribute & FILE_ATTRIBUTE_DIRECTORY))
        {                                       // not a folder
            szMessage.Format(FEC_IDS_NOTFOLDER, szFile);
            AfxMessageBox(szMessage);
            pDX->Fail();
        }
        if ((pFEC->GetFlags() & FEC_FILE) && (dwAttribute & FILE_ATTRIBUTE_DIRECTORY))
        {                                       // not a file
            szMessage.Format(FEC_IDS_NOTFILE, szFile);
            AfxMessageBox(szMessage);
            pDX->Fail();
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
//
//  DDX_FileEditCtrl (Global function)
//    Subclasses the control with the given ID
//    Transfers the data between the control and the supplied CString
//
//  Parameters :
//    pDX     [in] - Pointer to the CDataExchange object
//    nIDC    [in] - The controls resource ID
//    rStr    [in] - The CString that contains the initial control text, and receives the text
//    dwFlags [in] - The flags used to setup this control
//
//  Returns :
//    Nothing
//
//  Note :
//    See the FileEditCtrl.h file for descriptions of the flags used
//    the FEC_MULTIPLE flag can not be used (how can multiple files be returned in one CString?)
//
/////////////////////////////////////////////////////////////////////////////

void DDX_FileEditCtrl(CDataExchange *pDX, int nIDC, CString& rStr, DWORD dwFlags)
{
    CWnd *pWnd = pDX->m_pDlgWnd->GetDlgItem(nIDC);
    ASSERT(pWnd);
    if (pDX->m_bSaveAndValidate)                // update string with text from control
    {
        // ensure the control is a CFileEditCtrl control
        ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CFileEditCtrl)));
        // copy the first file listed in the control to the string
        rStr.Empty();
        CFileEditCtrl *pFEC = (CFileEditCtrl *)pWnd;
        POSITION pos = pFEC->GetStartPosition();
        if (pos)
            rStr = pFEC->GetNextPathName(pos);
    }
    else                                        // create the control if it is not already created
    {                                           // set the control text to the text in string
        CFileEditCtrl *pFEC = NULL;
        if (!pWnd->IsKindOf(RUNTIME_CLASS(CFileEditCtrl)))    // not subclassed yet
        {
            // create then subclass the control to the edit control with the ID nIDC
            HWND hWnd = pDX->PrepareEditCtrl(nIDC);
            pFEC = new CFileEditCtrl(TRUE);     // create the control with autodelete
            if (!pFEC)                          // failed to create control
            {
                ASSERT(FALSE);
                AfxThrowNotSupportedException();
            }
            if (!pFEC->SubclassWindow(hWnd))
            {                                   // failed to subclass the edit control
                delete pFEC;
                ASSERT(FALSE);
                AfxThrowNotSupportedException();
            }
            // call CFileEditCtrl::SetFlags() to initialize the internal data structures
            dwFlags &= ~FEC_MULTIPLE;           // can not put multiple files in one CString
            if (!pFEC->SetFlags(dwFlags))
            {
                delete pFEC;
                ASSERT(FALSE);
                AfxThrowNotSupportedException();
            }
        }
        else                                    // control already exists
            pFEC = (CFileEditCtrl *)pWnd;
        if (pFEC)
            pFEC->SetWindowText(rStr);          // set the control text
    }
}

/////////////////////////////////////////////////////////////////////////////
//
//  DDX_FileEditCtrl (Global function)
//    Subclasses the control with the given ID
//    Transfers the data between the control and the supplied CFileEditCtrl
//
//  Parameters :
//    pDX     [in] - Pointer to the CDataExchange object
//    nIDC    [in] - The controls resource ID
//    rCFEC   [in] - The CFileEditCtrl object that is to control this window
//    dwFlags [in] - The flags used to setup this control
//
//  Returns :
//    Nothing
//
//  Note :
//    See the FileEditCtrl.h file for descriptions of the flags used
//
/////////////////////////////////////////////////////////////////////////////

void DDX_FileEditCtrl(CDataExchange *pDX, int nIDC, CFileEditCtrl &rCFEC, DWORD dwFlags)
{
    ASSERT(pDX->m_pDlgWnd->GetDlgItem(nIDC));
    if (NULL == rCFEC.m_hWnd)                   // not yet subclassed
    {
        ASSERT(!pDX->m_bSaveAndValidate);
        // subclass the control to the edit control with the ID nIDC
        HWND hWnd = pDX->PrepareEditCtrl(nIDC);
        if (!rCFEC.SubclassWindow(hWnd))
        {                                       // failed to subclass the edit control
            ASSERT(FALSE);
            AfxThrowNotSupportedException();
        }
        // call CFileEditCtrl::SetFlags() to initialize the internal data structures
        if (!rCFEC.SetFlags(dwFlags))
        {
            ASSERT(FALSE);
            AfxThrowNotSupportedException();
        }
    }
    else if (pDX->m_bSaveAndValidate)
        rCFEC.GetStartPosition();               // updates the data from the edit control
}

/////////////////////////////////////////////////////////////////////////////
// FEC_NOTIFY structure

/////////////////////////////////////////////////////////////////////////////
//
//  FEC_NOTIFY constructor (public member function)
//    Initializes the FEC_NOTIFY structure used when the CFileEditCtrl sends
//    a WM_NOTIFY message to it's parent window (in CFileEditCtrl::ButtonClicked())
//
//  Parameters :
//    FEC  [in] - pointer to the CFileEditCtrl sending the message
//    code [in] - The notification message being sent. (FEC_NM_PREBROWSE,
//                FEC_NM_POSTBROWSE or FEC_NM_DROP)
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

tagFEC_NOTIFY::tagFEC_NOTIFY (CFileEditCtrl *FEC, UINT code)
{
    pFEC = FEC;
    hdr.hwndFrom = FEC->GetSafeHwnd();
    hdr.idFrom = FEC->GetDlgCtrlID();
    hdr.code = code;
    pNewText = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CFECFileDialog

IMPLEMENT_DYNAMIC(CFECFileDialog, CFileDialog)

/////////////////////////////////////////////////////////////////////////////
//
//  CFECFileDialog constructor  (public member function)
//    Initializes the class variables
//
//  Parameters :
//    See CFileDialog::CFileDialog for parameter information
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

CFECFileDialog::CFECFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
                               DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
    Files = NULL;
    Folder = NULL;
    bParsed = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFECFileDialog destructor  (public member function)
//    Cleans up the class variables
//
//  Parameters :
//    None
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

CFECFileDialog::~CFECFileDialog()
{
    if (Files)
    {
        delete[] Files;
        delete[] Folder;
    }
}

BEGIN_MESSAGE_MAP(CFECFileDialog, CFileDialog)
//{{AFX_MSG_MAP(CFECFileDialog)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//  CFECFileDialog::DoModal  (public member function)
//    Starts the CFileDialog and properly handles the return code
//
//  Parameters :
//    None
//
//  Returns :
//    IDOK if the user selected the OK button
//    IDCANCEL if the user selected the Cancel button or an error occured
//
/////////////////////////////////////////////////////////////////////////////

int CFECFileDialog::DoModal()
{
    if (Files)
    {
        delete[] Files;
        Files = NULL;
        delete[] Folder;
        Folder = NULL;
    }

    int ret = CFileDialog::DoModal();

    if (ret == IDCANCEL)
    {
        DWORD err = CommDlgExtendedError();
        if (err == FNERR_BUFFERTOOSMALL/*0x3003*/ && Files)
            ret = IDOK;
    }
	return ret;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFECFileDialog::GetNextPathName  (public member function)
//    Call this function to retrieve the next file name from the group
//    selected in the dialog box.
//
//  Parameters :
//    pos - A reference to a POSITION value returned from a previous GetNextPathName
//          or GetStartPosition function call. Will be set to NULL when the end of
//          the list has been reached.
//
//  Returns :
//    The full path of the file
//
/////////////////////////////////////////////////////////////////////////////

CString CFECFileDialog::GetNextPathName(POSITION &pos) const
{
    if (!Files)
        return CFileDialog::GetNextPathName(pos);

    ASSERT(pos);    
    TCHAR *ptr = (TCHAR *)pos;

    CString ret = Folder;
    ret += _T("\\");
    ret += ptr;

    ptr += _tcslen(ptr) + 1;
    if (*ptr)
        pos = (POSITION)ptr;
    else
        pos = NULL;

    return ret;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFECFileDialog::GetStartPosition  (public member function)
//    Call this member function to retrieve the POSITION of the first file
//    in a list of multiple selected files.
//
//  Parameters :
//    None
//
//  Returns :
//    A POSITION value that is used by the GetNextPathName function
//
/////////////////////////////////////////////////////////////////////////////

POSITION CFECFileDialog::GetStartPosition()
{
    if (!Files)
        return CFileDialog::GetStartPosition();

    if (!bParsed)
    {
        CString temp = Files;
        temp.Replace(_T("\" \""), _T("\""));
        temp.Delete(0, 1);                      // remove leading quote mark
        temp.Delete(temp.GetLength() - 1, 1);   // remove trailing space
    
        _tcscpy(Files, temp);
    
        TCHAR *ptr = Files;
        while (*ptr)
        {
            if ('\"' == *ptr)
                *ptr = '\0';
            ++ptr;
        }
        bParsed = TRUE;
    }
    
    return (POSITION)Files;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFECFileDialog::OnFileNameChange  (protected member function)
//    If the lpstrFile and nMaxFile variables in the OPENFILENAME structure
//    are not large enough to handle the users selection, we handle the
//    selection ourselves.
//
//  Parameters :
//    None
//
//  Returns :
//    Nothing
//
//  Note :
//    Works only if the CFileDialog is created with the OFN_EXPLORER flag
//
/////////////////////////////////////////////////////////////////////////////

void CFECFileDialog::OnFileNameChange()
{
    TCHAR dummy_buffer;
    
    // Get the required size for the 'files' buffer
    UINT nfiles = CommDlg_OpenSave_GetSpec(GetParent()->m_hWnd, &dummy_buffer, 1);

    // Get the required size for the 'folder' buffer
    UINT nfolder = CommDlg_OpenSave_GetFolderPath(GetParent()->m_hWnd, &dummy_buffer, 1);

    // Check if lpstrFile and nMaxFile are large enough
    if (nfiles + nfolder > m_ofn.nMaxFile)
    {
        bParsed = FALSE;
        if (Files)
            delete[] Files;
        Files = new TCHAR[nfiles + 1];
        CommDlg_OpenSave_GetSpec(GetParent()->m_hWnd, Files, nfiles);

        if (Folder)
            delete[] Folder;
        Folder = new TCHAR[nfolder + 1];
        CommDlg_OpenSave_GetFolderPath(GetParent()->m_hWnd, Folder, nfolder);
    }
    else if (Files)
    {
        delete[] Files;
        Files = NULL;
        delete[] Folder;
        Folder = NULL;
    }

    CFileDialog::OnFileNameChange();
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFECFileDialog::OnInitDialog  (protected member function)
//    Set the text of the IDOK button on an old style dialog to 'OK'
//
//  Parameters :
//    None
//
//  Returns :
//    TRUE
//
/////////////////////////////////////////////////////////////////////////////

BOOL CFECFileDialog::OnInitDialog() 
{
    CFileDialog::OnInitDialog();
    
    if (!(m_ofn.Flags & OFN_EXPLORER))
    {
        CString szOkButton;
#if defined FEC_NORESOURCESTRINGS
        szOkButton = FEC_IDS_OKBUTTON;
#else
        szOkButton.LoadString(FEC_IDS_OKBUTTON);
#endif
        GetDlgItem(IDOK)->SetWindowText(szOkButton);
    }
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFECFileDialog::OnInitDone  (protected member function)
//    Set the text of the IDOK button on an explorer style dialog to 'OK'
//
//  Parameters :
//    None
//
//  Returns :
//    Nothing
//
/////////////////////////////////////////////////////////////////////////////

void CFECFileDialog::OnInitDone()
{
    CString szOkButton;
#if defined FEC_NORESOURCESTRINGS
    szOkButton = FEC_IDS_OKBUTTON;
#else
    szOkButton.LoadString(FEC_IDS_OKBUTTON);
#endif
    CommDlg_OpenSave_SetControlText(GetParent()->m_hWnd, IDOK, (LPCTSTR)szOkButton);
}

/////////////////////////////////////////////////////////////////////////////
//
//  CFECFileDialog::SetOpenSave  (public member function)
//    Allows one to switch an existing CFileDialog instance from FileOpen to
//    FileSaveAs and back again
//
//  Parameters :
//    bOpen [in] - TRUE to make dialog a FileOpen dialog
//                 FALSE to make dialog a FileSaveAs dialog
//
//  Returns :
//    The previous state
//
/////////////////////////////////////////////////////////////////////////////

BOOL CFECFileDialog::SetOpenSave(BOOL bOpen)
{
    BOOL ret = m_bOpenFileDialog;
    m_bOpenFileDialog = bOpen;
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
//
//  End of FileEditCtrl.cpp
//
/////////////////////////////////////////////////////////////////////////////
