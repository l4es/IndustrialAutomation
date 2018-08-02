/*****************************************************************************
 *
 *  Filename : DRAGSIZERBMP.CPP
 *
 *****************************************************************************
 *
 *  Copyright : (c) Embedded Intelligence Ltd. 1993,2009
 *
 *****************************************************************************
 *
 *  Description : This is the implementation for CResizer class .
 *
 * ORIGINAL HEADDER (modified):
 *
 * Class to correctly move child windows after parent window was resized
 *
 *  Copyright: (C)2001 Dmitry Kochin <dco@mail.ru>
 * Created: 06/07/01 by dukei@
 *
 *Each child window side (left, top, right and bottom) is connected to a side of another window,
 *so called relative window. It is typically dialog window, that owns the child window.
 *When dialog window is resized, child window sides are moved after the relative window,
 *preserving the connections. 
 *
 *Typical usage:
 *
 * 1. CResizer m_resizer is member variable;
 *
 * 2. Add the following code to OnInitDialog() 
 * (replacing control IDs to your specific ones).
 * See array format description later in the comment
 *  
 *  static CResizer::CBorderInfo s_bi[] = {
 *    {IDC_CONTROL_ID,      {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},  *Left side
 *                          {CResizer::eFixed, IDC_MAIN, CResizer::eTop},   *Top side
 *                          {CResizer::eFixed, IDC_MAIN, CResizer::eLeft},  *Right side
 *                          {CResizer::eFixed, IDC_MAIN, CResizer::eTop}},  *Bottom side
 *    {IDC_STATIC_ID,       {CResizer::eFixed, IDC_MAIN, CResizer::eLeft}, 
 *                          {CResizer::eFixed, IDC_MAIN, CResizer::eTop},  
 *                          {CResizer::eFixed, IDC_MAIN, CResizer::eRight},
 *                          {CResizer::eFixed, IDC_MAIN, CResizer::eTop}}, 
 *  };
 *  const nSize = sizeof(s_bi)/sizeof(s_bi[0]);
 *  m_resizer.Init(m_hWnd, NULL, s_bi, nSize);
 *
 * 3. Add the following code to OnSize() handler
 *
 *  m_resizer.Move();
 *
 * 4. Everything should work now
 *
 * P.S. Data array format consists of one or more CBorderInfo structures, which contains
 * moved control ID (first field) and four CBorder structures, 
 * for left, top, right and bottom sides of moved control accordingly.
 *
 * The main difficulty is to understand CBorder structure, which has the following format:
 * 
 * {<how control side is connected to side of another window>, <another window id>, 
 *                  <side of another window, to which a control side is connected>}
 * 
 *     CResizer::eFixed                                  CResizer::eLeft 
 *     CResizer::eProportional  IDC_MAIN                 CResizer::eTop   
 * or {CResizer::eWidth       , IDC_ANOTHER_CONTROL_ID , CResizer::eRight   }
 *     CResizer::eHeight                                 CResizer::eBottom
 *                                                       CResizer::eXCenter
 *                                                       CResizer::eYCenter
 *
 * For example, {CResizer::eFixed, IDC_MAIN, CResizer::eLeft} means, that moved control side is
 * on the fixed distance (CResizer::eFixed) from left side (CResizer::eLeft) of dialog window (IDC_MAIN)
 *
 * Another example: {CResizer::eProportional, IDC_CONTROL_ID, CResizer::eLeft} means, that
 * moved control side preserves relation (proportionaly) (CResizer::eProportional)
 * to the width (CResizer::eLeft or CResizer::eRight) of control IDC_CONTROL_ID.
 *
 * TIP: Resizer resizes controls in the order they are defined in the array, so
 * <another window id> should always be defined (and, therefore, moved by the resizer) before
 * it is used as relative window. Otherwise, resizer ASSERTs.
 *
 *******************************************************************/

#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResizer::CResizer()
{
  memset(&m_rcInitial, 0, sizeof(m_rcInitial));
  m_vInfo = NULL;
  m_vRectInfo = NULL;
  m_nSize = 0;
  m_nCachedSize = 0;
}

CResizer::~CResizer()
{
  Clear();
}

void CResizer::Clear()
{
  delete [] m_vInfo;
  delete [] m_vRectInfo;
  m_vInfo = NULL;
  m_vRectInfo = NULL;
  m_nSize = 0;
  m_nCachedSize = 0;
}

