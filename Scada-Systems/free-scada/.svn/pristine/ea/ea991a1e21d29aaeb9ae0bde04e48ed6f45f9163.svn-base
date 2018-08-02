#pragma once
/********************************************************************
	Создан:	        2005/02/09
	Создан:	        9.2.2005   13:54
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives\Text.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives
	Имя файла:	    Text
	Расширение:	    h
	Автор(ы):	    Михаил, Алексей
    Класс(ы):       CText
	
Описание:	Класс реализует в дизайнере объект отображающий текст
*********************************************************************/

#include "BaseObject.h"
namespace Primitives
{
	class CText
		:public CObjText
		,public CBaseObject<CText>
	{
	public:
		CText(void);
		~CText(void);
		//Обработчики сообщений
		virtual BOOL OnMouseLButtonDown(sVector Point);
		virtual BOOL OnMouseRButtonDown(sVector Point);
		virtual BOOL OnMouseLButtonDblClk(sVector Point);
		virtual BOOL OnMouseLButtonUp(sVector Point);
		virtual BOOL OnMouseMove(sVector Point);
		virtual BOOL OnKeyDown(WPARAM wParam, LPARAM lParam);
		virtual void OnSetFocus();
		virtual void OnLostFocus();
		//Обработчик изменения объекта
		virtual void OnEdit();
		//Рисование текста
		virtual void Draw(HDC dc);
		//Проверка попадает ли вектор point на объект
		virtual bool HitTest(sVector point);
		//Получить значение параметра
		virtual bool GetParamValue(enCatID Category, wstring Name, sParam& Param);
		//Установить значение параметра
		virtual bool SetParamValue(enCatID Category, wstring Name, sParam Param);		
		//Получить строковое значение категории
		virtual wstring StrFromCatID(enCatID id);
		//Получить выравнивание текста в виде строки
		wstring GetAlignAsText();
		//Установить выравнивание текста через строку 
		void SetAlignAsText(wstring Align);
	};
}