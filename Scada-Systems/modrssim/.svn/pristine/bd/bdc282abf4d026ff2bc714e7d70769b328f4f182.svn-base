/////////////////////////////////////////////////////////////////////////////
//
// FILE: EditMemoryDlg.cpp : implementation file
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
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditMemoryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditMemoryDlg dialog


CEditMemoryDlg::CEditMemoryDlg(LPCTSTR formatting, 
                               LPCTSTR registerName, // the 'name' if this is a juice-plant register
                               DWORD memoryValue, 
                               WORD valueType, 
                               LPCTSTR description,
                               CWnd* pParent /*=NULL*/)
	: CDialog(CEditMemoryDlg::IDD, pParent)
{
   m_formatting = formatting; // HEX etc.
   m_description = description;
   m_value = memoryValue;
   m_valueType = valueType;
   m_registerName = registerName;

	//{{AFX_DATA_INIT(CEditMemoryDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEditMemoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditMemoryDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditMemoryDlg, CDialog)
	//{{AFX_MSG_MAP(CEditMemoryDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditMemoryDlg message handlers

BOOL CEditMemoryDlg::OnInitDialog() 
{
CString valStr;
LONG  *pLongVal;
float *pFloatVal;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
   switch (m_valueType)
   {
   case CMemoryEditorList::VIEWFORMAT_DECIMAL:
   case CMemoryEditorList::VIEWFORMAT_LONG:
      pLongVal = (LONG*)&m_value;
      valStr.Format(m_formatting, m_value);
      break;
   case CMemoryEditorList::VIEWFORMAT_HEX:
   case CMemoryEditorList::VIEWFORMAT_WORD:
   case CMemoryEditorList::VIEWFORMAT_DWORD:
      valStr.Format(m_formatting, m_value);
      break;
   case CMemoryEditorList::VIEWFORMAT_FLOAT:
      pFloatVal = (float*)&m_value;
      valStr.Format(m_formatting, *pFloatVal);
      break;
   case CMemoryEditorList::VIEWFORMAT_CHAR:
      ConvertWordToASCIICS(valStr, (WORD)m_value);
      break;
   default:
      ASSERT(0);
      break;
   }
   SetDlgItemText(IDC_VALUE, valStr);
   SetWindowText(m_description);

   SetDlgItemText(IDC_REGISTERNAME, m_registerName);

   ((CEdit*)GetDlgItem(IDC_VALUE))->SetSel(0, -1);
   GetDlgItem(IDC_VALUE)->SetFocus();
   return (FALSE);
	//return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditMemoryDlg::OnOK() 
{
CString text;
LONG  *pLongVal;
float *pFloatVal;

	// TODO: Add extra validation here
   GetDlgItemText(IDC_VALUE, text);
     
   switch (m_valueType)
   {
   case CMemoryEditorList::VIEWFORMAT_DECIMAL:
   case CMemoryEditorList::VIEWFORMAT_LONG:
      pLongVal = (LONG*)&m_value;
      sscanf(text,m_formatting, pLongVal);
      break;
   case CMemoryEditorList::VIEWFORMAT_HEX:
   case CMemoryEditorList::VIEWFORMAT_WORD:
   case CMemoryEditorList::VIEWFORMAT_DWORD:
      sscanf(text, m_formatting, &m_value);
      break;
   case CMemoryEditorList::VIEWFORMAT_FLOAT:
      pFloatVal = (float*)&m_value;
      sscanf(text, m_formatting, &m_value);
      break;
   case CMemoryEditorList::VIEWFORMAT_CHAR:
      //pLongVal = (LONG*)&m_value;
      //sscanf(text, m_formatting, m_value);
      {
         WORD w;
      ConvertASCIIToWord(text, w);
      m_value = w;
      }
      break;
   default:
      ASSERT(0);
      break;
   }
	CDialog::OnOK();
}
