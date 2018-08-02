#pragma once
/********************************************************************
	Создан:	        2005/02/09
	Создан:	        9.2.2005   9:32
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives\ManipulatorSizeRight.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives
	Имя файла:	    ManipulatorSizeRight
	Расширение:	    h
	Автор(ы):	    Михаил, Алексей
    Класс(ы):       CManipulatorSizeRight
	
Описание:	Класс реализует манипулятор для изменения размера объекта 
			путем растягивания его за правую  стороу
*********************************************************************/

#include "manipulator.h"

namespace Primitives
{

	class CManipulatorSizeRight :
		public CManipulator
	{
	public:
		CManipulatorSizeRight(CObject* Obj);
		virtual ~CManipulatorSizeRight(void);
		//Рисование манипулятора
		virtual void Draw(HDC dc);
		//Обработчики сообщений
		virtual BOOL OnMouseLButtonDown(sVector Point);
		virtual BOOL OnMouseLButtonUp(sVector Point);
		virtual BOOL OnMouseMove(sVector Point);
		//Проверка попадает ли вектор point на объект
		virtual bool HitTest(sVector point);
		//Установка минимальных размеров объекта
		virtual void SetMinSize(sVector MinSize){ _MinSize = MinSize; }
		//Получение значений минимальных размеров объекта
		virtual sVector GetMinSize(){ return _MinSize; }


	protected:
		sVector _BrickSize; //Размеры квадратика рисуемого посередине правой стороны
		sVector _MinSize; //Минимальные размеры объекта
		CBrush	_Brush;
		CPen	_Pen;
		sVector	_LastPosition; 
		bool	_MLButtonDown;
	};

}
