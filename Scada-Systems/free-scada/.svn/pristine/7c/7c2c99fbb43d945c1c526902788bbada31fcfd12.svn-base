#include "stdafx.h"
#include ".\schemadialog.h"

CSchemaDialog::CSchemaDialog(void)
{
	m_Name = "";
	m_FixedSize = false;
	m_HasImageBackgrd = false;
	m_ImageName = "";
	m_StretchBackGrd = false;
	m_Height = 0;
	m_Width = 0;
	m_BGDColor = cBackgroundColor;
}

CSchemaDialog::~CSchemaDialog(void)
{
}

LRESULT CSchemaDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DlgResize_Init(true, true, NULL);
	DoDataExchange(DDX_LOAD);
	UIAddChildWindowContainer(m_hWnd);
	UIEnable(IDC_WIDTH,	m_FixedSize, TRUE);
	UIEnable(IDC_HEIGHT, m_FixedSize, TRUE);
	UIEnable(IDC_IMAGE_NAMES, m_HasImageBackgrd, TRUE);
	UIEnable(IDC_STRETCH_IMAGE, m_HasImageBackgrd, TRUE);

	if(!m_ListImage.empty())
	{
		CComboBox box = (CComboBox)GetDlgItem(IDC_IMAGE_NAMES);
		box.ResetContent();
		for(TStrings::iterator it = m_ListImage.begin(); it != m_ListImage.end(); it++)
		{
			box.AddString(it->c_str());
		}
		if(m_ImageName != "")
			box.SelectString(0, m_ImageName);
	}

	_Brush.CreateSolidBrush(m_BGDColor.AsCOLORREF());
	_ColorStatic = (CStatic)GetDlgItem(IDC_COLOR_BGD);
	_ColorStatic.RedrawWindow();
	
	
	UIUpdateChildWindows();
	CenterWindow(GetParent());
	return TRUE;
}


LRESULT CSchemaDialog::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(DDX_SAVE);
	EndDialog(wID);
	return 0;
}


LRESULT CSchemaDialog::OnFixedSize(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CButton Check = (CButton)GetDlgItem(IDC_FIXED_SIZE);
	m_FixedSize = Check.GetCheck();
	UIEnable(IDC_WIDTH,	m_FixedSize, TRUE);
	UIEnable(IDC_HEIGHT,m_FixedSize, TRUE);
	UIUpdateChildWindows();
	return 0;
}

LRESULT CSchemaDialog::OnImageBackgrd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CButton Check = (CButton)GetDlgItem(IDC_IMAGE_BACKGRD);
	m_HasImageBackgrd = Check.GetCheck();
	UIEnable(IDC_IMAGE_NAMES, m_HasImageBackgrd, TRUE);
	UIEnable(IDC_STRETCH_IMAGE, m_HasImageBackgrd, TRUE);
	UIUpdateChildWindows();
	return 0;
}

LRESULT CSchemaDialog::OnStretchBackGrd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CButton Check = (CButton)GetDlgItem(IDC_STRETCH_IMAGE);
	m_StretchBackGrd = Check.GetCheck();
	return 0;
}

void CSchemaDialog::LoadImageNames(TStrings Names)
{
	m_ListImage = Names;
}

LRESULT CSchemaDialog::OnCbnSelchange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComboBox box = (CComboBox)GetDlgItem(IDC_IMAGE_NAMES);
	wchar_t buffer[0xff];
	box.GetLBText(box.GetCurSel(), buffer);
	m_ImageName = CString(buffer);
	return 0;
}


LRESULT CSchemaDialog::OnBrowseColor(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;

	dlg.m_cc.rgbResult = m_BGDColor.AsCOLORREF();
	if(dlg.DoModal() == IDOK)
	{
		_Brush.DeleteObject();
		_Brush.CreateSolidBrush(dlg.m_cc.rgbResult);
		_ColorStatic.RedrawWindow();

		m_BGDColor = dlg.m_cc.rgbResult;

	}
	return 0;
}


LRESULT CSchemaDialog::OnCtlColorStatic(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	if((HWND)lParam == GetDlgItem(IDC_COLOR_BGD))
		return (LRESULT)_Brush.m_hBrush;
	bHandled = FALSE;
	return 0;
}


