/////////////////////////////////////////////////////////////////////////////
//
// FILE: MemoryEditorList.cpp : implementation file
//
// Mod_RSSim (c) Embedded Intelligence Ltd. 1993,2009
// AUTHOR: Conrad Braam.  http://www.plcsimulator.org
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Affero General Public License as
//    published by the Free Software Foundation, either version 3 of the
//    License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Affero General Public License for more details.
//
//    You should have received a copy of the GNU Affero General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Implements a owner data (callback) style CListCtrl
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemoryEditorList.h"
#include "EditMemoryDlg.h"
#include "stringcvt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CHAR * PLCViewerFormats[8] =
{
   "",
   "%d",    //   VIEWFORMAT_DECIMAL =1,
   "%04X",  //   VIEWFORMAT_HEX,
   "%u",    //   VIEWFORMAT_WORD,
   "%u",    //   VIEWFORMAT_DWORD,
   "%ld",   //   VIEWFORMAT_LONG,
   "%f" ,    //   VIEWFORMAT_FLOAT
   "%c"     //   VIEWFORMAT_CHAR
};

CHAR * ViewerFormatNames[8] =
{
   "",
   "decimal",     //   VIEWFORMAT_DECIMAL =1,
   "hex",         //   VIEWFORMAT_HEX,
   "word 16",     //   VIEWFORMAT_WORD,
   "dword 32",    //   VIEWFORMAT_DWORD,
   "long 32",     //   VIEWFORMAT_LONG,
   "float 32",    //   VIEWFORMAT_FLOAT
   "char string"  //   VIEWFORMAT_CHAR
};



/////////////////////////////////////////////////////////////////////////////
// CMemoryEditorList
class CMOD_simDlg;

extern CMOD_simDlg *pGlobalDialog;

CMemoryEditorList::CMemoryEditorList()
{
   for (int i=0;i<sizeof(m_selectedListPos)/sizeof(m_selectedListPos[0])/*MAX_MOD_MEMTYPES */;i++)
      m_selectedListPos[i] = 0;

   m_memAreaDisplayed = (DWORD)-1;
   SetAreaDisplayed(MODBUS_MEM_REGISTERS);
   SetDisplayedWidth(10);
   m_addressFormatHEX = FALSE;
   SetDataViewFormat(VIEWFORMAT_DECIMAL);

   m_backSysColor = GetSysColor(COLOR_WINDOW);
int red=    GetRValue(m_backSysColor),
   blue= GetBValue(m_backSysColor),
   green= GetGValue(m_backSysColor);
   if ((red+blue+green) >= (250*3))
   {
      red=blue=green=155;
   }
int ave = (256-(int)((red+blue+green)/6.0));
   
   red += ave;
   green += ave;
   blue += ave;

   m_backSysColorFade = RGB(red,green, blue);
}

CMemoryEditorList::~CMemoryEditorList()
{

}

// ------------------------------ SetAreaDisplayed -----------------------------
LONG CMemoryEditorList::SetAreaDisplayed(LONG PLCArea)
{
   // save the current place.
POSITION pos;
   
   // make sure that the control at least exists
   if ((DWORD)-1 !=m_memAreaDisplayed)
   {
      pos = GetFirstSelectedItemPosition();
      m_selectedListPos[m_memAreaDisplayed] = GetNextSelectedItem(pos);
#ifdef _UI_DEBUGGING
      CString msg;
      msg.Format("Selection was at %d in %d.\n", m_selectedListPos[m_memAreaDisplayed],m_memAreaDisplayed);
      OutputDebugString(msg);
#endif // _UI_DEBUGGING
   }
   else
      m_selectedListPos[m_memAreaDisplayed] = 0; // first time in here
   m_memAreaDisplayed = PLCArea;
   return (m_memAreaDisplayed);
} // SetAreaDisplayed

// ---------------------------------- GetViewFormat ----------------------------
// return a suitable format for the current data type
CMemoryEditorList::ListViewFormats_ CMemoryEditorList::GetViewFormat()
{
   if ((pGlobalDialog->m_selectedProtocol == PROTOCOL_SELABMASTER232) ||
       (pGlobalDialog->m_selectedProtocol == PROTOCOL_SELAB232))
      return(m_viewFormat);

   //
   switch (m_memAreaDisplayed)
   {
   case MODBUS_MEM_OUTPUTS:
   case MODBUS_MEM_INPUTS:
      return (VIEWFORMAT_DECIMAL);
      break;
   default:
      break;
   }
   return (m_viewFormat);
}

