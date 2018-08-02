#include "StdAfx.h"
#include ".\moduleservices.h"

namespace ModuleServices
{
    //Определение, можно ли модуль загрузить
	bool CModuleServices::Detect()
	{
		return true;
	}

	//Загрузка модуля
	bool CModuleServices::Load(HWND hParent, HWND /*hMainWnd*/, CRect /*wndRect*/)
	{
		//Создание вида модуля
		m_hWindow = m_View.Create(hParent);
		m_View.m_Module = this;

	//	m_hToolBar = CFrameWindowImplBase<>::CreateSimpleToolBarCtrl(hMainWnd, IDR_ROUTER, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

		//Создание узла модуля
		LoadNodes(NULL);

		return true;
	}

	//Создание узла модуля
	bool CModuleServices::LoadNodes(sNode* Parent)
	{
		m_Node = new sNode();
		m_Node->Module = this;
		m_Node->Type = T_ITEM;
		m_Node->ParentNode = Parent;
		m_Node->ItemID = (DWORD)-1;
		m_Node->Name = L"Services";
		m_Node->IconActive		= NI_SERVICES;
		m_Node->IconInactive	= NI_SERVICES;
		return true;
	}

	//Установка узла модуля текущим
	bool CModuleServices::SetCurrent(sNode* Node)
	{
		m_View.RescanServices();
		//Делаем активными или неактивными соотвествущие кнопки на виде
		m_View.Update();


		m_CurrentNode = Node;
		return true; 
	}
}
