#pragma once
/********************************************************************
	Создан:	        2005/02/09
	Создан:	        9.2.2005   13:16
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives\Select.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives
	Имя файла:	    Select
	Расширение:	    h
	Автор(ы):	    Алексей
    Класс(ы):       CSelect
	
Описание:	Класс реализующий Selection в дизайнере(объект содержащий внутри себя в
ыбранные объекты)

*********************************************************************/

#include "BaseObject.h"
#include "Primitives.h"


namespace Primitives
{
	class CSelect
		: public CSelection
		, public CBaseObject<CSelect>

	{
	public:
		CSelect(void);
		~CSelect(void);
		//Обработчики сообщений
		virtual BOOL OnMouseLButtonDown(sVector Point);
		virtual BOOL OnMouseRButtonDown(sVector Point);
		virtual BOOL OnMouseLButtonDblClk(sVector Point);
		virtual BOOL OnMouseLButtonUp(sVector Point);
		virtual BOOL OnMouseMove(sVector Point);
		virtual BOOL OnKeyDown(WPARAM wParam, LPARAM lParam);
		virtual void OnSetFocus();
		virtual void OnLostFocus();
		//Рисование объекта
		virtual void Draw(HDC dc);
		//Получить параметр объекта
		virtual bool GetParamValue(enCatID Category, wstring Name, sParam& Param );
		//Установить параметр объекта
		virtual bool SetParamValue(enCatID Category, wstring Name, sParam Param);
		//Обработчик изменения объекта		
		virtual void OnEdit();
		//Добавление объекта в Selection
		virtual void AddObject(CObject* obj);
		//Приведение наследника CObject к CBase
		CBase* GetBase(CObject* obj)
		{
			switch(obj->GetTypeID()) 
			{
				case _RECTANGLE:    return static_cast<CRectangle*>(obj);
				case _POLYLINE:		return static_cast<CPolyline*>(obj);
				case _ELLIPSE:		return static_cast<CEllipse*>(obj);
				case _IMAGE:		return static_cast<CImage*>(obj);
				case _TEXT:			return static_cast<CText*>(obj);
				case _BUTTON:		return static_cast<CButton*>(obj);
				case _INPUT:		return static_cast<CInput*>(obj);
				case _SOUND:		return static_cast<CSound*>(obj);
				case _GRAPH:		return static_cast<CGraph*>(obj);
				default:
					ATLASSERT("Неизвестный тип объекта" == NULL);
			}
			return NULL;
		}

	};
}
