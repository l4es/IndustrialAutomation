#pragma once
/********************************************************************
	Создан:	        2004/12/08
	Создан:	        8.12.2004   11:45
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\Manipulator.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Library
	Имя файла:	    Manipulator
	Расширение:	    h
	Автор(ы):	    Михаил
    Класс(ы):       CManipulator
	
Описание:	Базовый класс для манипуляторов.
*********************************************************************/

//#include "object.h"
namespace Primitives
{

class CManipulator :
	public CObject
{
public:
	//Constructor/Destructor
	CManipulator(CObject* Obj);
	virtual ~CManipulator(void);

	//Прорисовка
	virtual void Draw(HDC dc)=0;

	//////////////////////////////////////////////////////////////////////////
	// Обработка сообщений
	virtual BOOL OnMouseLButtonDown(sVector Point){return FALSE;};	//WM_LBUTTONDOWN
	virtual BOOL OnMouseLButtonUp(sVector Point){return FALSE;};	//WM_LBUTTONUP
	virtual BOOL OnMouseMove(sVector Point){return FALSE;};			//WM_MOUSEMOVE
	//////////////////////////////////////////////////////////////////////////

protected:
	CObject* _Object;	//Объект с которым работает манипулятор
};

}