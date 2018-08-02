///////////////////////////////////////////////////////////////////////
//
// Mylib.h
//
// Author : Conrad Braam
// This lib is a collection of usefull classes, mostly graphical in nature
//
// Credits CStaticLink - Paul Di Lascia
//         Ado classes - Carlos Antollini
//
//
///////////////////////////////////////////////////////////////////////

#ifndef __AFXMT_H__
   // include multithreaded MFC headder for Thread class
	#include <afxmt.h>

   //#error Must include <afxmt.h> Multithreaded classes.
#endif


#ifndef __MYLIB_H__
#define __MYLIB_H__

#include <afxwin.h>         // MFC core and standard components
#include <afxdlgs.h>
#if     _MSC_VER > 1200	//VC 9
	#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x502   // Target XP SP2
	#endif
#else
	// "unsafe" CRT functions which I've changed to use the safe version in subsequent CRT.
	// macros defined here temporally bypass the non-existent safe versions.
	#define strncpy_s(a,b,c,d)		strncpy(a,c,d)
	#define strcpy_s(a,b,c)			strcpy(a,c)
	#define strtok_s(a,b,c)			strtok(a,b)
	#define sprintf_s(a,b,c,d)		sprintf(a,c,d)
	#define sscanf_s(a,b,c)			sscanf(a,b,c)

	#define _itoa_s(a,b,c,d)				itoa(a,b,d)
	#define _ltoa_s(a,b,c,d)				ltoa(a,b,d)
	#define _ultoa_s(a,b,c,d)				ultoa(a,b,d)
	#define _gcvt_s(a,b,c,d)				gcvt(c,d,a)

#endif

//#include "ado.h"
#include "statlink.h"
#include "colorstatic.h"
#include "microtick.h"
#include "tools.h"
#include "commslamp.h"
#include "colorpickercb.h"
#include "FileEditCtrl.h"

#define SUCCESS                     0

#ifdef FAILED
   #undef FAILED
#endif
#define FAILED                   -1

#define MAX_VALUE_NAME           256
#define MAX_COMPORT_NAME            256

// dialog-box constants
#define PORT_INUSESTR      " *"     // appended to LB for used comm ports

int ConvertASCIIToByte(const char*String, BYTE &bValue);

LONG CalcLRC(BYTE * lrcBuffPtr,    //  -> pointer to buffer at start of LRC
             DWORD  buffLen,       //  ->
             BOOL   calc2Comp,     //  -> Calculate 2's Complement also (last call)
             BYTE * lrc            // <->
            );

LONG SwopWords(DWORD * x);
LONG SwopBytes(WORD * x);
LONG SwopDWords(DWORD * x);
double Round(double val);

#define MAX_ERROR_STR_LEN     256

CHAR * GetLongComPortName(LPCTSTR portName, LPSTR newName);
LONG EnumerateSerialPorts (char *deviceName, //port name
                           DWORD maxLen,     //length of above string
                           DWORD index);     // index, called in a loop (0...) until this fails

LONG CalcCRC( BYTE * crcStartPtr,   //  ->
                    DWORD  buffLen,       //  ->
                    WORD * crc            // <->
                   );

BOOL ExistFile(const CHAR * fN);


/////////////////////////////////////////////////////////////////////////////
// Registry access class
//

class DRegKey : public CObject
{
   public:
      // default constructor & destructor
      DRegKey()
         { Init(); };
      ~DRegKey()
         { Close(); };
   
      // alternate constructor
      DRegKey(HKEY hKey, LPCTSTR keyName = NULL, REGSAM regSam= KEY_ALL_ACCESS);

      // operations
      LONG Open(HKEY hKey, LPCTSTR keyName = NULL, REGSAM regSam= KEY_ALL_ACCESS);
      LONG Close();
      void Attach(HKEY key)
         { m_hKey = key; };
      HKEY Detach()
         { HKEY temp = m_hKey; m_hKey = NULL; return(temp);};
      operator HKEY() const;

      // the functions below return the win32 result codes: TRUE/FALSE
      // get value methods
      LONG QueryValue(LPCTSTR valueName , DWORD *dwValue);                 // DWORD values
      LONG QueryValue(LPCTSTR valueName , BYTE *bValue);                   // BYTE values
      LONG QueryValue(LPCTSTR valueName , LONG *lValue);                   // LONG values
      LONG QueryValue(LPCTSTR valueName , LPSTR szValue, DWORD *pLength);  // char buffers
      LONG QueryValue(LPCTSTR valueName , CString& szValue);               // CString
      LONG QueryBinValue(LPCTSTR valueName , BYTE* binData, DWORD *pLength);// RAW BYTE buffer
      // set value methods
      LONG SetBinValue(LPCTSTR valueName , BYTE* binData, DWORD length);
      LONG SetValue(LPCTSTR valueName , DWORD dwValue);
      LONG SetValue(LPCTSTR valueName , LPCTSTR szValue, DWORD length);
      LONG SetValue(LPCTSTR valueName , const BYTE* binData, DWORD length);
      LONG SetValue(LPCTSTR valueName , CString& stringCString) {const char *pS = stringCString;LONG ret=SetValue(valueName,(const char*)pS, stringCString.GetLength());return(ret);};
      // Reg. management functions
      LONG DeleteValue(LPCTSTR valueName);
      LONG DeleteKey(LPCTSTR keyName = NULL);
      LONG DeleteKeyRecurse(LPCTSTR keyName = NULL);

