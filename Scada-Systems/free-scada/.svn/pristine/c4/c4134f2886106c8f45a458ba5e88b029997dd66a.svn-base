#include "StdAfx.h"
#include ".\objectsdialog.h"
#include "ObjectMenager.h"

LRESULT CObjectsDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DlgResize_Init(true, true, NULL);
	DoDataExchange(DDX_LOAD);
	CenterWindow(GetParent());

	m_List = (CListBox)GetDlgItem(IDC_LIST_OBJECTS);
	m_List.ResetContent();

	CObjectMenagerPtr mng;

	//Получение списка имен примитивов и имени выбранного примитива
	TStrings Names =  mng->GetObjectNames();
	CObject* SelObj = mng->GetSelected();
	wstring SelName = L"";
	if(SelObj != NULL)SelName = SelObj->GetName();
	int i = 0;
	
	//Заполняем ListBox именами объектов
	if(!Names.empty())
	{
		for(TStrings::iterator it = Names.begin(); it != Names.end(); it++, i++)
		{
			int item = m_List.AddString(it->c_str());
			//Делаем выделенным в ListBox-е выбраный примитив
			if(wcscmp(SelName.c_str(), it->c_str()) == 0)
				m_List.SetSel(item);
		}
	}

	//Получаем список имен объектов из Listbox-а и делаем их выделенными
	TStrings SelectedNames = mng->GetSelection()->GetObjects();
	if(!SelectedNames.empty())
	{
		for(TStrings::iterator it = SelectedNames.begin(); it != SelectedNames.end(); it++, i++)
		{
			int item = m_List.AddString(it->c_str());
			m_List.SetSel(item);
		}
	}

	return TRUE;
}


LRESULT CObjectsDialog::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);
	return TRUE;
}

LRESULT CObjectsDialog::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_ObjectNames.clear();
	if(wID != IDOK)
	{
		EndDialog(wID);
		return 0;
	}


	TStrings Selected; //Список имен выбранных примитивов

	int count = m_List.GetCount();


	//Заполняем список выбранных примитивов
	for(int i =0; i < count; i++)
	{
		if(m_List.GetSel(i))
		{
			wchar_t buffer[0xff];
			m_List.GetText(i, buffer); 
			wstring str(buffer);
			Selected.push_back(str);
		}
	}



	CObjectMenagerPtr mng;
	mng->SelectObject(NULL);

//Если только один примитив выделен, делаем его выбранным
	if(Selected.size() == 1)
	{
		CObject* obj = mng->FindObject(*Selected.begin());
		if(obj != NULL)
			mng->SelectObject(obj);
	}

//Если более одного выделено - помещаем в Selection
	if(Selected.size() > 1)
	{
		for(TStrings::iterator it = Selected.begin(); it != Selected.end(); it++)
		{
			CObject* obj = mng->FindObject(*it);
			if(obj != NULL)
				mng->AddToSelected(obj);
		}
	}

	EndDialog(wID);
	return 0;
}


