#pragma once
/********************************************************************
	Создан:	        2005/02/09
	Создан:	        9.2.2005   10:00
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives\ManipulatorSizeTop.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives
	Имя файла:	    ManipulatorSizeTop
	Расширение:	    h
	Автор(ы):	    Михаил, Алексей
    Класс(ы):       CManipulatorSizeTop
	
Описание:	Класс предназначен для изменения размера объекта  путем
			растягивания его за верхнюю сторону
*********************************************************************/

#include "manipulator.h"

namespace Primitives
{

	class CManipulatorSizeTop :
		public CManipulator
	{
	public:
		CManipulatorSizeTop(CObject* Obj);
		virtual ~CManipulatorSizeTop(void);
		//Рисование манипулятора
		virtual void Draw(HDC dc);
		//Обработчики  сообщений
		virtual BOOL OnMouseLButtonDown(sVector Point);
		virtual BOOL OnMouseLButtonUp(sVector Point);
		virtual BOOL OnMouseMove(sVector Point);
		//Проверка попадает ли вектор point на манипулятор
		virtual bool HitTest(sVector point);
		//Получить минимальные размеры объекта
		virtual void SetMinSize(sVector MinSize){ _MinSize = MinSize; }
		//Установить минимальные размеры объекта
		virtual sVector GetMinSize(){ return _MinSize; }

	protected:
		sVector _BrickSize; //Размеры квадратика посередине верхней стороны
		CBrush	_Brush;
		CPen	_Pen;
		sVector	_LastPosition;
		bool	_MLButtonDown;
		sVector	_MinSize; //Минимальный размер объекта
	};

}
