#pragma once
/********************************************************************
	Создан:	        2005/02/09
	Создан:	        9.2.2005   14:20
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\ActionsView.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer
	Имя файла:	    ActionsView
	Расширение:	    h
	Автор(ы):	    Алексей
    Класс(ы):		CPropertyList, CActionsView
	
Описание:	Класс CPropertyList реализует окно свойств, 
			класс CActionsView реализует окно позволяющее устанавливать 
			парамертры действий, присущих выбранному объекту
*********************************************************************/

#include "resource.h"


class CPropertyList
	: public CPropertyListImpl<CPropertyList>
{
public:
	DECLARE_WND_SUPERCLASS(_T("WTL_PropertyList"), GetWndClassName())

	typedef CPropertyListImpl<CPropertyList> baseClass;
	BEGIN_MSG_MAP(CPropertyListView)
		CHAIN_MSG_MAP(baseClass)
	//	FORWARD_NOTIFICATIONS()
	//	DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	
};

class CActionsView
: public CWindowImpl<CActionsView>
{
public:
	typedef CWindowImpl<CActionsView> baseClass ;
	BEGIN_MSG_MAP(CActionsView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_POSTCREATE, OnPostCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		COMMAND_ID_HANDLER(ID_ACTION_MOVER, OnAddActionMover)
		COMMAND_ID_HANDLER(ID_ACTION_SIZER, OnAddActionSizer)
		COMMAND_ID_HANDLER(ID_ACTION_ROTATE, OnAddActionRotate)
		COMMAND_ID_HANDLER(ID_ACTION_VISUAL, OnAddActionVisual)
		COMMAND_ID_HANDLER(ID_ACTION_COLOR, OnAddActionColor)
		COMMAND_ID_HANDLER(ID_ACTION_PLAY_SOUND, OnAddActionPlaySound)
		COMMAND_ID_HANDLER(ID_ACTION_CHANGE_IMAGE, OnAddActionChangeImage)
		COMMAND_ID_HANDLER(ID_ACTION_CURVE, OnAddActionCurve)
		COMMAND_ID_HANDLER(ID_DEL_ACTION, OnDeleteAction)
		NOTIFY_CODE_HANDLER(TBN_DROPDOWN, OnToolbarDropDown)
		NOTIFY_CODE_HANDLER(PIN_ITEMCHANGED, OnItemChanged) 
		//CHAIN_MSG_MAP_MEMBER(m_Prop)
		REFLECT_NOTIFICATIONS()
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP();
	CActionsView();

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPostCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	//Разворачивание меню выбора действий
	LRESULT OnToolbarDropDown(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	//Добавление действия перемещения
	LRESULT OnAddActionMover(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Добавление действия изменения размеров
	LRESULT OnAddActionSizer(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Добавление действия вращения
	LRESULT OnAddActionRotate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Добавление действия изменения видимости
	LRESULT OnAddActionVisual(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Добавление действия изменения цвета
	LRESULT OnAddActionColor(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddActionPlaySound(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddActionChangeImage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddActionCurve(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	//Удаление действия
	LRESULT OnDeleteAction(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Обработчик смены выбранного пункта
	LRESULT OnItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled);

	void SetObject(Primitives::CBase* obj);
	//Добавление в окно параметров действий, которые есть у объекта
	void AddActionsToList();
	//Добавление действия в окно параметров
	void AddToList(CAction* Action);
	//Сохранение значения выбранного параметра
	void SaveValue();
	
	//Получить значение пункта в виде вещественного значения
	float FloatFromProp(HPROPERTY prop);
	//Получить значение пункта в виде строки
	wstring StringFromProp(HPROPERTY prop);
	
	
	//Устанавливаем пункт меню выбора действий действительным или не действительным
	void SetMenuItemEnabled(enActionType Type, bool Enabled);
	//Получить ID пункта меню выбора действий по типу действия
	UINT GetIDFromType(enActionType Type);
	//Получить строковое название действия по типу действия
	wstring GetNameFromType(enActionType Type);
	
	CMenu m_ActionsMenu;
	Primitives::CBase* m_Object; //выбранный объект
	CPropertyList m_Prop; //окно параметров действий
	CToolBarXPCtrl m_Toolbar;
	TActionsMap	m_ActionsMap; //карта тип действия-список структур sCtrlParam(название-контрол)

};
