#include "stdafx.h"
#include "ObjectMenager.h"
#include "Graphic.h"
#include ".\manipulatorsizeright.h"

namespace Primitives
{

CManipulatorSizeRight::CManipulatorSizeRight(CObject* Obj)
	: CManipulator(Obj)
	, _MLButtonDown(false)
{
	//Установка размеров квадратика
	_BrickSize.x = MOVE_BLOCK_SIZE;
	_BrickSize.y = MOVE_BLOCK_SIZE;

	//Установка минимальных размеров объекта
	_MinSize.x = MOVE_BLOCK_SIZE;
	_MinSize.y = MOVE_BLOCK_SIZE;

	_Brush.CreateSolidBrush(RGB(0,50,0));
	_Pen.CreatePen(PS_SOLID, 1, RGB(255,255,255));
}

CManipulatorSizeRight::~CManipulatorSizeRight(void)
{
	_Brush.DeleteObject();
	_Pen.DeleteObject();
}

//Рисование манипулятора
void CManipulatorSizeRight::Draw(HDC dc_)
{
	ATLASSERT(_Object != NULL);
	CDCHandle dc(dc_);
	CRect rc;
	
	sVector pos		= _Object->GetPosition();
	sVector size	= _Object->GetSize();
	sVector center	= size/2;
	//Расчет вершин квадратика посередине правой стороны
	sVector points[4];
	points[0] = sVector(-_BrickSize.x/2 + center.x, -_BrickSize.y/2);
	points[1] = sVector(+_BrickSize.x/2 + center.x, -_BrickSize.y/2);
	points[2] = sVector(+_BrickSize.x/2 + center.x, +_BrickSize.y/2);
	points[3] = sVector(-_BrickSize.x/2 + center.x, +_BrickSize.y/2);
	CObjectMenagerPtr mng;
	for(int i=0;i<4;i++)
	{
		points[i].Rotate(_Object->GetAngle());
		points[i] += pos+center;
		points[i] = mng->GlobalToScreen(points[i]);
	}

	CGraphic graph(dc_);
	
	CBrushHandle bold = dc.SelectBrush(_Brush);
	CPenHandle pold = dc.SelectPen(_Pen);

	//Рисование квадратика посередине правой стороны
	graph.Rectangle(points[0], points[1], points[2], points[3]);

	dc.SelectBrush(bold);
	dc.SelectPen(pold);
}

//Обработчик нажатия левой кнопки мыши
BOOL CManipulatorSizeRight::OnMouseLButtonDown(sVector Point)
{
	_LastPosition = Point;
	_MLButtonDown = true;
	CObjectMenagerPtr mng;
	//Захват ObjectMenager-ом манипулятора
	//все сообщения будет получать этот манипулятор
	mng->CaptureMouse(this);

	return TRUE;
}

BOOL CManipulatorSizeRight::OnMouseLButtonUp(sVector Point)
{
	_MLButtonDown = false;
	CObjectMenagerPtr mng;
	//Освобождение манипулятора ObjectMenager-ом
	mng->ReleaseCaptureMouse();

	return TRUE;
}

//Обработчик перемещения мыши
BOOL CManipulatorSizeRight::OnMouseMove(sVector Point)
{
	//////////////////////////////////////////////////////////////////////////
	if(_MLButtonDown)
	{
		sVector pos	= _Object->GetPosition();
		sVector size	= _Object->GetSize();
		float angle = _Object->GetAngle();
		sVector delta = Point - _LastPosition;

		//Вычисление вектора перемещения по нормали к правой стороне
		sVector normal(cos(angle), sin(angle));
		sVector tmp;
		float scalar = (delta.x*normal.x + delta.y*normal.y);
		tmp.x = scalar*normal.x ;  
		tmp.y = scalar*normal.x ; 

		delta = tmp;
		//Если после перемещения мыши размер становится меньше минимального
		//размеры объекта не изменяем
		if(cos(angle) != 0)
		{
			if((size.x + delta.x/cos(angle))<_MinSize.x)
				delta.x = 0;
		}
		else
			if((size.x + delta.x)<_MinSize.x)
				delta.x = 0;

		//Вычисления разменров и позиции объекта после перемещения
		//Перемещение объекта и изменение размеров
		pos.x += delta.x/2;
		if(cos(angle) != 0)
			pos.y += delta.x/2*sin(angle)/cos(angle);
		_Object->MoveTo(pos);

		sVector sizeNew = size;

		if(cos(angle) != 0)
			sizeNew.x += delta.x/cos(angle);
		else
			sizeNew.x += delta.x;

		if(sizeNew.x < _MinSize.x)
			sizeNew.x = _MinSize.x;
		if(sizeNew.y < _MinSize.y)
			sizeNew.y = _MinSize.y;

		_Object->SetSize(sizeNew);
		_Object->MoveBy((size - sizeNew)/2);
		//Обновление параметров в окне свойств
		_Object->OnEdit();

		_LastPosition = Point;
	}


	//Установка курсора в зависимости от угла поворота объекта
	float angle = abs(_Object->GetAngle());
	while(angle > 2*PI)
		angle -= (float)(2*PI);

    if(angle < PI/4 || 
		(angle > (PI/2+PI/4) && angle<PI+PI/2))
		::SetCursor(LoadCursor(NULL, IDC_SIZEWE));
	else
		::SetCursor(LoadCursor(NULL, IDC_SIZENS));
	return TRUE;
}

//Проверка попадает ли вектор point на манипулятор
bool CManipulatorSizeRight::HitTest(sVector point)
{
	sVector pos	= _Object->GetPosition();
	sVector size	= _Object->GetSize();
	sVector center = size/2;

	//Вершины квадратика посередине правой стороны
	sVector points[4];
	points[0] = sVector(-_BrickSize.x/2 + center.x, -_BrickSize.y/2);
	points[1] = sVector(+_BrickSize.x/2 + center.x, -_BrickSize.y/2);
	points[2] = sVector(+_BrickSize.x/2 + center.x, +_BrickSize.y/2);
	points[3] = sVector(-_BrickSize.x/2 + center.x, +_BrickSize.y/2);
	for(int i=0;i<4;i++)
	{
		points[i].Rotate(_Object->GetAngle());
		points[i] += pos+center;
	}

	CGraphic graph(NULL);
	//Проверка, попадает ли point на квадратик посередине правой стороны
	return graph.TestPtInRect(point, points);
}

}