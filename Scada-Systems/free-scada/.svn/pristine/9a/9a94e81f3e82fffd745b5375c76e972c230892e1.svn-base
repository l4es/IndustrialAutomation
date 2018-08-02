#include "StdAfx.h"
#include ".\modulemanager.h"
#include "ModuleArchive.h"
#include "ModuleServices.h"

#include "MainFrm.h"

CModuleManager::CModuleManager(void)
{
	//Создание модуля архиватора
	ModuleArchive::CModuleArchive* Archive =  new ModuleArchive::CModuleArchive();
	Archive->m_ModuleManager = this;
	//Если модуль определен помещаем его в вектор модулей
	if(Archive->Detect())
		m_Modules.push_back(Archive);
	else
		//если не определен - удаляем
		delete Archive;

	//Создание модуля запуска-остановки служб
	ModuleServices::CModuleServices* Services = new ModuleServices::CModuleServices();
	Services->m_ModuleManager = this;
	//Если модуль определен помещаем его в вектор модулей
	if(Services->Detect())
		m_Modules.push_back(Services);
	else
		//если не определен - удаляем
		delete Services;


	_CurrentNode = NULL;
}

CModuleManager::~CModuleManager(void)
{
	for(TVModule::iterator it = m_Modules.begin(); it != m_Modules.end(); it++)
	{
		delete *it;
	}
}


//Загрузка модулей
bool CModuleManager::LoadModules(HWND hParent, HWND hMainWnd, CRect wndRect)
{
	TVModule::iterator it = m_Modules.begin(); 
	while( it != m_Modules.end() )
	{
		CBaseModule* Module = *it;
		Module->Load(hParent, hMainWnd, wndRect);
		it++;
	}

	return true;
}

//Получить HWND вида текущего узла
HWND CModuleManager::GetCurrentHWND()
{
	CBaseModule* Module = _CurrentNode->Module;
	
	if(Module == NULL)
		return NULL;
	
	return Module->GetWndHandle();
}

//Установка текущего узла
void CModuleManager::OnSetCurrent(sNode* Node)
{
	if(Node == NULL)
		return;
	//Сохранение данных текущего узла
	if(_CurrentNode != NULL)
	{
		CBaseModule* OldModule = _CurrentNode->Module;
		if(OldModule != NULL)
			OldModule->SaveNode(_CurrentNode);
	}

	//Определяем к какому модулю относится узел Node
	CBaseModule* Module = Node->Module;
	if(Module == NULL)
		return;
	//Установка текущим узла Node внутри модуля
	Module->SetCurrent(Node);
	//Установка текущим узла Node
	_CurrentNode = Node;
}

//Раскрытие узла
bool CModuleManager::OnExpandNode(sNode* Node)
{
	if(Node == NULL)
		return true;
//Если тип узла - item (не папка) - работает как обычно
	if(Node->Type == T_ITEM)
		return true;

	CBaseModule* Module = Node->Module;
	if(Module == NULL)
		return true;

	//Если модуль не допускает раскрытия  - функция возвращает false
	if(!Module->ExpandNode(Node))
		return false;

	return true;
}

//Получить текущий узел
sNode* CModuleManager::GetCurrentNode()
{
	return _CurrentNode;
}
