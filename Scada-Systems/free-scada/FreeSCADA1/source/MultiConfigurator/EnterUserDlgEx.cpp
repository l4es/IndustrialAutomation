#include "StdAfx.h"
#include ".\enteruserdlgex.h"

namespace ModuleArchive
{
	LRESULT CEnterUserDlgEx::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		
		if(wID == IDOK)
		{
			DoDataExchange(DDX_SAVE);
			if(m_User.IsEmpty())
			{
				MessageBox(L"Incorrect user name", L"Input Error",	MB_OK);
				return 0;
			}

			if(m_Pass != m_RepeatPass)
			{
				MessageBox(L"Passwords mismatch", L"Input Error",	MB_OK);
				return 0;
			}
		}
		
		EndDialog(wID);
		return 0;
	}

	LRESULT CEnterUserDlgEx::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(!m_EnableUser)
			GetDlgItem(IDC_USER).EnableWindow(FALSE);
		DoDataExchange(DDX_LOAD);
		CenterWindow(GetParent());
		return 0;
	}

	LRESULT CEnterUserDlgEx::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		EndDialog(IDCANCEL);
		return TRUE;
	}
}

