#pragma once
#include "../PropertyWindow/ObjectPropertyWindow.h"
#include "BaseObject.h"

namespace Primitives
{
	//Класс реализует объект кнопку
	class CButton
		:public CObjButton
		,public CBaseObject<CButton>
	{
	public:
		CButton(void);
		virtual ~CButton(void);
		//Обработчики сообщений
		virtual BOOL OnMouseLButtonDown(sVector Point);
		virtual BOOL OnMouseRButtonDown(sVector Point);
		virtual BOOL OnMouseLButtonUp(sVector Point);
		virtual BOOL OnMouseLButtonDblClk(sVector Point);
		virtual BOOL OnMouseMove(sVector Point);
		virtual BOOL OnKeyDown(WPARAM wParam, LPARAM lParam);
		virtual void OnSetFocus();
		virtual void OnLostFocus();
		
		//Событие изменение объекта
		virtual void OnEdit();
		//Проверка, попадает ли point на объект
		virtual bool HitTest(sVector point);
		//Рисование кнопки
		virtual void Draw(HDC dc_); 
		//Сохранение-загрузка объекта
		virtual void Serialize(CMemFile& file, BOOL IsLoading = 1);
		//Получить значение параметра с категорией Category и названием Name
		virtual bool GetParamValue(enCatID Category, wstring Name, sParam& Param);
		//Установить значение параметра с категорией Category и названием Name
		virtual bool SetParamValue(enCatID Category, wstring Name, sParam Param);
		//Получить строковое значение категории
		virtual wstring StrFromCatID(enCatID id);
	protected:


	};
}