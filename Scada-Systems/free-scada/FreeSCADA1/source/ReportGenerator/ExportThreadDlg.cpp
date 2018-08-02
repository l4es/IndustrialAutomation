#include "StdAfx.h"
#include ".\exportthreaddlg.h"

CExportTaskMenager* CExportTaskMenager::_self = NULL;
ULONG CExportTaskMenager::_refcount = NULL;

CExportTaskMenager* CExportTaskMenager::Instance()
{
	if(!_self) 
		_self = new CExportTaskMenager();

	_refcount++;
	return _self;
};

void CExportTaskMenager::Release()
{
	if(!_refcount) 
		return;

	_refcount--;

	if(!_refcount) 
	{
		delete this;
		_self = NULL;
	}
}

CExportTaskMenager::~CExportTaskMenager()
{
	for(list<CExportThreadDlg*>::iterator i=_Tasks.begin();i!=_Tasks.end();i++)
	{
		if((*i)->IsWindow())
			(*i)->DestroyWindow();
		delete *i;
	}
}

void CExportTaskMenager::CreateNewTask(CTagDB::TVChannelData& Data, wstring FileName, HWND ParentWnd, wstring Title)
{
	CExportThreadDlg* dlg = new CExportThreadDlg(Data, FileName, Title);
	dlg->Create(ParentWnd);
	dlg->ShowWindow(SW_NORMAL);
	dlg->UpdateWindow();
	_Tasks.push_back(dlg);
}

void CExportTaskMenager::DeleteTask(CExportThreadDlg* task)
{
	typedef list<CExportThreadDlg*>::iterator it;
	it item = find(_Tasks.begin(), _Tasks.end(), task);
	if(item != _Tasks.end())
	{
		delete *item;
		_Tasks.erase(item);
	}
	else
		ATLASSERT(item != _Tasks.end());
}

CExportThreadDlg::CExportThreadDlg(CTagDB::TVChannelData& Data, wstring FileName, wstring Title)
	: _Data(Data)
	, _FileName(FileName)
	, m_Left(NULL)
	, _Total(NULL)
	, _TerminateThread(FALSE)
	, _Title(Title)
{
	_TaskMenager = CExportTaskMenager::Instance();
}

CExportThreadDlg::~CExportThreadDlg()
{
	_TaskMenager->Release();
}

LRESULT CExportThreadDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DoDataExchange(DDX_LOAD);

	_hThread = CreateThread(	NULL,
								NULL,
								ProcessThread,
								this,
								NULL,
								&_ThreadID);
	ATLASSERT(_hThread != NULL);

	SetWindowText(_Title.c_str());
	SetTimer(1,300);

	return TRUE;
}

LRESULT CExportThreadDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(DDX_SAVE);
	DestroyWindow();
	return 0;
}
LRESULT CExportThreadDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	::PostMessage(GetParent(), WM_DELETE_TASK, (WPARAM)this, NULL);
	_cs.Lock();
	_TerminateThread = TRUE;
	_cs.Unlock();
	if(WaitForSingleObject(_hThread, 30000) == WAIT_TIMEOUT)
		TerminateThread(_hThread, 1);
	return 0;
}

DWORD WINAPI CExportThreadDlg::ProcessThread(LPVOID _param)
{
	CoInitialize(NULL);
	CExportThreadDlg* param = (CExportThreadDlg*)_param;

	typedef CTagDB::TVChannelData::iterator it;
	it begin = param->_Data.begin();
	it end = param->_Data.end();
	it i=begin;

	if(PathFileExists(param->_FileName.c_str()))
	{
		if(!DeleteFile(param->_FileName.c_str()))
		{
			param->PostMessage(WM_CLOSE);
			return 1;
		}
	}

	param->_cs.Lock();
	param->_Total = param->_Data.size();
	param->_Current = 0;
	param->_cs.Unlock();

	CString sSql;
	sSql.Format(L"DRIVER={%s};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s",
				L"MICROSOFT EXCEL DRIVER (*.XLS)", param->_FileName.c_str(), param->_FileName.c_str());
	CString strConn = L"Provider=MSDASQL;";
	strConn += sSql;

	UINT TableNo=1;
	CString TableName;

	DB::CADODatabase db;
	if(db.Open(strConn))
	{
		while(i != end)
		{
			TableName.Format(L"[Данные каналов %u]", TableNo);
			sSql = L"CREATE TABLE ";
			sSql += TableName;
			sSql += L" ([Дата/Время] TEXT,[Значение] TEXT)";
			if(db.Execute(sSql))
			{
				while(((i - begin) < 0xfffe) && i!=end)
				{
					param->_cs.Lock();
					param->_Current = i - param->_Data.begin();
					param->_cs.Unlock();

					CString tmp;
					tmp = i->DateTime.Format(L"'%Y.%m.%d %H:%M:%S'");
					sSql = L"INSERT INTO ";
					sSql += TableName;
					sSql += L" ([Дата/Время], [Значение]) VALUES (";
					sSql += tmp;
					sSql += L", ";
					tmp.Format(L"%.5f",i->val);
					sSql += tmp;
					sSql += L");";

					db.Execute(sSql);
					i++;

					param->_cs.Lock();
					if(param->_TerminateThread == TRUE)
					{
						db.Close();
						CoUninitialize();
						param->_cs.Unlock();
						return 0;
					}
					param->_cs.Unlock();
				}
				if((i - begin) >= 0xfffe)
				{
					begin = i;
					TableNo++;
				}
			}
		}
		db.Close();
	}
	CoUninitialize();

	param->PostMessage(WM_CLOSE);
	return 0;
}
LRESULT CExportThreadDlg::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(wParam == 1)
	{
		_cs.Lock();
		m_Left = _Total - _Current;
		DoDataExchange(DDX_LOAD);

		CProgressBarCtrl bar = GetDlgItem(IDC_PROGRESS1);
		bar.SetRange(0, 100);
		double pos;
		pos = ((double)_Current/(double)_Total)*100.0f;
		bar.SetPos((int)pos);
		_cs.Unlock();
	}
	return 0;
}
