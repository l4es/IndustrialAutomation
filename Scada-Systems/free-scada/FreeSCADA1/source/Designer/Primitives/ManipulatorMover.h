#pragma once
/********************************************************************
	Создан:	        2005/02/08
	Создан:	        8.2.2005   16:10
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives\ManipulatorMover.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives
	Имя файла:	    ManipulatorMover
	Расширение:	    h
	Автор(ы):	    Михаил
    Класс(ы):       CManipulatorMover
	
Описание:	Класс предназначен для перемещения объекта
*********************************************************************/

#include "manipulator.h"
namespace Primitives
{

class CManipulatorMover :
	public CManipulator
{
public:
	CManipulatorMover(CObject* Obj);
	~CManipulatorMover(void);

	//Обработчики сообщений
	virtual BOOL OnMouseLButtonDown(sVector Point);
	virtual BOOL OnMouseLButtonUp(sVector Point);
	virtual BOOL OnMouseMove(sVector Point);

	//Проверка попадает ли point на объект
	virtual bool HitTest(sVector point);
	//Рисование объекта
	virtual void Draw(HDC dc);

protected:
	sVector _BrickSize; 
	CBrush	_Brush;
	sVector	_LastPosition;
	bool	_MLButtonDown;
};

}
