#include "StdAfx.h"
#include ".\projectsoundsdlg.h"
#include "InputBox.h"

LRESULT CProjectSoundsDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DlgResize_Init(true, true, NULL);
	CenterWindow(GetParent());

	UIAddChildWindowContainer(m_hWnd);

	UIEnable(IDC_STOP, FALSE);
	FillList();
	return TRUE;
}

LRESULT CProjectSoundsDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

//Заполнение списка именами звуков которые уже есть в проекте
void CProjectSoundsDlg::FillList()
{
	CWaitCursor wait;
	TStrings list;
	//Получаем из ArchiveMenager имена изображений
	_Menager->GetDirectoryFileList(list, CArchiveManager::SF_SOUNDS);

	CListBox lb = GetDlgItem(IDC_IMAGE_LIST);
	CString LastItem;
	if(lb.GetCurSel()>=0)
		lb.GetText(lb.GetCurSel(), LastItem);
	lb.ResetContent();
	for(TStrings::iterator i=list.begin();i!=list.end();i++)
		lb.AddString(i->c_str());

	if(!LastItem.IsEmpty())
		lb.SetCurSel(lb.FindString(-1, LastItem));
	
	BOOL tmp=FALSE;
	OnLbnSelchangeImageList(NULL,NULL,NULL,tmp);
}

//Загрузка изображения из файла
LRESULT CProjectSoundsDlg::OnBnClickedLoadFromFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY, cFilterSounds);

	if(dlg.DoModal() != IDOK)
		return 0;

	CWaitCursor wait;
	CSound snd;
	if(snd.LoadFromStdFile(dlg.m_szFileName) != true)
	{
		MessageBox(L"Cannot load the file.", L"Error", MB_OK|MB_ICONINFORMATION);
		return 0;
	}

	wstring SoundName = dlg.m_szFileName;
	SoundName.erase(0, HelpersLib::ExtractFilePath(SoundName.c_str()).size());
	SoundName.erase(SoundName.begin()+SoundName.find_last_of('.'),SoundName.end());
	SoundName = CString(SoundName.c_str()).Trim(L"\\");

	SoundName = _Menager->GetSpecialFolder(CArchiveManager::SF_SOUNDS) + L"\\" + SoundName;
	if(_Menager->IsFileExists(SoundName))
	{
		MessageBox(L"A file with the same name already present in the project.", L"Error", MB_OK|MB_ICONINFORMATION);
		return 0;
	}
	
	CMemFile* file;
	if(_Menager->CreateMemFile(SoundName, &file) != true)
	{
		MessageBox(L"Cannot create file.", L"Error", MB_OK|MB_ICONINFORMATION);
		return 0;
	}
	snd.Save(*file);
	_Menager->WriteAndFreeMemFile(file);

	FillList();
	return 0;
}

//Переименование изображения загруженного в проект
LRESULT CProjectSoundsDlg::OnBnClickedRename(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CListBox lb = GetDlgItem(IDC_IMAGE_LIST);
	if(lb.GetCurSel()<0)	
		return 0;

	CWaitCursor wait;
	CString ImageName,origImageName;
	lb.GetText(lb.GetCurSel(), ImageName);
	origImageName = ImageName;
	ImageName = wstring(_Menager->GetSpecialFolder(CArchiveManager::SF_IMAGES) + L"\\" + (LPCTSTR)ImageName).c_str();

	CMemFile* file;
	if(_Menager->OpenMemFile((LPCTSTR)ImageName, &file) != true)
	{
		MessageBox(L"Cannot open the file.", L"Error", MB_OK|MB_ICONERROR);
		return 0;
	}
	CInputBox InputBox(L"Input new name:", L"Rename sound", (LPCTSTR)origImageName);
	if(InputBox.DoModal() != IDOK)
	{
		_Menager->FreeMemFile(file);
		return 0;
	}
	wait.Restore();
	wstring NewName = InputBox.GetValAsText();
	NewName = _Menager->GetSpecialFolder(CArchiveManager::SF_IMAGES) + L"\\" + NewName;

	if(_Menager->DeleteFile((LPCTSTR)ImageName) != true)
	{
		_Menager->FreeMemFile(file);
		MessageBox(L"Cannot rename the file.", L"Error", MB_OK|MB_ICONERROR);
		return 0;
	}
	
	if(_Menager->WriteAsAndFreeMemFile(NewName, file) != true)
	{
		MessageBox(L"Cannot rename the file.", L"Error", MB_OK|MB_ICONERROR);
		return 0;
	}
	FillList();
	return 0;
}

//Выделение изображения в ListBox-е, заполнение просмотра изображения
LRESULT CProjectSoundsDlg::OnLbnSelchangeImageList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CWaitCursor wait;
	const BOOL ItemSelected = CListBox(GetDlgItem(IDC_IMAGE_LIST)).GetCurSel()>=0;
	
	UIEnable(IDC_PLAY, ItemSelected);
	UIEnable(IDC_RENAME, ItemSelected);
	UIEnable(IDC_DELETE, ItemSelected);
	UIUpdateChildWindows();
	if(!ItemSelected)
		return 0;

	SetDlgItemText(IDC_WIDTH, L"0");
	SetDlgItemText(IDC_HEIGHT, L"0");

	CListBox lb = GetDlgItem(IDC_IMAGE_LIST);
	//Получам полное имя
	CString SoundName;
	lb.GetText(lb.GetCurSel(), SoundName);
	SoundName = wstring(_Menager->GetSpecialFolder(CArchiveManager::SF_SOUNDS) + L"\\" + (LPCTSTR)SoundName).c_str();

	//Загрузка изображения из архива
	CMemFile* file;
	if(_Menager->OpenMemFile((LPCTSTR)SoundName, &file))
	{
		_Sound.Load(*file);
		_Menager->FreeMemFile(file);
	}
	else
	{
		_Sound.Clear();
	}
	UIUpdateChildWindows();
	return 0;
}

//Удаление изображения
LRESULT CProjectSoundsDlg::OnBnClickedDelete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CListBox lb = GetDlgItem(IDC_IMAGE_LIST);
	if(lb.GetCurSel()<0)	
		return 0;

	CWaitCursor wait;

	//Получаем полное имя в архиве для выбранного изображения 
	CString FileName;
	lb.GetText(lb.GetCurSel(), FileName);
	FileName = wstring(_Menager->GetSpecialFolder(CArchiveManager::SF_SOUNDS) + L"\\" + (LPCTSTR)FileName).c_str();

	//Удаляем файл
	if(_Menager->DeleteFile((LPCTSTR)FileName) != true)
	{
		MessageBox(L"Cannot delete the file.", L"Error", MB_OK|MB_ICONERROR);
		return 0;
	}
	//Обновляем список
	FillList();
	return 0;
}
LRESULT CProjectSoundsDlg::OnPlayBnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	_Sound.PlayAsync(false);
	UIEnable(IDC_STOP, TRUE);
	UIUpdateChildWindows();
	return 0;
}

LRESULT CProjectSoundsDlg::OnStopBnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	_Sound.StopAsync();
	UIEnable(IDC_STOP, FALSE);
	UIUpdateChildWindows();
	return 0;
}
