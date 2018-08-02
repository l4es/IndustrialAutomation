#include "StdAfx.h"
#include ".\projectimagedlg.h"
#include "InputBox.h"

LRESULT CProjectImageDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DlgResize_Init(true, true, NULL);
	DoDataExchange(DDX_LOAD);
	CenterWindow(GetParent());

	FillList();
	
	return TRUE;
}

LRESULT CProjectImageDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

//Заполнение списка именами изображений которые уже есть в проекте
void CProjectImageDlg::FillList()
{
	CWaitCursor wait;
	TStrings list;
	//Получаем из ArchiveMenager имена изображений
	_Menager->GetDirectoryFileList(list, CArchiveManager::SF_IMAGES);

	CListBox lb = GetDlgItem(IDC_IMAGE_LIST);
	CString LastItem;
	if(lb.GetCurSel()>=0)
	{
		lb.GetText(lb.GetCurSel(), LastItem);
	}
	lb.ResetContent();
	for(TStrings::iterator i=list.begin();i!=list.end();i++)
	{
		lb.AddString(i->c_str());
	}
	if(!LastItem.IsEmpty())
	{
		lb.SetCurSel(lb.FindString(-1, LastItem));
	}

	BOOL tmp=FALSE;
	OnLbnSelchangeImageList(NULL,NULL,NULL,tmp);
}

//Загрузка изображения из файла
LRESULT CProjectImageDlg::OnBnClickedLoadFromFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY, cFilterAllGraphics);

	if(dlg.DoModal() != IDOK)
		return 0;

	CWaitCursor wait;
	CImage img;
	if(img.LoadFromStdFile(dlg.m_szFileName) != true)
	{
		MessageBox(L"Cannot load the file.", L"Error", MB_OK|MB_ICONINFORMATION);
		return 0;
	}

	wstring ImageName = dlg.m_szFileName;
	ImageName.erase(0, HelpersLib::ExtractFilePath(ImageName.c_str()).size());
	ImageName.erase(ImageName.begin()+ImageName.find_last_of('.'),ImageName.end());
	ImageName = CString(ImageName.c_str()).Trim(L"\\");

	ImageName = _Menager->GetSpecialFolder(CArchiveManager::SF_IMAGES) + L"\\" + ImageName;
	if(_Menager->IsFileExists(ImageName))
	{
		MessageBox(L"A file with the same name already present in the project.", L"Error", MB_OK|MB_ICONINFORMATION);
		return 0;
	}
	
	CMemFile* file;
	if(_Menager->CreateMemFile(ImageName, &file) != true)
	{
		MessageBox(L"Cannot create file.", L"Error", MB_OK|MB_ICONINFORMATION);
		return 0;
	}
	img.Save(*file);
	_Menager->WriteAndFreeMemFile(file);

	//Обновление ListBox-а с именами изображения
	FillList();
	return 0;
}

