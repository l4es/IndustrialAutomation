#pragma once
#include "BaseModule.h"
#include "ServicesView.h"

namespace ModuleServices
{
//Класс модуля запуска, остановки и перезапуска служб
	class CModuleServices 
		: public CBaseModule
	{
	public:
		CServicesView m_View; //Вид, отображающий форму с кнопками управлениями службами

		virtual bool Detect(); // Можно ли загружать модуль
		virtual bool Load(HWND hParent, HWND hMainWnd, CRect wndRect); //Загрузка модуля
		virtual bool LoadNodes(sNode* Parent); //Создание узла

		virtual bool SetCurrent(sNode* Node); //Установка узла модуля теуущим
	};

}
