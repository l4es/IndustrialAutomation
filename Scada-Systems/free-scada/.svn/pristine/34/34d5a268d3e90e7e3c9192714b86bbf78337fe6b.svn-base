#pragma once
/********************************************************************
	Создан:	        2005/02/08
	Создан:	        8.2.2005   15:43
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives\Input.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives
	Имя файла:	    Input
	Расширение:	    h
	Автор(ы):	    Михаил, Алексей
    Класс(ы):       CInput
	
Описание:	Класс CInput предназначен для ввода текста или числа
*********************************************************************/


#include "../PropertyWindow/ObjectPropertyWindow.h"
#include "BaseObject.h"

namespace Primitives
{
	class CInput
		:public CObjInput
		,public CBaseObject<CInput>
	{
	public:
		CInput(void);
		virtual ~CInput(void);

		//Обработчики сообщений
		virtual BOOL OnMouseLButtonDown(sVector Point);
		virtual BOOL OnMouseRButtonDown(sVector Point);
		virtual BOOL OnMouseLButtonUp(sVector Point);
		virtual BOOL OnMouseMove(sVector Point);
		virtual BOOL OnMouseLButtonDblClk(sVector Point);
		virtual BOOL OnKeyDown(WPARAM wParam, LPARAM lParam);
		virtual bool HitTest(sVector point);
		virtual void OnSetFocus();
		virtual void OnLostFocus();

		//Рисование объекта
		virtual void Draw(HDC dc_); 
		
		//Обработчик изменения объекта
		virtual void OnEdit();
		//Сохранение - загрузка объекта
		virtual void Serialize(CMemFile& file, BOOL IsLoading = 1);
		//Получить значение параметра с категорией Category и названием Name
		virtual bool GetParamValue(enCatID Category, wstring Name, sParam& Param);
		//Установить значение параметра с категорией Category и названием Name
		virtual bool SetParamValue(enCatID Category, wstring Name, sParam Param);
		//Получить имя шрифта
		wstring GetFontName(){return GetFont().Name;};
		//Установить имя шрифта
		void SetFontName(wstring FontName){GetFont().Name = FontName;};
	};
}
