#pragma once

#include "resource.h"
#include "TagDB.h"

class CExportTaskMenager;
class CExportThreadDlg
	: public CDialogImpl<CExportThreadDlg>
	, public CWinDataExchange<CExportThreadDlg>
{
public:
	enum { IDD = IDD_EXPORT_THREAD };

	BEGIN_DDX_MAP(CExportThreadDlg)
		DDX_UINT(IDC_TOTAL, _Total)
		DDX_UINT(IDC_LEFT, m_Left)
	END_DDX_MAP()

	BEGIN_MSG_MAP(CExportThreadDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
	END_MSG_MAP()

	CExportThreadDlg(CTagDB::TVChannelData& Data, wstring FileName, wstring Title = _T("Экспорт в Excel"));
	~CExportThreadDlg();

	DWORD m_Left;

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

protected:
	CTagDB::TVChannelData _Data;
	wstring _FileName;
	wstring _Title;
	CExportTaskMenager* _TaskMenager;
	HANDLE _hThread;
	DWORD _ThreadID;
	CComAutoCriticalSection _cs;
	DWORD _Total;
	DWORD _Current;
	BOOL _TerminateThread;

	static DWORD WINAPI ProcessThread(LPVOID);
public:
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

//Менеджер задач
class CExportTaskMenager
{
public:
	static CExportTaskMenager* Instance();
	void Release();

	void CreateNewTask(CTagDB::TVChannelData& Data, wstring FileName, HWND ParentWnd, wstring Title);
	void DeleteTask(CExportThreadDlg* task);

protected:
	static CExportTaskMenager* _self;
	static ULONG _refcount;

	CExportTaskMenager(){};
	~CExportTaskMenager();
	list<CExportThreadDlg*>	_Tasks;
};