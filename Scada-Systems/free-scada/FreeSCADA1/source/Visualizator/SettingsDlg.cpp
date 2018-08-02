#include "StdAfx.h"
#include ".\settingsdlg.h"

namespace ConfigFunc
{
	//Функция возвращает true, если программа находится в автозагрузке
	inline bool AutoRunIsOn()
	{
		//Получить полное имя файла программы
		wstring str(MAX_PATH+1,'\0');
		wstring FileName;
		GetModuleFileName(NULL, &str[0], MAX_PATH);

		//Выделить краткое имя файла ("c:\prog.exe" => "prog.exe")
		FileName = str;
		wstring::size_type pos = FileName.find_last_of('\\');
		FileName.erase(FileName.begin(), FileName.begin()+pos+1);

		//Прочитать значение автозагрузки из реестра
		CRegKey reg;
		reg.Open(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run");
		TCHAR val[MAX_PATH+1];
		ULONG nChars = MAX_PATH;
		reg.QueryStringValue(FileName.c_str(), val, &nChars);
		reg.Close();

		if(nChars == 0)
			return false; //Значение пусто

		//Совпадает ли имя файла в реестре с именем модуля?
		return _tcsicmp( str.c_str(), val) == NULL;
	}

	//Включить автозагрузку для текущего модуля
	inline void SetAutoRun(bool state)
	{
		//Получить полное имя файла программы
		wstring str(MAX_PATH+1,'\0');
		wstring FileName;
		GetModuleFileName(NULL, &str[0], MAX_PATH);

		//Выделить краткое имя файла ("c:\prog.exe" => "prog.exe")
		FileName = str;
		wstring::size_type pos = FileName.find_last_of('\\');
		FileName.erase(FileName.begin(), FileName.begin()+pos+1);

		CRegKey reg;
		reg.Open(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run");

		if(state)
			reg.SetStringValue(FileName.c_str(), str.c_str()); //Записать значение
		else
			reg.DeleteValue(FileName.c_str()); //Удалить значение
		reg.Close();
	}

	//Функция возвращает true, если включена опция автозагрузки файла
	inline bool FileAutoRunIsOn()
	{
		hl::CCfgReg reg;
		reg.Open(cProduct, true);
		wstring FileName = reg.ReadString(L"AutoRunFile");
		if(FileName.empty())
			return false;

		return PathFileExists(FileName.c_str())?true:false; //Существует ли заданный файл?
	}

	//Получить полное имя автозагружаемого файла
	inline wstring FileAutoRun()
	{
		HelpersLib::CCfgReg reg;
		reg.Open(cProduct, true);
		return reg.ReadString(L"AutoRunFile").c_str();
	}

	//Включить опцию автозагрузки для указанного файла
	inline void SetFileAutoRun(wstring FileName)
	{
		HelpersLib::CCfgReg reg;
		reg.Open(cProduct);
		reg.WriteString(L"AutoRunFile", FileName.c_str());
	}
}

LRESULT CSettingsDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//Инициализация переменных
	m_AutoRun		= ConfigFunc::AutoRunIsOn()?1:0;
	m_FileAutoRun	= ConfigFunc::FileAutoRunIsOn()?1:0;
	m_FileName		= ConfigFunc::FileAutoRun().c_str();
	
	DoDataExchange(DDX_LOAD);
	CenterWindow(GetParent());

	//Обновить поля "автозагрузка файла"
	BOOL tmp=FALSE;
	OnBnClickedFileAutorun(NULL,NULL,NULL, tmp);

	return TRUE;
}

LRESULT CSettingsDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(wID == IDOK)
	{
		DoDataExchange(DDX_SAVE);

		ConfigFunc::SetAutoRun(m_AutoRun?true:false);			//Автозагрузка программы
		if(m_FileAutoRun)
			ConfigFunc::SetFileAutoRun((LPCTSTR)m_FileName);	//Автозагрузка файла вкл
		else
			ConfigFunc::SetFileAutoRun(L"");						//Автозагрузка файла выкл
	}
	EndDialog(wID);
	return 0;
}
LRESULT CSettingsDlg::OnBnClickedFileAutorun(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(DDX_SAVE, IDC_FILE_AUTORUN);

	//Вкл/Выкл окна в зависимости от флага "Автозагрузка файла"
	GetDlgItem(IDC_FILENAME).EnableWindow(m_FileAutoRun==1?TRUE:FALSE);
	GetDlgItem(IDC_SELECT_FILE).EnableWindow(m_FileAutoRun==1?TRUE:FALSE);
	return 0;
}

LRESULT CSettingsDlg::OnBnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(DDX_SAVE, IDC_FILENAME);

	//Выбор файла для автозагрузки
	CFileDialog dlg(TRUE, L".mp", m_FileName, OFN_HIDEREADONLY, cFilterProject);
	if(dlg.DoModal() == IDOK)
	{
		m_FileName = dlg.m_szFileName;
		DoDataExchange(DDX_LOAD, IDC_FILENAME);
	}
	return 0;
}