// ----------------------------- GetViewFormatString ---------------------------
LPCTSTR CMemoryEditorList::GetViewFormatString()
{
   // call our member Fn
   return (PLCViewerFormats[GetViewFormat()]);
}

// --------------------------------- GetFormatName ----------------------------
LPCTSTR CMemoryEditorList::GetFormatName()
{
   // call our member Fn
   return (ViewerFormatNames[GetViewFormat()]);
}


/*static */
BOOL CMemoryEditorList::Is32BitFormat(ListViewFormats_ format)
{
   switch (format)
   {
   case VIEWFORMAT_DWORD:
   case VIEWFORMAT_LONG:
   case VIEWFORMAT_FLOAT:
      return(TRUE);
      break;
   case VIEWFORMAT_WORD:
   case VIEWFORMAT_DECIMAL:
      return(FALSE);
      break;
   default:
      ASSERT(0);
      break;
   }
   return(FALSE);
}


// ------------------------------ SetupColumns --------------------------------
// populate the control's headdings
void CMemoryEditorList::SetupColumns(LONG width, BOOL cloneAddressing)
{
int nColumnCount;

   m_displayableWidth = (WORD)width-1;
   m_cloneAddressing = cloneAddressing;
   // start out fresh
   nColumnCount = GetHeaderCtrl()->GetItemCount();

   // Delete all of the columns.
   for (int i=0;i < nColumnCount;i++)
   {
      DeleteColumn(0);
   }

   // set up the column headings on the list-view control
   {   
      LV_COLUMN   lvColumn;
      int         i;
      TCHAR       szString[NUMBER_LISTCOLS][20] = {TEXT(" Address"),  
                                                   TEXT(" +0"), 
                                                   TEXT(" +1"), 
                                                   TEXT(" +2"), 
                                                   TEXT(" +3"), 
                                                   TEXT(" +4"), 
                                                   TEXT(" +5"), 
                                                   TEXT(" +6"), 
                                                   TEXT(" +7"), 
                                                   TEXT(" +8"), 
                                                   TEXT(" +9")};
      if (pGlobalDialog->PLCIsBitMemory(m_memAreaDisplayed))
      {
         // provide headdings for bit addresses
         strcpy(szString[0],  TEXT(" Address") );
         for (i=0 ; i<=15 ; i++)
         {
            if (cloneAddressing)
               sprintf(szString[i+1], TEXT(" +%d"), i );
            else
               sprintf(szString[i+1], TEXT(" +%d"), 15-i );
         }
         strcpy(szString[i+1],  TEXT(" Total") );
         m_displayableWidth++;
      }

      // initialize the columns
      lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
      lvColumn.fmt = LVCFMT_LEFT;
      lvColumn.cx = WIDTH_LISTCOL1;   // 1st col is wider
      for(i = 0; i < (int)m_displayableWidth+1; i++)
      {
         lvColumn.pszText = szString[i];
         
         // work out width for all columns except the "address" column
         if (i!=0)
         {
            if (pGlobalDialog->PLCIsBitMemory(m_memAreaDisplayed))
            { // col width for all bits
               if (i!=17)  // all columns but the last one "total"
               {
                  lvColumn.cx = WIDTH_LISTCOLBIT;  // digitals are narrower
                  if (strlen(szString[i])==3)    // Make 0..9 narrower still
                     lvColumn.cx -=6;   // make it narrower by 5 DLU's
               }
               else
                  lvColumn.cx = WIDTH_LISTCOLTOTAL; // "total" column
            }
            else
            {
               // col width for a register
               lvColumn.cx = WIDTH_LISTCOLS;
            }
         }
         SendMessage(LVM_INSERTCOLUMN, (WPARAM)i, (LPARAM)&lvColumn);
      }

      if (pGlobalDialog->PLCIsBitMemory(m_memAreaDisplayed))
         m_displayableWidth--;     // total column is not data

   }
   // set listview item count to maximum
   DWORD    maximumOffset = pGlobalDialog->GetPLCMemoryLimit(m_memAreaDisplayed);

   WORD numListItem = (WORD)ceil(maximumOffset/*MAX_MOD_MEMWORDS*//((float)m_displayableWidth));
   SendMessage(LVM_SETITEMCOUNT, (WPARAM)(numListItem), (LPARAM)LVSICF_NOINVALIDATEALL);
   
   EnsureVisible(m_selectedListPos[m_memAreaDisplayed], FALSE);
} // SetupColumns


