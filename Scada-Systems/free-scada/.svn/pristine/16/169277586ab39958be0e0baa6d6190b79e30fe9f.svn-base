#pragma once
/********************************************************************
	Создан:	        2005/02/08
	Создан:	        8.2.2005   17:46
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives\ManipulatorSizeLeft.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives
	Имя файла:	    ManipulatorSizeLeft
	Расширение:	    h
	Автор(ы):	    Михаил, Алексей
    Класс(ы):       CManipulatorSizeLeft
	
Описание:	Класс предназначен для изменения ширины объекта (для растяривания 
			объекта с помощью мыши за левую сторону)
*********************************************************************/

#include "manipulator.h"

namespace Primitives
{

	class CManipulatorSizeLeft :
		public CManipulator
	{
	public:
		CManipulatorSizeLeft(CObject* Obj);
		virtual ~CManipulatorSizeLeft(void);
		//Раиование манипулятора
		virtual void Draw(HDC dc);
		//Обработчики сообщений
		virtual BOOL OnMouseLButtonDown(sVector Point);
		virtual BOOL OnMouseLButtonUp(sVector Point);
		virtual BOOL OnMouseMove(sVector Point);
		//Проверка попадает ли point на объект
		virtual bool HitTest(sVector point);
		//Установить минимальный размер объекта
		virtual void SetMinSize(sVector MinSize){ _MinSize = MinSize; }
		//Получить минимальный размер объекта
		virtual sVector GetMinSize(){ return _MinSize; }

	protected:
		sVector _BrickSize;
		sVector _MinSize;
		CBrush	_Brush;
		CPen	_Pen;
		sVector	_LastPosition;
		bool	_MLButtonDown;
	};

}