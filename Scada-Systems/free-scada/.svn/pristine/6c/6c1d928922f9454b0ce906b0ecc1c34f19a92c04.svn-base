#pragma once
/********************************************************************
	Создан:	        2004/12/08
	Создан:	        8.12.2004   11:32
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\Graphic.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Library
	Имя файла:	    Graphic
	Расширение:	    h
	Автор(ы):	    Алексей, Михаил
    Класс(ы):       CGraphic
	
Описание:	Класс содержит некоторые вспомогательные функции для работы с графикой.
*********************************************************************/

#include "Common.h"

typedef sVector TRect[4];

class CGraphic
{
public:
	CGraphic(HDC dc):_dc(dc){};

	//Рисование линии между двумя точками (в экранных координатах)
	void Line(sVector p1, sVector p2);

	//Рисование прямоугольника
	void Rectangle(sVector p1, sVector p2);
	//Рисование четырехугольника
	void Rectangle(sVector p1, sVector p2, sVector p3, sVector p4);

	//Проверка попадает ли точка в заданный четырехугольник
	bool TestPtInRect(sVector Point, TRect Rect);

protected:
	HDC _dc;
};