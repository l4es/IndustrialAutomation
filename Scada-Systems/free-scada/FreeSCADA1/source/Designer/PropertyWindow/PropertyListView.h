#pragma once
/********************************************************************
	Создан:	        2005/02/17
	Создан:	        17.2.2005   15:54
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\PropertyWindow\PropertyListView.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\PropertyWindow
	Имя файла:	    PropertyListView
	Расширение:	    h
	Автор(ы):	    Алексей
    Класс(ы):       CPropertyListView
	
Описание:	Класс реализует окно отображающее аттрибуты примитива
*********************************************************************/

#include "../Primitives/BaseObject.h"
#include <PropertyList/PropertyList.h>


class CPropertyListView
	: public CPropertyListImpl<CPropertyListView>
{
public:
	DECLARE_WND_SUPERCLASS(_T("WTL_PropertyListView"), GetWndClassName())

	typedef CPropertyListImpl<CPropertyListView> baseClass;
	BEGIN_MSG_MAP(CPropertyListView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_POSTCREATE, OnPostCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		REFLECTED_NOTIFY_CODE_HANDLER(PIN_ITEMCHANGED, OnItemChanged) 
		CHAIN_MSG_MAP( baseClass)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	Primitives::CBase* m_Object;

	CPropertyListView(void);
	~CPropertyListView(void);

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPostCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);

	//Установка примитива, атрибута которого будут отображаться
	void SetObject(Primitives::CBase* obj);
	//Запись значения выбранного аттрибута в объекте примитива
	void SaveObjectValue(BOOL UpdateProp=FALSE);
	//Обновление отображаемых значений аттрибутов
	void RefreshProrerty();

	CToolBarCtrl m_Toolbar;
};
