#include "StdAfx.h"
#include ".\doc.h"
#include "Input.h"
#include "MainFrm.h"

CDoc::CDoc(void)
{
	//инициализация
	_OPCConnections = new CArrayOfOPCConnection();
	EmptyView();
}

CDoc::~CDoc(void)
{
	//Отключаемя от OPC
	_OPCConnections->DisconnectAll();
	delete _OPCConnections;
}

void CDoc::EmptyView()
{
	m_SchemaSettings.FixedSize		= false;
	m_SchemaSettings.HasBackgrd		= false;
	m_SchemaSettings.BackGrdColor	= GetSysColor(COLOR_APPWORKSPACE);

	CObjectMenagerPtr om;
	om->RemoveAll();

	m_Archive.SetArchiveFile(L"");
	UpdateMainCaption();
}

bool CDoc::LoadProject(wstring FileName)
{
	//Загрузить файл
	m_Archive.SetArchiveFile(FileName);
	if(!m_Archive.LoadVariables())
	{
		m_Archive.SetArchiveFile(L"");
		return false;
	}

	//Получить список мнемосхем
	TStrings List;
	if(!m_Archive.GetDirectoryFileList(List, CArchiveManager::SF_SCHEMAS))
	{
		m_Archive.SetArchiveFile(L"");
		return false;
	}

	//Загружаем схему
	if(List.size()>0)
	{
		//Не указана ли начальная схема?
		sProjectSettings ps;
		m_Archive.LoadProjectSettings(ps);
		if(!ps.InitialSchema.empty())
		{
			//Указана. Ищем ее в списке схем
			TStrings::iterator item = find(List.begin(),List.end(),ps.InitialSchema);
			if(item != List.end() && !SelectSchema(*item))
			{
				if(!SelectSchema(List[0])) //Схема не найдена/не загружена
				{
					EmptyView();
					return false;
				}
			}
		}
		else
			if(!SelectSchema(List[0]))
			{
				EmptyView();
				return false;
			}
	}
	UpdateAllViews();
	ConnectToOPC();
	UpdateMainCaption();
	return true;
}

void CDoc::ConnectToOPC()
{
	//получить список переменных
	CVariableMenagerPtr mngr;
	TStrings Variables = mngr->GetVariableList();
	_OPCConnections->DisconnectAll();
	_OPCHandles.clear();
	_OPCTagTimer.clear();

	//Подключаем к OPC каждую переменную
	for(TStrings::iterator i=Variables.begin();i!=Variables.end();i++)
	{
		if(mngr->GetVariableType(*i) != CVariableMenager::VT_OPC)
			continue; //Это не OPC переменная
		if(mngr->TestVariableInUse(*i) != true)
			continue; //Переменная не используется в текущей схеме

		TVariableSettings vs = mngr->GetVariableSetting(*i);
		const OPCHANDLE handle = _OPCConnections->GenerateUniqueHandle();

		_OPCHandles[handle] = *i;
		_OPCConnections->Connect(	vs.OPC.Computer, 
									vs.OPC.guid, 
									vs.OPC.TagName, 
									handle,
									this);
		_OPCTagTimer[handle] = GetTickCount();
	}
}

