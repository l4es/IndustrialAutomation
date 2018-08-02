#pragma once
#include "../PropertyWindow/ObjectPropertyWindow.h"
#include "BaseObject.h"

namespace Primitives
{
	class CEllipse
		:public CObjEllipse
		,public CBaseObject<CEllipse>
	{
	public:
		CEllipse(void);
		virtual ~CEllipse(void);
		
		//Обработчики сообщений
		virtual BOOL OnMouseLButtonDown(sVector Point);
		virtual BOOL OnMouseRButtonDown(sVector Point);
		virtual BOOL OnMouseLButtonDblClk(sVector Point);
		virtual BOOL OnMouseLButtonUp(sVector Point);
		virtual BOOL OnMouseMove(sVector Point);
		virtual BOOL OnKeyDown(WPARAM wParam, LPARAM lParam);
		virtual void OnSetFocus();
		virtual void OnLostFocus();
		virtual void OnEdit();
		//Рисование эллипса
		virtual void Draw(HDC dc);
		//Проверка, попадает ли point на объект
		virtual bool HitTest(sVector point);
		//Получить значение параметра с категорией Category и названием Name
		virtual bool GetParamValue(enCatID Category, wstring Name, sParam& Param);
		//Установить значение параметра с категорией Category и названием Name
		virtual bool SetParamValue(enCatID Category, wstring Name, sParam Param);
		
	protected:


	};
}
