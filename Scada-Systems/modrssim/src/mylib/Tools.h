#ifndef TOOLS_H
#define TOOLS_H


///////////////////////////////////////////////////////////////////////////////
// Usefull macros
//
#define KEYDOWN(Key) ((GetKeyState(Key)&0x8000)!=0)
#define VIRTUAL_KEY(nChar,nFlags) (((nFlags)&0x0100)||(nChar)<=VK_ESCAPE||(nChar)>=VK_F1)
#define ANSI_KEY(nChar) ((nChar)>VK_ESCAPE)
#define VK_CTRL_A 1
#define VK_CTRL_Z 26
#define VK_ALT VK_MENU

// To not have to type the 4 fields !
#define EXPAND_RECT(rc) (rc).left,(rc).top,(rc).right,(rc).bottom


///////////////////////////////////////////////////////////////////////////////
// CRect of the window's client area
///////////////////////////////////////////////////////////////////////////////
class CClientRect : public CRect
{
public:
    CClientRect (HWND hWnd)
    {
        ::GetClientRect (hWnd, this);
    };
    CClientRect (const CWnd* pWnd)
    {
        ::GetClientRect (pWnd->GetSafeHwnd(), this);
    };
};


///////////////////////////////////////////////////////////////////////////////
// CRect of the window
///////////////////////////////////////////////////////////////////////////////
class CWindowRect : public CRect
{
public:
    CWindowRect (HWND hWnd)
    {
        ::GetWindowRect (hWnd, this);
    };
    CWindowRect (const CWnd* pWnd)
    {
        ::GetWindowRect (pWnd->GetSafeHwnd(), this);
    };
};


#endif // TOOLS_H