bool CResizer::Init(HWND hWndParent, LPCRECT rcInitial, const CResizer::CBorderInfo *pBorders, int nSize)
{
  Clear();
  m_vInfo = new CControlInfo[nSize];
  m_nSize = nSize;

  m_wndParent = hWndParent;
  
  if(rcInitial == NULL)
    ::GetClientRect(m_wndParent, &m_rcInitial);
  else
    ::CopyRect(&m_rcInitial, rcInitial);
  
  for(int i=0; i<m_nSize; i++){
    CControlInfo &ci = m_vInfo[i];
    ci.pInfo = pBorders + i;
    GetDlgItemRect(ci.pInfo->nID, ci.rcInitial);
    
#ifdef _DEBUG
    //Make some debug checking
    //Check that no controls have reserved IDs
    //IDC_MAIN == 0 isn't allowed for control identifiers!
    //Set another control ID with CWindow::SetDlgCtrlID() or ::SetWindowLong(m_hWnd, GWL_ID, nID)!
    _ASSERTE(ci.pInfo->nID != IDC_MAIN); 
    //Check that this control ID is unique.
    //ALL control identifiers MUST BE UNIQUE!!!
    for(int j=0; j < i; j++){
      const CControlInfo &ciPrevious = m_vInfo[j];
      _ASSERTE(ciPrevious.pInfo->nID != ci.pInfo->nID); //Duplicated control ID!!!
      //Control j in initialization array has the same id as control i.
    }
#endif
  }
  return true;
}

void CResizer::Move() const{
  if(m_vRectInfo == NULL && m_nSize > 0)
    m_vRectInfo = new CRectInfo[m_nSize];
  
  for(int i=0; i<m_nSize; i++){
    //Move(i) should be called always for i=0 to m_nSize!!
    //It is required by its implementation!
    Move(i);
  }
  
  MoveAndHideOverlapped();
}

void CResizer::Move(int nIndex) const{
  const CControlInfo &ci = m_vInfo[nIndex];
  CRectInfo &ri = m_vRectInfo[nIndex];
  m_nCachedSize = nIndex + 1; //Now m_vRectInfo contains nIndex + 1 valid items
  
  ri.nID = ci.pInfo->nID;
  
  RECT &rc = ri.rc;
  
  rc.left = GetCoordinate(eLeft, ci.rcInitial, ci.pInfo->left, rc);
  rc.top = GetCoordinate(eTop, ci.rcInitial, ci.pInfo->top, rc);
  rc.right = GetCoordinate(eRight, ci.rcInitial, ci.pInfo->right, rc);
  rc.bottom = GetCoordinate(eBottom, ci.rcInitial, ci.pInfo->bottom, rc);
  
  HWND pCtl = GetDlgItem(ci.pInfo->nID);
  LONG dwStyle = ::GetWindowLong(pCtl, GWL_STYLE);
  ri.bVisible = (::IsWindowVisible(pCtl) != FALSE && (dwStyle&WS_CLIPSIBLINGS) == 0);
  ri.bHide = false;
}

int CResizer::GetCoordinate(ESize eType, const RECT &rcInitial, const CBorder &border, const RECT &rc) const{
  int nOld = GetRectCoord(eType, rcInitial);
  switch(border.eType){
  case eFixed:
    {
      //Get initial relative window position
      RECT rc;
      GetInitialDlgItemRect(border.nRelID, rc);
      int nRelOld = GetRectCoord(border.eRelType, rc);
      //Get current relative window position
      int nRelNew = GetRelativeCoord(border);
      //Compute and return new position
      return nOld - nRelOld + nRelNew;
    }
  case eProportional:
    {
      //Get initial relative window position
      RECT rcOld;
      GetInitialDlgItemRect(border.nRelID, rcOld);
      int nOldSize = GetRectSize(eType, rcOld);
      int nOldBase = GetRectCoord(border.eRelType, rcOld);
      //Get current relative window position
      RECT rcNew;
      GetCachedDlgItemRect(border.nRelID, rcNew);
      int nNewSize = GetRectSize(eType, rcNew);
      int nNewBase = GetRectCoord(border.eRelType, rcNew);
      //Compute and return new position
      return nNewBase + (nOld - nOldBase)*nNewSize/(nOldSize <= 0 ? 1 : nOldSize);
    }
  case eWidth:
    {
      return rc.left + rcInitial.right - rcInitial.left;
    }
  case eHeight:
    {
      return rc.top + rcInitial.bottom - rcInitial.top;
    }
  }
  _ASSERTE(FALSE); //Wrong relation type is specified. Use items from EBorder enum.
  return 0;
}

