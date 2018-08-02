#include "StdAfx.h"
#include "Helpers.h"
#include <WTypes.h>

#define _ATL_DISABLE_DEPRECATED
#include <atlbase.h>

namespace HelpersLib
{
	using namespace std;

	void RegistryFileExt(std::wstring Ext, std::wstring DocID, std::wstring Cmd, std::wstring Icon, std::wstring Type)
	{
		ATLASSERT(!Ext.empty());
		ATLASSERT(!DocID.empty());
		ATLASSERT(!Cmd.empty());
		ATLASSERT(!Type.empty());

		CRegKey Reg;
		Reg.Create(HKEY_CLASSES_ROOT, Ext.c_str());
		Reg.SetValue(DocID.c_str(),L"");
		Reg.Close();

		Reg.Create(HKEY_CLASSES_ROOT, DocID.c_str());
		Reg.SetValue(DocID.c_str(),L"");
		Reg.Close();

		std::wstring tmp;
		if(!Icon.empty())
		{
			tmp = DocID+L"\\DefaultIcon";
			Reg.Create(HKEY_CLASSES_ROOT, tmp.c_str());
			Reg.SetValue(Icon.c_str(),L"");
			Reg.Close();
		}

		tmp = DocID+L"\\shell\\"+Type+L"\\command";
		Reg.Create(HKEY_CLASSES_ROOT, tmp.c_str());
		Reg.SetValue(Cmd.c_str(), L"");
		Reg.Close();
	}

	std::wstring ExtractFilePath(std::wstring FileName)
	{
		for(size_t i=FileName.size()-1;i>=0;i--)
		{
			if(FileName[i] == L'\\')
			{
				std::wstring res(FileName.begin(), FileName.begin()+i);
				return res;
			}
		}

		return FileName;
	};

	BOOL TestAcceptingOS(DWORD OS)
	{
#define SHOW_MSG(msg) MessageBox(NULL, msg, _T("Информация"), MB_OK|MB_ICONSTOP)
#define SHOW_MSGQ(msg) MessageBox(NULL, msg, _T("Информация"), MB_YESNO|MB_ICONWARNING|MB_DEFBUTTON1|icon)

		OSVERSIONINFO os;
		os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&os);

		//Win 3.11
		if(	(OS & OS_DENY_W311) && os.dwPlatformId == VER_PLATFORM_WIN32s)
		{
			SHOW_MSG(_T("Windows 3.х не поддерживается."));
			return FALSE;
		}
		if(	(OS & OS_UNSUPPORT_W311) && os.dwPlatformId == VER_PLATFORM_WIN32s)
		{
			if(SHOW_MSG(_T("Корректная работа программы на систамах Windows 3.x не гарантируется.\n\nПродолжить выполнение программы?")) == IDYES)
				return TRUE;
			else
				return FALSE;
		}

		//Win 9x
		if(	(OS & OS_DENY_W9X) && os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		{
			SHOW_MSG(_T("Системы Windows 95/98/Me не поддерживаеются."));
			return FALSE;
		}
		if(	(OS & OS_UNSUPPORT_W9X) && os.dwPlatformId == VER_PLATFORM_WIN32s)
		{
			if(SHOW_MSG(_T("Корректная работа программы на систамах Windows 95/98/Me не гарантируется.\n\nПродолжить выполнение программы?")) == IDYES)
				return TRUE;
			else
				return FALSE;
		}

		//Win NT4
		if(OS & OS_DENY_NT4)
		{
			if ((os.dwMajorVersion == 4 && os.dwMinorVersion == 0) ||
				(os.dwMajorVersion == 3 && os.dwMinorVersion == 51))
			{
				SHOW_MSG(_T("Системы Windows NT 3.51 и Windows NT 4.0 не поддерживаеются."));
				return FALSE;
			}
		}
		if(OS & OS_UNSUPPORT_NT4)
		{
			if ((os.dwMajorVersion == 4 && os.dwMinorVersion == 0) ||
				(os.dwMajorVersion == 3 && os.dwMinorVersion == 51))
			{
				if(SHOW_MSG(_T("Корректная работа программы на систамах Windows NT 3.51 и Windows NT 4.0 не гарантируется.\n\nПродолжить выполнение программы?")) == IDYES)
					return TRUE;
				else
					return FALSE;
			}
		}

		if(OS & OS_UNSUPPORT_W2K)
		{
			if ((os.dwMajorVersion == 5 && os.dwMinorVersion == 0))
			{
				if(SHOW_MSG(_T("Корректная работа программы на систамах Windows 2000 не гарантируется.\n\nПродолжить выполнение программы?")) == IDYES)
					return TRUE;
				else
					return FALSE;
			}
		}

		return TRUE;
	}
}