//Переименование изображения загруженного в проект
LRESULT CProjectImageDlg::OnBnClickedRename(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
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
	CInputBox InputBox(L"Input new name:", L"Rename image", (LPCTSTR)origImageName);
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
LRESULT CProjectImageDlg::OnLbnSelchangeImageList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CWaitCursor wait;
	const BOOL ItemSelected = CListBox(GetDlgItem(IDC_IMAGE_LIST)).GetCurSel()>=0;
	
	CWindow(GetDlgItem(IDC_RENAME)).EnableWindow(ItemSelected);
	CWindow(GetDlgItem(IDC_DELETE)).EnableWindow(ItemSelected);

	SetDlgItemText(IDC_WIDTH, L"0");
	SetDlgItemText(IDC_HEIGHT, L"0");

	if(!m_Preview.IsNull())
		m_Preview.DeleteObject();

	CListBox lb = GetDlgItem(IDC_IMAGE_LIST);
	if(lb.GetCurSel()<0)	
	{
		//Рисуем черный фон, если не выбрано изображение
		CDCHandle dc = GetDC();
		CDC tmpDC;
		tmpDC.CreateCompatibleDC(dc);
		m_Preview.CreateCompatibleBitmap(tmpDC, 100,100);
		m_Preview.SetBitmapDimension(100,100);
		tmpDC.SelectBitmap(m_Preview);
		tmpDC.FillSolidRect(0,0,100,100,RGB(0,0,0));
		ReleaseDC(dc);
		return 0;
	}

	CString Caption; 
	GetWindowText(Caption);
	SetWindowText(CString(Caption + " [Loading...]"));

	CStatic frame = GetDlgItem(IDC_PREVIEW);

	//Получам полное имя изображения (с путем в архивном файле)
	CString ImageName;
	lb.GetText(lb.GetCurSel(), ImageName);
	ImageName = wstring(_Menager->GetSpecialFolder(CArchiveManager::SF_IMAGES) + L"\\" + (LPCTSTR)ImageName).c_str();

	//Загрузка изображения из архива
	CMemFile* file;
	if(_Menager->OpenMemFile((LPCTSTR)ImageName, &file))
	{
		CImage img;
		img.Load(*file);
		_Menager->FreeMemFile(file);

		CDCHandle dc = frame.GetDC();

		//Создаем Bitmap
		CBitmapHandle oldbmp;
		m_Preview.CreateCompatibleBitmap(dc, img.GetWidth(), img.GetHeight());
		m_Preview.SetBitmapDimension(img.GetWidth(), img.GetHeight());
		CDC src;
		src.CreateCompatibleDC(dc);
		oldbmp = src.SelectBitmap(m_Preview);

		//Рисование изображения(заполнение Bitmap-а)
		img.DrawOnDC(src);

		src.SelectBitmap(oldbmp);
		frame.ReleaseDC(dc);

		CString tmp;
		tmp.Format(L"%u",img.GetWidth());
		SetDlgItemText(IDC_WIDTH, tmp);
		tmp.Format(L"%u",img.GetHeight());
		SetDlgItemText(IDC_HEIGHT, tmp);

		RedrawWindow();
	}
	else
	{
		//Рисуем черный фон
		CDCHandle dc = GetDC();
		CDC tmpDC;
		tmpDC.CreateCompatibleDC(dc);
		m_Preview.CreateCompatibleBitmap(tmpDC, 100,100);
		m_Preview.SetBitmapDimension(100,100);
		tmpDC.SelectBitmap(m_Preview);
		tmpDC.FillSolidRect(0,0,100,100,RGB(0,0,0));
		ReleaseDC(dc);
	}

	SetWindowText(Caption);
	return 0;
}

//Удаление изображения
LRESULT CProjectImageDlg::OnBnClickedDelete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CListBox lb = GetDlgItem(IDC_IMAGE_LIST);
	if(lb.GetCurSel()<0)	
		return 0;

	CWaitCursor wait;

	//Получаем полное имя в архиве для выбранного изображения 
	CString ImageName;
	lb.GetText(lb.GetCurSel(), ImageName);
	ImageName = wstring(_Menager->GetSpecialFolder(CArchiveManager::SF_IMAGES) + L"\\" + (LPCTSTR)ImageName).c_str();

	//Удаляем файл
	if(_Menager->DeleteFile((LPCTSTR)ImageName) != true)
	{
		MessageBox(L"Cannot delete the file.", L"Error", MB_OK|MB_ICONERROR);
		return 0;
	}
	//Обновляем список изображений
	FillList();
	return 0;
}

//Рисование Bitmap-а
LRESULT CProjectImageDlg::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(*this);
	if(m_Preview.IsNull())
		return 0;

	CStatic frame = GetDlgItem(IDC_PREVIEW);

	CBitmapHandle oldbmp;
	CDC src;
	src.CreateCompatibleDC(dc);
	oldbmp = src.SelectBitmap(m_Preview);

	dc.SetStretchBltMode(HALFTONE);
	CRect rc;
	frame.GetWindowRect(rc);
	ScreenToClient(rc);
	CSize bmpSize;
	m_Preview.GetBitmapDimension(&bmpSize);
	dc.StretchBlt(rc.left, rc.top, rc.Width(), rc.Height(), src, 0, 0, bmpSize.cx, bmpSize.cy, SRCCOPY );

	src.SelectBitmap(oldbmp);

	return 0;
}