BEGIN_MESSAGE_MAP(CMemoryEditorList, CListCtrl)
	//{{AFX_MSG_MAP(CMemoryEditorList)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	//}}AFX_MSG_MAP
   ON_NOTIFY_REFLECT ( NM_CUSTOMDRAW, OnCustomdrawMyList )
END_MESSAGE_MAP()

void CMemoryEditorList::Format(DWORD memoryValue, ListViewFormats_ format, CString& text)
{
LONG  *pLongVal;
float *pFloatVal;
CString formatting;

   formatting = PLCViewerFormats[format];
   switch (format)
   {
   case CMemoryEditorList::VIEWFORMAT_DECIMAL:
   case CMemoryEditorList::VIEWFORMAT_LONG:
      pLongVal = (LONG*)&memoryValue;
      text.Format(formatting, pLongVal);
      break;
   case CMemoryEditorList::VIEWFORMAT_HEX:
   case CMemoryEditorList::VIEWFORMAT_WORD:
   case CMemoryEditorList::VIEWFORMAT_DWORD:
      text.Format(formatting, memoryValue);
      break;
   case CMemoryEditorList::VIEWFORMAT_FLOAT:
      pFloatVal = (float*)&memoryValue;
      text.Format(formatting, pFloatVal);
      break;
   default:
      text = "???";
      ASSERT(0);
      break;
   }
}

void CMemoryEditorList::Convert(DWORD memoryValue, ListViewFormats_ format, CString& text)
{
   ASSERT(0); // TODO!
}



/////////////////////////////////////////////////////////////////////////////
// CMemoryEditorList message handlers

