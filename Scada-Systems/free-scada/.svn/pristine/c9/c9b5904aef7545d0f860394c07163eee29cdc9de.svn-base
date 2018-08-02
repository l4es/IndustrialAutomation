#pragma once
/********************************************************************
	Создан:	        2005/02/08
	Создан:	        8.2.2005   17:21
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives\ManipulatorSizeBottom.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives
	Имя файла:	    ManipulatorSizeBottom
	Расширение:	    h
	Автор(ы):	    Михаил, Алексей
    Класс(ы):       CManipulatorSizeBottom
	
Описание:	Класс предназначен для изменения размера объекта (растягивания за
			нижнюю сторону)
*********************************************************************/

#include "manipulator.h"

namespace Primitives
{

class CManipulatorSizeBottom :
	public CManipulator
{
public:
	CManipulatorSizeBottom(CObject* Obj);
	virtual ~CManipulatorSizeBottom(void);
	//Рисование манипулятора
	virtual void Draw(HDC dc);
	//Обработчики сообщений
	virtual BOOL OnMouseLButtonDown(sVector Point);
	virtual BOOL OnMouseLButtonUp(sVector Point);
	virtual BOOL OnMouseMove(sVector Point);
	//Поверка попадает ли point на объект
	virtual bool HitTest(sVector point);
	//Задать минимальный размер объекта
	virtual void SetMinSize(sVector MinSize){ _MinSize = MinSize; }
	//Получить минимальный размер объекта
	virtual sVector GetMinSize(){ return _MinSize; }

protected:
	sVector _BrickSize; //размер квадратика за который растягивается объект
	CBrush	_Brush;
	CPen	_Pen;
	sVector	_LastPosition;
	bool	_MLButtonDown;
	sVector	_MinSize;	//Минимальный размер объекта который манипулятор изменяет
};

}