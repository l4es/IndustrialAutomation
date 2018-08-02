// CSVFileImportDlg.cpp : implementation file
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
// CREDIT: uses a edit control by JP Arendse
// http://www.codeproject.com/KB/edit/fileeditctrl.aspx

#include "stdafx.h"
#include "mod_rssim.h"
#include "CSVFileImportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCSVFileImportDlg dialog


CCSVFileImportDlg::CCSVFileImportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCSVFileImportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCSVFileImportDlg)
	m_csvImportEnable = FALSE;
	m_importFolder = _T("");
	m_logFileName = _T("");
	//}}AFX_DATA_INIT
}


void CCSVFileImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCSVFileImportDlg)
	DDX_Check(pDX, IDC_IMPORTENABLED, m_csvImportEnable);
	DDX_Text(pDX, IDC_IMPORTFOLDER, m_importFolder);
	DDX_Text(pDX, IDC_IMPORTLOGNAME, m_logFileName);
	//}}AFX_DATA_MAP
   DDX_FileEditCtrl(pDX, IDC_IMPORTFOLDER, m_FolderEditCtrl, FEC_FOLDER);
   DDX_FileEditCtrl(pDX, IDC_IMPORTLOGNAME, m_FileEditCtrl, FEC_FILESAVEAS);
   if (pDX->m_bSaveAndValidate)
   {
   SYSTEMTIME sysTime;
   GetLocalTime(&sysTime);
   CString yFolder;
   int pos;
      yFolder.Format("%04d%02d%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay);
      pos = m_importFolder.Find(yFolder);
      if (pos>1)
      {
         m_importFolder = m_importFolder.Left(pos-1);
      }
   }
}


BEGIN_MESSAGE_MAP(CCSVFileImportDlg, CDialog)
	//{{AFX_MSG_MAP(CCSVFileImportDlg)
	ON_BN_CLICKED(IDC_QUICKIMPORT, OnQuickImport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCSVFileImportDlg message handlers

void CCSVFileImportDlg::OnOK() 
{
   UpdateData();

    CString str;

    // CSV folder-name
     POSITION pos = m_FolderEditCtrl.GetStartPosition();
    ASSERT(pos);
     str = m_FileEditCtrl.GetNextPathName(pos);
     m_importFolder = str;

     // log filename
     pos = m_FileEditCtrl.GetStartPosition();
    ASSERT(pos);

     str = m_FileEditCtrl.GetNextPathName(pos);
     m_logFileName = str;

	CDialog::OnOK();
}

void CCSVFileImportDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

BOOL CCSVFileImportDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
   UpdateData(FALSE);

   CRect rect;
   m_FolderEditCtrl.GetClientRect(rect);
   m_FolderEditCtrl.SetButtonWidth(rect.Height());
   m_FileEditCtrl.SetButtonWidth(rect.Height());

   m_FolderEditCtrl.SetWindowText(this->m_importFolder);
   m_FileEditCtrl.SetWindowText(this->m_logFileName);

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// ---------------------------------------- OnQuickImport ------------------------------
// allow user to manually load a file
void CCSVFileImportDlg::OnQuickImport() 
{
	// TODO: Add your control notification handler code here
static char BASED_CODE szFilter[] = "CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||";
CString initialDir, fullFileName;

   UpdateData(TRUE);
	CFileDialog dlg(TRUE, "csv", 0, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST ,szFilter, this);

   initialDir = m_importFolder;
   dlg.m_ofn.lpstrInitialDir = initialDir.GetBuffer(0);

   int ret = dlg.DoModal();
   if (IDOK == ret)
   {
      fullFileName = dlg.GetPathName();
      CCSVTextImporter  importer;
      CRegisterUpdaterIMP impl((CMOD_simDlg *)GetParent());

      if (importer.ImportFile(fullFileName, TRUE, &impl))
      {
            importer.UpdateRegisters();
      }
   }
   initialDir.ReleaseBuffer();
}