      LONG EnumKey(CStringArray &csaKeyNames);          // populate a CStringarray with key names
      LONG EnumKey(DWORD index, CString &valueName);         // return the key name at an index
      LONG EnumKey(DWORD index, LPSTR string, DWORD maxSize);// ditto
   private:
      void Init()
      {  m_hKey = NULL; m_baseKey = NULL;};

   public:
      HKEY  m_hKey;

      static HKEY local_machine;   // = HKEY_LOCAL_MACHINE;
      static HKEY current_user;    // = HKEY_CURRENT_USER;

   private:
      LONG result;
      HKEY  m_baseKey;
      CString m_KeyName;
}; // class DRegKey

////////////////////////////////////////////////////////////////////////////////
// Dialog window resizing class
// see CPP file for more

#define IDC_MAIN 0   //Parent dialog ID to be relative window

class CResizer  
{
public:
  enum EBorder{
    eFixed = 1,  //Given distance to specified window side
    eProportional, //Coordinate is changed proportionally to width/height of specified window
    eWidth,   //The width is preserved (relative window and window side are ignored)
    eHeight,  //The height is preserved (relative window and window side are ignored)
  };
  enum ESize{
    eLeft = 1,  //Relative to left side
    eTop,       //Top
    eRight,     //Right
    eBottom,    //Bottom
    eXCenter,   //The center of width
    eYCenter    //The center of height
  };
  struct CBorder{
    EBorder eType;  //Type of relation to relative control side
    int nRelID;     //Relative control
    ESize eRelType; //Side of relative control
  };
  struct CBorderInfo{
    int nID;         //Control ID
    CBorder left;    
    CBorder top;
    CBorder right;
    CBorder bottom;
  };
private:
  struct CControlInfo{
    RECT rcInitial; //initial control position;
    const CBorderInfo *pInfo;
  };
  struct CRectInfo{
    int nID;       //Control ID
    RECT rc;      //New control rect
    bool bVisible; //If control is visible
    bool bHide;    //If control should be hidden because it overlaps senior control
  };
  HWND m_wndParent;
  RECT m_rcInitial; //Initial window client area
  typedef CControlInfo * TInfo;
  typedef CRectInfo * TRectInfo;
  TInfo m_vInfo;
  int m_nSize; //Size of m_vInfo array
  mutable TRectInfo m_vRectInfo;
  mutable int m_nCachedSize; //Size of m_vRectInfo
protected:
  HWND GetDlgItem(int nID) const{return ::GetDlgItem(m_wndParent, nID);}
  void GetDlgItemRect(int nID, RECT &rc) const;
  void GetCachedDlgItemRect(int nID, RECT &rc) const;
  void GetInitialDlgItemRect(int nID, RECT &rc) const;
  void Move(int nIndex) const;
  int GetCoordinate(ESize eType, const RECT &rcInitial, const CBorder &border, const RECT &rc) const;
  static int GetRectCoord(ESize eType, const RECT &rc);
  static int GetRectSize(ESize eType, const RECT &rc);
  int GetRelativeCoord(const CBorder &border) const;
  int Find(int nID) const;
  int FindCached(int nID) const;
  void MoveAndHideOverlapped() const;
  void Clear();
public:
  //Initializes resizer
  bool Init(HWND hWndParent, LPCRECT rcInitial, const CBorderInfo *pBorders, int nSize);
  //Performs moving of controls
  void Move() const;
  //Just constructor
  CResizer();
  //Just destructor
  ~CResizer();
};

/////////////////////////////////////////////////////////////////////////////
// The CDragSizerBmp class implements a bitmap subclassed from a static.
// A mouse-down over the bitmap is used to capture the mouse and then size the 
// parent window, like a window "drag" bar, which posts WM_SIZE to the parent
// untill the mouse is released to another window gains focus.

class CDragSizerBmp : public CWnd
{
// Construction
public:
	CDragSizerBmp();

   BOOL SetBitmapID(int resourceId);

   // mouse cursor type and window corner type specified:
   enum _CursorCornerType
   {
      CORNER_BOTTOMRIGHT   = 0,  // 0=default : IDC_SIZENWSE
      CORNER_BOTTOMLEFT,         // 1=IDC_SIZENESW
      CORNER_TOPRIGHT,           // 2=IDC_SIZENESW
      CORNER_TOPLEFT             // 3=IDC_SIZENESW
   };
   
   BOOL SetCornerType(_CursorCornerType corner, int resourceId, BOOL shiftControl = TRUE);
// Attributes
public:

private:
   CBitmap m_bitMap;
   BOOL    m_buttonDown;
   CPoint  m_downPosition;
   _CursorCornerType m_cornerType;

   virtual void PreSubclassWindow();
   // Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDragSizerBmp)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDragSizerBmp();

	// Generated message map functions
protected:
   void DrawTrans(HDC hDC, int x, int y);
   void DrawTrans(CDC* pDC, int x, int y);
   int GetWidth();
   int GetHeight();

   int m_iWidth;
   int m_iHeight;
   HBITMAP m_hbmMask;    // handle to mask bitmap

   void GetMetrics();
   void CreateMask(HDC hDC);

   DECLARE_DYNAMIC(CDragSizerBmp)
	//{{AFX_MSG(CDragSizerBmp)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif __MYLIB_H__