// --------------------------- OnDblclk ---------------------------------
void CMemoryEditorList::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
POINT pt;
RECT  rect;
CHAR position[80];
LVHITTESTINFO htInfo;
LONG absItemNumber;

   // get cursor position
   GetCursorPos(&pt);
   // convert point to client area co-ordinates
   rect.top = pt.y;
   rect.bottom = pt.y;
   rect.left = pt.x;
   rect.right = pt.x;
   ScreenToClient(&rect);
   // do a hit test
   htInfo.pt.x = rect.left;
   htInfo.pt.y = rect.top;
   SubItemHitTest(&htInfo);   // this will return the Item

   //**** 20021101 add by HF
   if(htInfo.iSubItem == 0) return;
   //**** 20021101 end

   if (htInfo.flags & LVHT_ONITEMLABEL)
   {
   CString description, descrFormat;
   WORD memPrefix;
   CString registerName;

#ifdef _UI_DEBUGGING
      sprintf(position, "\nEditable data columns =%d |", m_displayableWidth);
      OutputDebugString(position);
      sprintf(position, "Selected at %d, %d (%d,%d)\n", rect.left, rect.top, htInfo.iItem, htInfo.iSubItem);
      OutputDebugString(position);
#endif
//      absItemNumber = (htInfo.iItem * m_displayableWidth) + htInfo.iSubItem-1;// work out a zero-based index
//      absItemNumber = (htInfo.iItem * (NUMBER_LISTCOLS-1)) + htInfo.iSubItem-1;
// **** 20021101 mod by HF start
      absItemNumber = (htInfo.iItem * (m_displayableWidth)) + htInfo.iSubItem-1;
//      absItemNumber = (htInfo.iItem * (BITS_IN_WORD-1)) + htInfo.iSubItem-1;
// **** 20021101 mod by HF end
      memPrefix = PLCMemPrefixes[m_memAreaDisplayed];
      descrFormat.Format("Edit %s value at %s", GetFormatName(), GetAddressFormat());
      description.Format(descrFormat, (IsAddressFormatHex()?0:memPrefix*100000) + absItemNumber+1);
      if ((pGlobalDialog->m_seperateRegisters) && 
          ((PROTOCOL_SELMOD232 == pGlobalDialog->m_selectedProtocol)||
           (PROTOCOL_SELMODETH == pGlobalDialog->m_selectedProtocol)))
      {  // work out which station and register # this item belongs to if we are using seperate modbus reg. /station
         registerName.Format("Station #%d , I/O #%d", 
                     (WORD)(absItemNumber / pGlobalDialog->m_numSeperate), 
                     (WORD)(absItemNumber % pGlobalDialog->m_numSeperate));
      }
      else
         if (pGlobalDialog->InPlantSimulation()) // retrieve a "juice-plant" name instead
            pGlobalDialog->GetRegisterName(m_memAreaDisplayed, absItemNumber, registerName);

      if (htInfo.iSubItem)
      {
         // if U double-click on a digital, we just toggle it for U quickly
         if (pGlobalDialog->PLCIsBitMemory(m_memAreaDisplayed))
         {
            if (17 == htInfo.iSubItem)
            {
            DWORD registerValue;
            
               absItemNumber = (htInfo.iItem * (BITS_IN_WORD)) + 0;
               registerValue = pGlobalDialog->GetWordValue(m_memAreaDisplayed, 
                                                           absItemNumber, 
                                                           BITS_IN_WORD);
               descrFormat.Format("Edit %s value at %s", ViewerFormatNames[VIEWFORMAT_HEX], GetAddressFormat());
               description.Format(descrFormat, memPrefix*10000 + absItemNumber);

#ifdef _UI_DEBUGGING
      OutputDebugString(description);
#endif
               {
               // construct the "value editor" dialog to edit "16-bits" of I/O
               CEditMemoryDlg memEditor(PLCViewerFormats[VIEWFORMAT_HEX], 
                                        registerName, 
                                        registerValue, 
                                        VIEWFORMAT_HEX, 
                                        description);
         
                  if (IDOK==memEditor.DoModal())
                  {
                     // unpack the value again into bits
                     registerValue = (DWORD)memEditor.m_value;
                     pGlobalDialog->SetWordValue(m_memAreaDisplayed, 
                                                 absItemNumber, 
                                                 registerValue, 
                                                 BITS_IN_WORD);
                  }
               }
               // re-paint the row of digitals
               RedrawItems(htInfo.iItem, htInfo.iItem);
            }
            else
            {
               descrFormat.Format("Toggle digital at %s",  GetAddressFormat());
               description.Format(descrFormat, (IsAddressFormatHex()? 0 : memPrefix*10000) + absItemNumber);

#ifdef _UI_DEBUGGING
      OutputDebugString(description);
#endif
               if (PLCMemory[m_memAreaDisplayed][absItemNumber])
                  PLCMemory.SetAt(m_memAreaDisplayed, absItemNumber, 0);
               else
                  PLCMemory.SetAt(m_memAreaDisplayed, absItemNumber, 1);
               // re-paint the digital
               RedrawItems(htInfo.iItem, htInfo.iItem);
            }
         }
         else  // Edit the item in a dialog
         {
         DWORD    maximumOffset = pGlobalDialog->GetPLCMemoryLimit(m_memAreaDisplayed);

            if (absItemNumber < (LONG)maximumOffset/*MAX_MOD_MEMWORDS*/)
            {
            CEditMemoryDlg memEditor(GetViewFormatString(), 
                                     registerName, 
                                     pGlobalDialog->GetPLCMemoryValue(m_memAreaDisplayed, absItemNumber, GetViewFormat()), 
                                     GetViewFormat(), 
                                     description);
         
#ifdef _UI_DEBUGGING
      OutputDebugString(description);
#endif
               if (IDOK==memEditor.DoModal())
               {
                  // CMemoryEditorList::ListViewFormats_ CMemoryEditorList::GetViewFormat()
                  if (CMemoryEditorList::Is32BitFormat(GetViewFormat()))
                  {
                  DWORD dwValueToSave = memEditor.m_value;
                     if (CMemoryEditorList::VIEWFORMAT_FLOAT == GetViewFormat())
                     {
                        if (pGlobalDialog->IsClone())
                           SwopWords(&dwValueToSave);          //clone PLC's have a swapped float
                     }

                     PLCMemory.SetAt(m_memAreaDisplayed,
                                  absItemNumber, 
                                  (WORD)(dwValueToSave >>16));
                     PLCMemory.SetAt(m_memAreaDisplayed,
                                  absItemNumber+1, 
                                  (WORD)dwValueToSave );

                     // invalidate the item itself
                     RedrawItems(htInfo.iItem, htInfo.iItem+1);
                  }
                  else
                  {
                     // SET the 16-bit item based on it's display formatting and size
                     PLCMemory.SetAt(m_memAreaDisplayed,
                                  absItemNumber, 
                                  (WORD)memEditor.m_value);
                     // invalidate the item itself
                     RedrawItems(htInfo.iItem, htInfo.iItem);
                  }
               }
            }
         }
      }
      else
      {
         // User clicked in the addresses column.
         if (pGlobalDialog->IsAddressFormatHex())
            pGlobalDialog->OnViewdec();
         else
            pGlobalDialog->OnViewhex();
      }
   }
   else
      sprintf(position, "Clicked at %d, %d", rect.left, rect.top);
   //MessageBox(position);	

	*pResult = 0;
} // OnDblclk

