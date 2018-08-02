#pragma once


// CSelfRegistrationDlg dialog

class CSelfRegistrationDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelfRegistrationDlg)

public:
	CSelfRegistrationDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelfRegistrationDlg();

	void OnDetailsChanged();
// Dialog Data
	enum { IDD = IDD_REGISTRATIONDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeRegistrationKey();
	afx_msg void OnEnChangeUsername();
	CString m_username;
	CString m_registrationkey;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