bool CDoc::SelectSchema(wstring Name)
{
	//Удалить все объекты
	CObjectMenagerPtr om;
	om->RemoveAll();

	//Загрузить параметры схемы
	if(!m_Archive.LoadSchemaSettings(Name, m_SchemaSettings))
		return false;

	//Загрузить объекты
	if(!m_Archive.LoadSchemaObj<CObjRectangle>(Name, _RECTANGLE))
		return false;
	if(!m_Archive.LoadSchemaObj<CObjPolyline>(Name, _POLYLINE))
		return false;
	if(!m_Archive.LoadSchemaObj<CObjEllipse>(Name, _ELLIPSE))
		return false;
	if(!m_Archive.LoadSchemaObj<CObjImage>(Name, _IMAGE))
		return false;
	if(!m_Archive.LoadSchemaObj<CObjText>(Name, _TEXT))
		return false;
	if(!m_Archive.LoadSchemaObj<CObjButton>(Name, _BUTTON))
		return false;
	if(!m_Archive.LoadSchemaObj<Primitives::CInput>(Name, _INPUT))
		return false;
	if(!m_Archive.LoadSchemaObj<CObjSound>(Name, _SOUND))
		return false;
	if(!m_Archive.LoadSchemaObj<CObjGraph>(Name, _GRAPH))
		return false;

	//Загрузить действия
	if(!m_Archive.LoadActions(Name))
		return false;

	//Отсортировать объекты в соответствии с их порядковым номером
	om->SortObjects();

	if(m_SchemaSettings.FixedSize && g_MainFrame)
	{
		int w = m_SchemaSettings.cx,
			h = m_SchemaSettings.cy;

		h += GetSystemMetrics(SM_CYMENU);		//Menu
		h += GetSystemMetrics(SM_CYCAPTION);	//Caption
		h += GetSystemMetrics(SM_CYFRAME);		//Рамка
		w += GetSystemMetrics(SM_CXFRAME)*2;	//Рамка

		if(CWindow(g_MainFrame->m_hWndStatusBar).IsWindowVisible())
		{
			CRect rc;
			CWindow(g_MainFrame->m_hWndStatusBar).GetWindowRect(&rc);
			h += rc.Height();
		}		
		g_MainFrame->SetWindowPos(NULL,0,0,w,h,SWP_NOZORDER|SWP_NOMOVE);
	}
	UpdateAllViews();
	//ConnectToOPC();
	UpdateMainCaption();

	CVariableMenagerPtr vm;
	TStrings Variables = vm->GetVariableList();
	for(TStrings::iterator i=Variables.begin();i!=Variables.end();i++)
	{
		CComVariant v;
		v.ChangeType(vm->GetVariable(*i).vt);
		vm->SetVariable(*i, v,false);
	}
	return true;
}

void CDoc::UpdateMainCaption()
{
	if(g_MainFrame && g_MainFrame->IsWindow())
	{
		CString DefaultCaption, Caption;
		DefaultCaption.LoadString(IDR_MAINFRAME);

		if(m_Archive.GetArchiveFile().empty())
			Caption = DefaultCaption;
		else
			Caption.Format(L"[%s] — %s",m_Archive.GetArchiveFile().c_str(),DefaultCaption);

		g_MainFrame->SetWindowText(Caption);
	}
}
/*
BOOL CDoc::TestTagTimer()
{
	_OPCTimerCS.Lock();
	if(_OPCTagTimer.empty())
	{
		_OPCTimerCS.Unlock();
		return TRUE;
	}

	typedef map<OPCHANDLE,ULONG>::iterator it;
	sProjectSettings ps;
	if(!m_Archive.LoadProjectSettings(ps))
	{
		_OPCTimerCS.Unlock();
		return FALSE;
	}

	const DWORD tick = GetTickCount();
	for(it i=_OPCTagTimer.begin();i!=_OPCTagTimer.end();i++)
	{
		if((tick - i->second) > ps.IntervalOPC*TIMEOUT_MUL)
		{
			_OPCTimerCS.Unlock();
			return FALSE;
		}
	}

	_OPCTimerCS.Unlock();
	return TRUE;
}

void CDoc::UpdateTagTimer(OPCHANDLE handle)
{
	_OPCTimerCS.Lock();
	map<OPCHANDLE,ULONG>::iterator item = _OPCTagTimer.find(handle);
	if(item != _OPCTagTimer.end())
	{
		item->second = GetTickCount();
	}
	else
	{
		//ATLASSERT(_OPCTagTimer.find(handle) != _OPCTagTimer.end());
		ATLTRACE("** Error (%s): Tag(0x%x) handle haven't been found\n",__FUNCTION__, handle);
	}
	_OPCTimerCS.Unlock();
};
*/
void CDoc::RefreshOPC()
{
	if(_OPCConnections)
		_OPCConnections->RefreshChannels();
}