int CResizer::GetRectCoord(ESize eType, const RECT &rc){
  switch(eType){
  case eLeft:
    return rc.left;
  case eTop:
    return rc.top;
  case eRight:
    return rc.right;
  case eBottom:
    return rc.bottom;
  case eXCenter:
    return (rc.right + rc.left)/2;
  case eYCenter:
    return (rc.bottom + rc.top)/2;
  }
  _ASSERTE(FALSE); //Wrong side is specified. Use items from ESize enum.
  return 0;
}

int CResizer::GetRectSize(ESize eType, const RECT &rc){
  switch(eType){
  case eLeft:
  case eRight:
  case eXCenter:
    return rc.right - rc.left;
  case eTop:
  case eBottom:
  case eYCenter:
    return rc.bottom - rc.top;
  }
  _ASSERTE(FALSE); // Wrong side is specified. Use items from ESize enum.
  return 0;
}

int CResizer::GetRelativeCoord(const CBorder &border) const{
  RECT rc;
  GetCachedDlgItemRect(border.nRelID, rc);
  return GetRectCoord(border.eRelType, rc);
}

void CResizer::GetDlgItemRect(int nID, RECT &rc) const{
  switch(nID){
  case IDC_MAIN:
    {
      ::GetClientRect(m_wndParent, &rc);
      break;
    }
  default:
    {
      HWND pCtl = GetDlgItem(nID);
      ::GetWindowRect(pCtl, &rc);
      
      POINT pt1, pt2;
      pt1.x = rc.left, pt1.y = rc.top;
      pt2.x = rc.right, pt2.y = rc.bottom;
      ::ScreenToClient(m_wndParent, &pt1);
      ::ScreenToClient(m_wndParent, &pt2);
      rc.left = pt1.x, rc.top = pt1.y, rc.right = pt2.x, rc.bottom = pt2.y;
      break;
    }
  }
}

void CResizer::GetCachedDlgItemRect(int nID, RECT &rc) const{
  switch(nID){
  case IDC_MAIN:
    {
      GetDlgItemRect(nID, rc);
      break;
    }
  default:
    {
      int i = FindCached(nID);
      rc = m_vRectInfo[i].rc;
      break;
    }
  }
}

void CResizer::GetInitialDlgItemRect(int nID, RECT &rc) const{
  switch(nID){
  case IDC_MAIN:
    {
      rc = m_rcInitial;
      break;
    }
  default:
    {
      //Get initial relative window position
      int i = Find(nID);
      rc = m_vInfo[i].rcInitial;
      break;
    }
  }
}

int CResizer::Find(int nID) const{
  for(int i=0; i<m_nSize; i++){
    if(m_vInfo[i].pInfo->nID == nID)
      return i;
  }
  _ASSERTE(FALSE); //Possibly control id nID wasn't defined before it is used
  //as relative window. Read the TIP in the header file resizer.h
  return -1;
}

int CResizer::FindCached(int nID) const{
  for(int i=0; i<m_nCachedSize; i++){
    if(m_vRectInfo[i].nID == nID)
      return i;
  }
  _ASSERTE(FALSE); //Possibly control id nID wasn't defined before it is used
  //as relative window. Read the TIP in the header file resizer.h
  return -1;
}

void CResizer::MoveAndHideOverlapped() const{
  _ASSERTE(m_nSize == m_nCachedSize);
  for(int i=0; i<m_nSize; i++){
    CRectInfo &riSlave = m_vRectInfo[i];
    for(int j=0; j<i; j++){
      const CRectInfo &riMaster = m_vRectInfo[j];
      //if senior window is visible and intersects with juniur, junior is to be hidden
      if(riMaster.bVisible && riSlave.bVisible && !riSlave.bHide){ 
        RECT rc;
        ::IntersectRect(&rc, &riMaster.rc, &riSlave.rc);
        riSlave.bHide = !::IsRectEmpty(&rc);
      }
    }
    //if the window doesn't intersect with seniors, it can be drawn
    HWND pCtl = GetDlgItem(riSlave.nID);
    if(riSlave.bHide)
      ::SetRectEmpty(&riSlave.rc);
    //SWP_NOCOPYBITS is obligatory, otherwise windows don't correctly redraw itselves
    ::SetWindowPos(pCtl, NULL, riSlave.rc.left, riSlave.rc.top, 
      riSlave.rc.right - riSlave.rc.left, riSlave.rc.bottom - riSlave.rc.top, 
      SWP_NOCOPYBITS|SWP_NOZORDER);
  }
}
