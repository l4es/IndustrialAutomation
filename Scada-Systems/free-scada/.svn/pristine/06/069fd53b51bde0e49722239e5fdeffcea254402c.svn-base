#include "common.h"
#include ".\graphic.h"

//Рисование линии между двумя точками (в экранных координатах)
void CGraphic::Line(sVector p1, sVector p2)
{
	::MoveToEx(_dc, (int)p1.x, (int)p1.y, NULL);
	::LineTo(_dc, (int)p2.x, (int)p2.y);
}

//Рисование прямоугольника
void CGraphic::Rectangle(sVector p1, sVector p2)
{
	POINT points[4] =
	{
		(LONG)p1.x, (LONG)p1.y,
		(LONG)p2.x, (LONG)p1.y,
		(LONG)p2.x, (LONG)p2.y,
		(LONG)p1.x, (LONG)p2.y
	};
	::Polygon(_dc, (LPPOINT)points, 4);
}

//Рисование четырехугольника
void CGraphic::Rectangle(sVector p1, sVector p2, sVector p3, sVector p4)
{
	POINT points[4] =
	{
		(LONG)p1.x, (LONG)p1.y,
		(LONG)p2.x, (LONG)p2.y,
		(LONG)p3.x, (LONG)p3.y,
		(LONG)p4.x, (LONG)p4.y
	};
	::Polygon(_dc, (LPPOINT)points, 4);
}

//Проверка попадает ли точка в заданный четырехугольник
bool CGraphic::TestPtInRect(sVector Point, TRect Rect)
{
	//Проверяем все ли нормали "смотрят" от точки.
	// Если так, то точка внутри полигона
	sVector normals[4];
	sVector ToPoint[4];
	normals[0] = Rect[1]-Rect[0];
	normals[1] = Rect[2]-Rect[1];
	normals[2] = Rect[3]-Rect[2];
	normals[3] = Rect[0]-Rect[3];
	ToPoint[0] = Point - Rect[0];
	ToPoint[1] = Point - Rect[1];
	ToPoint[2] = Point - Rect[2];
	ToPoint[3] = Point - Rect[3];

	for(int i=0;i<4;i++)
	{
		normals[i].Rotate((float)(-PI/2.0f));
		if(AngleBetweenVector(normals[i], ToPoint[i])<(PI/2))
			return false;
	}
	return true;
}