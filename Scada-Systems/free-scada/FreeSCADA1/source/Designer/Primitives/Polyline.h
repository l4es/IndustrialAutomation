#pragma once
/********************************************************************
	Создан:	        2005/02/09
	Создан:	        9.2.2005   12:46
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives\Polyline.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives
	Имя файла:	    Polyline
	Расширение:	    h
	Автор(ы):	    Михаил, Алексей
    Класс(ы):       CPolyline
	
Описание:	Класс реализует объект полилинии для дизайнера
*********************************************************************/

#include "../PropertyWindow/ObjectPropertyWindow.h"
#include "BaseObject.h"

namespace Primitives
{
	class CPolyline
		: public CObjPolyline
		,public CBaseObject<CPolyline>
	{
	public:
		CPolyline(void);
		virtual ~CPolyline(void);
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
		//Рисование полилинии
		virtual void Draw(HDC dc); 
		//Проверка попадает ли point на полилинию
		virtual bool HitTest(sVector point);

		


	protected:
		BOOL	_EditVertex; //Если TRUE - режим редактирования точек
	};
}
