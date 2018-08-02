#pragma once
/********************************************************************
	Создан:	        2005/02/08
	Создан:	        8.2.2005   16:08
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives\Image.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives
	Имя файла:	    Image
	Расширение:	    h
	Автор(ы):	    Михаил, Алексей
    Класс(ы):       CImage
	
Описание:	Класс CImage предназначен для вывода изображения
*********************************************************************/

#include "../PropertyWindow/ObjectPropertyWindow.h"
#include "BaseObject.h"

namespace Primitives
{
	class CImage
		:public CObjImage
		,public CBaseObject<CImage>
	{
	public:
		CImage(void);
		virtual ~CImage(void);

		//Обработчики сообщений
		virtual BOOL OnMouseLButtonDown(sVector Point);
		virtual BOOL OnMouseRButtonDown(sVector Point);
		virtual BOOL OnMouseLButtonUp(sVector Point);
		virtual BOOL OnMouseMove(sVector Point);
		virtual BOOL OnKeyDown(WPARAM wParam, LPARAM lParam);
		virtual void OnSetFocus();
		virtual void OnLostFocus();
		virtual BOOL OnMouseLButtonDblClk(sVector Point);
		//Обработчик изменения обмена
		virtual void OnEdit();
		//Проверка попадает ли point на объект
		virtual bool HitTest(sVector point);
		//Рисование картинки
		virtual void Draw(HDC dc_); 
		//Загрузка картинки из архива
		virtual void SetImage(wstring Name);
		//Сохранение-загрузка объекта
		virtual void Serialize(CMemFile& file, BOOL IsLoading = 1);
		//Получить значение параметра с категорией Category и названием Name
		virtual bool GetParamValue(enCatID Category, wstring Name, sParam& Param);
		//Установить значение параметра с категорией Category и названием Name
		virtual bool SetParamValue(enCatID Category, wstring Name, sParam Param);

	protected:


	};
}
