// SelfRegistrationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mod_RSsim.h"
#include "SelfRegistrationDlg.h"


// CSelfRegistrationDlg dialog

IMPLEMENT_DYNAMIC(CSelfRegistrationDlg, CDialog)

CSelfRegistrationDlg::CSelfRegistrationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelfRegistrationDlg::IDD, pParent)
	, m_username(_T(""))
	, m_registrationkey(_T(""))
{

}

CSelfRegistrationDlg::~CSelfRegistrationDlg()
{
}

void CSelfRegistrationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USERNAME, m_username);
	DDV_MaxChars(pDX, m_username, 60);
	DDX_Text(pDX, IDC_REGISTRATION_KEY, m_registrationkey);
	DDV_MaxChars(pDX, m_registrationkey, 8);
}


BEGIN_MESSAGE_MAP(CSelfRegistrationDlg, CDialog)
	ON_EN_CHANGE(IDC_REGISTRATION_KEY, &CSelfRegistrationDlg::OnEnChangeRegistrationKey)
	ON_EN_CHANGE(IDC_USERNAME, &CSelfRegistrationDlg::OnEnChangeUsername)
	ON_BN_CLICKED(IDOK, &CSelfRegistrationDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSelfRegistrationDlg message handlers

void CSelfRegistrationDlg::OnEnChangeRegistrationKey()
{
	OnDetailsChanged();
}

void CSelfRegistrationDlg::OnEnChangeUsername()
{
	OnDetailsChanged();
}

void CSelfRegistrationDlg::OnDetailsChanged()
{
	UpdateData(1);
	CWnd *okButton = GetDlgItem(IDOK);
		
	if (okButton)
	{
		okButton->EnableWindow(CRegistrationTest::CheckRegistrationKey(m_username, m_registrationkey, TRUE));
	}
}

BOOL CSelfRegistrationDlg::OnInitDialog()
{
CString registryPath;
DRegKey key;

	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	registryPath = APPREGISTRY_SETTINGSKEY;

	key.Open(DRegKey::current_user, registryPath);
	key.QueryValue("RegistrationUserName", m_username);
	key.QueryValue("RegistrationKey",      m_registrationkey);
	
	UpdateData(FALSE);
	OnDetailsChanged();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSelfRegistrationDlg::OnBnClickedOk()
{
CString registryPath;
DRegKey key;
	// TODO: Add your control notification handler code here
	UpdateData(1);
   registryPath = APPREGISTRY_SETTINGSKEY;

   key.Open(DRegKey::current_user, registryPath);
   key.SetValue("RegistrationUserName",   m_username);
   key.SetValue("RegistrationKey",        m_registrationkey);

	OnOK();
}