// ---------------------------------- GetDataColumnCount ------------------------------
int CMemoryEditorList::GetDataColumnCount() 
{
int memoryViewWidth = m_displayableWidth;

   return(memoryViewWidth);
}

// ------------------------------------ OnGetdispinfo ---------------------------------
void CMemoryEditorList::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	
   // call my internal function
	*pResult = GetListViewDISPINFO((LPARAM)pNMHDR);
} // OnGetdispinfo


// --------------------------- GetListViewDISPINFO -------------------------
//
LRESULT CMemoryEditorList::GetListViewDISPINFO(LPARAM lParam)
{
LPNMHDR  lpnmh = (LPNMHDR) lParam;
HWND     hwndListView = m_hWnd;
DWORD    absItemNumber;
DWORD    memoryViewWidth;
DWORD    registerValue = 0;
CString  valueFormat;
float    *pFloat32value;
int      viewDataFormat = GetViewFormat();  // fetch user selection
DWORD    maximumOffset = pGlobalDialog->GetPLCMemoryLimit(m_memAreaDisplayed);

LV_DISPINFO *lpdi = (LV_DISPINFO *)lParam;
TCHAR szString[MAX_PATH];
   

   if(lpdi->item.iSubItem)
   {
      // return a sub-item (the data values)
      if(lpdi->item.mask & LVIF_TEXT)
      {
         memoryViewWidth = m_displayableWidth;
         if (pGlobalDialog->PLCIsBitMemory(m_memAreaDisplayed))
         {
            memoryViewWidth--;
            // determine the bit #
            if (17 == lpdi->item.iSubItem)
            {  // the totals column
               viewDataFormat = VIEWFORMAT_WORD;// always display in HEX

               absItemNumber = (lpdi->item.iItem * (BITS_IN_WORD)) ;
               registerValue = pGlobalDialog->GetWordValue(m_memAreaDisplayed, 
                                                           absItemNumber, 
                                                           BITS_IN_WORD);
               valueFormat = PLCViewerFormats[VIEWFORMAT_HEX];       // BIT totals in HEX
            }
            else
            { // individual bit #
               absItemNumber = (lpdi->item.iItem * BITS_IN_WORD) + lpdi->item.iSubItem-1;
               if (absItemNumber < maximumOffset)
               {
                  registerValue = PLCMemory[m_memAreaDisplayed][absItemNumber];
                  valueFormat = PLCViewerFormats[VIEWFORMAT_DECIMAL];   // bits are 0 or 1
               }
            }
         }
         else
         {
            valueFormat = GetViewFormatString();
            // determine the register #
            absItemNumber = (lpdi->item.iItem * (memoryViewWidth)) + lpdi->item.iSubItem-1;
            if (absItemNumber < maximumOffset/*MAX_MOD_MEMWORDS*/)
               registerValue = pGlobalDialog->GetPLCMemoryValue(m_memAreaDisplayed, absItemNumber, GetViewFormat());
            if ((m_cloneAddressing)&&(GetViewFormat() == VIEWFORMAT_FLOAT))
               SwopWords(&registerValue);
         }
         
         if (absItemNumber < maximumOffset/*MAX_MOD_MEMWORDS*/)
         {
            
            // print the contents
            switch (viewDataFormat)
            {
            case VIEWFORMAT_DECIMAL:
               sprintf(szString, valueFormat, (short)registerValue);
               break;
            case VIEWFORMAT_HEX: 
            case VIEWFORMAT_WORD: 
               sprintf(szString, valueFormat, registerValue);
               break;
            case VIEWFORMAT_DWORD:
               sprintf(szString, valueFormat, registerValue);
               break;
            case VIEWFORMAT_LONG:
               sprintf(szString, valueFormat, (LONG)registerValue);
               break;
            case VIEWFORMAT_FLOAT:
               pFloat32value = (float*)&registerValue;
               sprintf(szString, valueFormat, *pFloat32value);
               break;
            case VIEWFORMAT_CHAR:
               ConvertWordToASCII(szString, (WORD)registerValue);
               break;
            default:
               ASSERT(0);
               break;
            }
            lstrcpyn(lpdi->item.pszText, szString, lpdi->item.cchTextMax);
         }
         else
         {
            lstrcpyn(lpdi->item.pszText, "--", lpdi->item.cchTextMax);
         }
      }
   }
   else
   {
   CString addrFormat;
      
      // return address column values
      if(lpdi->item.mask & LVIF_TEXT)
      {
      DWORD addrEndItem, offset = 0;

         absItemNumber = lpdi->item.iItem * (m_displayableWidth) +1 ;//modbus addresses start at 1 not zero
         if ((PROTOCOL_SELABMASTER232 == pGlobalDialog->m_selectedProtocol) ||
             (PROTOCOL_SELAB232 == pGlobalDialog->m_selectedProtocol))
         { // Allen-Bradley
            offset = 0;
         }
         else
         { // modbus
            if (!IsAddressFormatHex()) // decimal address format
            {
               if (absItemNumber>9999) //5-digit offset
                  offset = PLCMemPrefixes[m_memAreaDisplayed]*100000;
               else
                  offset = PLCMemPrefixes[m_memAreaDisplayed]*10000;
            }
         }
         addrFormat.Format("%s-%s", GetAddressFormat(), GetAddressFormat());

         // work out the last address on this line/row
         addrEndItem = absItemNumber + m_displayableWidth-1;
         if (addrEndItem > maximumOffset/*MAX_MOD_MEMWORDS*/)
             addrEndItem = maximumOffset/*MAX_MOD_MEMWORDS*/; //range check it
         
         sprintf(szString, addrFormat, offset+absItemNumber, offset+addrEndItem);
         lstrcpyn(lpdi->item.pszText, szString, lpdi->item.cchTextMax);
      }
   }

   if(lpdi->item.mask & LVIF_IMAGE)
   {
      lpdi->item.iImage = 0;
   }

   if(lpdi->item.mask & LVIF_INDENT)
   {
      lpdi->item.iIndent = 0;
   }
   
   return 0;
} // GetListViewDISPINFO

