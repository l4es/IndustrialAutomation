#pragma once
/********************************************************************
	Создан:	        2005/02/08
	Создан:	        8.2.2005   16:26
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives\ManipulatorRotate.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives
	Имя файла:	    ManipulatorRotate
	Расширение:	    h
	Автор(ы):	    Алексей
    Класс(ы):       CManipulatorRotate
	
Описание:	Класс предназначен для вращения объекта
*********************************************************************/

#include "manipulator.h"
namespace Primitives
{

class CManipulatorRotate :
	public CManipulator
{
public:
	CManipulatorRotate(CObject* Obj);
	~CManipulatorRotate(void);

	//Рисование манипулятора
	virtual void Draw(HDC dc);
	//Обработчики сообщений
	virtual BOOL OnMouseLButtonDown(sVector Point);
	virtual BOOL OnMouseLButtonUp(sVector Point);
	virtual BOOL OnMouseMove(sVector Point);
	//Поверка, попадает ли point в область объекта
	virtual bool HitTest(sVector point);

protected:
	CBrush	_Brush;
	CPen	_Pen;
	sVector	_LastPosition;
	bool	_MLButtonDown;
};

}
