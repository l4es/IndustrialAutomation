#pragma once
#include "BaseModule.h"
//Класс менеджера модулей 
class CMainFrame;
class CModuleManager
{
public:
	typedef vector<CBaseModule*> TVModule;

	TVModule m_Modules;//Вектор, содержащий модули
	sNode* _CurrentNode;//Текущий узел

	CMainFrame* m_MainFrame;
	

	CModuleManager(void);
	~CModuleManager(void);

	bool LoadModules(HWND hParent, HWND hMainWnd, CRect wndRect);//Загрузка модулей
	HWND GetCurrentHWND();//Получить HWND вида (правого окна) для выбранного узла
	void OnSetCurrent(sNode* Node);//Установка текущего узла
	bool OnExpandNode(sNode* Node);//Раскрытие узла
	sNode* GetCurrentNode();//Получить текущий узел
};