// ------------------------------- OnCustomdrawMyList ---------------------
// owner-draw code
void CMemoryEditorList::OnCustomdrawMyList ( NMHDR* pNMHDR, LRESULT* pResult )
{
NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

    // Take the default processing unless we set this to something else below.
    *pResult = CDRF_DODEFAULT;

    // First thing - check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.

    if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
        {
        *pResult = CDRF_NOTIFYITEMDRAW;
        }
    else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
        {
        // This is the notification message for an item.  We'll request
        // notifications before each subitem's prepaint stage.

        *pResult = CDRF_NOTIFYSUBITEMDRAW;
        }
    else if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage )
        {
        // This is the prepaint stage for a subitem. Here's where we set the
        // item's text and background colors. Our return value will tell 
        // Windows to draw the subitem itself, but it will use the new colors
        // we set here.
        // The text color will cycle through red, green, and light blue.
        // The background color will be light blue for column 0, red for
        // column 1, and black for column 2.
    
         COLORREF crText, crBkgnd;
        
         if (( 0 == pLVCD->iSubItem ) || (17 == pLVCD->iSubItem) || (!pGlobalDialog->PLCIsBitMemory(m_memAreaDisplayed)) )
         {
            crText = RGB(0,0,0);
            crBkgnd = m_backSysColor;//RGB(255,255,255);
         }
         else 
         {  // Display a data item, which is a BIT data type:
            LV_DISPINFO di;
            CHAR chText[20];
            int    nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );

            di.item.iItem = nItem;
            di.item.iSubItem = pLVCD->iSubItem;
            di.item.mask = LVIF_TEXT;
            di.item.pszText = chText;
            di.item.cchTextMax = sizeof(chText);

            GetListViewDISPINFO((LPARAM)&di);
            if (di.item.pszText[0] == '1')
            {  // display digital 1 with gray background for better LCD contrast
               crText = RGB(0,0,0);
               crBkgnd = m_backSysColorFade;//RGB(205,205,205);//m_backSysColor;
            }
            else
            {
               crText = RGB(0,0,0);
               crBkgnd = m_backSysColor;
            }
        }
        // Store the colors back in the NMLVCUSTOMDRAW struct.
        pLVCD->clrText = crText;
        pLVCD->clrTextBk = crBkgnd;

        // Tell Windows to paint the control itself.
        *pResult = CDRF_DODEFAULT;
   }
}