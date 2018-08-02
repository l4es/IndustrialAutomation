#include "stdafx.h"
#include "ObjectMenager.h"
#include ".\manipulatorsizeBottom.h"
#include "Graphic.h"

namespace Primitives
{

CManipulatorSizeBottom::CManipulatorSizeBottom(CObject* Obj)
	: CManipulator(Obj)
	, _MLButtonDown(false)
{
	_BrickSize.x = MOVE_BLOCK_SIZE;
	_BrickSize.y = MOVE_BLOCK_SIZE;

	_MinSize.x = MOVE_BLOCK_SIZE;
	_MinSize.y = MOVE_BLOCK_SIZE;

	_Brush.CreateSolidBrush(RGB(0,50,0));
	_Pen.CreatePen(PS_SOLID, 1, RGB(255,255,255));
}

CManipulatorSizeBottom::~CManipulatorSizeBottom(void)
{
	_Brush.DeleteObject();
	_Pen.DeleteObject();
}

//Рисование манипулятора
void CManipulatorSizeBottom::Draw(HDC dc_)
{
	ATLASSERT(_Object != NULL);
	CDCHandle dc(dc_);
	CRect rc;

	sVector pos	= _Object->GetPosition();
	sVector size	= _Object->GetSize();
	sVector center = size/2;

	//Расчет вершин квадратика посередине нижней стороны
	sVector points[4];
	points[0] = sVector(-_BrickSize.x/2, -_BrickSize.y/2 + center.y);
	points[1] = sVector(+_BrickSize.x/2, -_BrickSize.y/2 + center.y);
	points[2] = sVector(+_BrickSize.x/2, +_BrickSize.y/2 + center.y);
	points[3] = sVector(-_BrickSize.x/2, +_BrickSize.y/2 + center.y);
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
	//Рисование квадратика посередине нижней стороны
	graph.Rectangle(points[0], points[1], points[2], points[3]);

	dc.SelectBrush(bold);
	dc.SelectPen(pold);
}

//Обработчик нажатия на левую кнопку мыши
BOOL CManipulatorSizeBottom::OnMouseLButtonDown(sVector Point)
{
	_LastPosition = Point;
	_MLButtonDown = true;
	CObjectMenagerPtr mng;
	//Захват манипулятора ObjectMenager-ом
	//Манипулятор будет получать все сообщения
	mng->CaptureMouse(this);

	return TRUE;
}

//Обработчик отпускания левой кнопки мыши
BOOL CManipulatorSizeBottom::OnMouseLButtonUp(sVector Point)
{
	_MLButtonDown = false;
	CObjectMenagerPtr mng;
	mng->ReleaseCaptureMouse(); //Освобождение манипулятора

	return TRUE;
}

//Обработчик перемещения мыши
BOOL CManipulatorSizeBottom::OnMouseMove(sVector Point)
{
	if(_MLButtonDown)
	{
		sVector pos	= _Object->GetPosition();
		sVector size	= _Object->GetSize();
		float angle = _Object->GetAngle();
		sVector delta = Point - _LastPosition;
	
		//Определяем вектор перемещения перпендикулярно стороне
		sVector normal(-sin(angle), cos(angle));
		sVector tmp;
		float scalar = (delta.x*normal.x + delta.y*normal.y);
		tmp.x = scalar*normal.y ;  
		tmp.y = scalar*normal.y ; 

		delta = tmp;
		
		//если объекта высота после изменения размера меньше минимальной 
		//высоту объекта не изменяем
		if(cos(angle) != 0)
		{
			if((size.y + delta.y/cos(angle))<_MinSize.y)
				delta.y = 0;
		}
		else
			if((size.y + delta.y)<_MinSize.y)
				delta.y = 0;

		//Изменение высоты объекта с учетом перемещения объекта
		//вызванного изменением высоты
		pos.y += delta.y/2;
		if(cos(angle) != 0)
			pos.x -= delta.y/2*sin(angle)/cos(angle);
		_Object->MoveTo(pos);

		sVector sizeNew = size;

		if(cos(angle) != 0)
			sizeNew.y += delta.y/cos(angle);
		else
			sizeNew.y += delta.y;

		if(sizeNew.x < _MinSize.x)
			sizeNew.x = _MinSize.x;
		if(sizeNew.y < _MinSize.y)
			sizeNew.y = _MinSize.y;

		_Object->SetSize(sizeNew);
		_Object->MoveBy((size - sizeNew)/2);
		_Object->OnEdit(); //Обновляем параметры в окне свойств

		_LastPosition = Point;
	}
	//Установка курсора в зависимости от угла поворота объекта 
	float angle = abs(_Object->GetAngle());
	while(angle > 2*PI)
		angle -= (float)(2*PI);


	if(angle < PI/4 || 
		(angle > (PI/2+PI/4) && angle<PI+PI/2))

		::SetCursor(LoadCursor(NULL, IDC_SIZENS));
	else
		::SetCursor(LoadCursor(NULL, IDC_SIZEWE));

	return TRUE;
}

//Проверка попадает ли point на объект
bool CManipulatorSizeBottom::HitTest(sVector point)
{
	sVector pos	= _Object->GetPosition();
	sVector size	= _Object->GetSize();
	sVector center = size/2;

	//Расчет вершин квадратика посередине нижней стороны
	sVector points[4];
	points[0] = sVector(-_BrickSize.x/2, -_BrickSize.y/2 + center.y);
	points[1] = sVector(+_BrickSize.x/2, -_BrickSize.y/2 + center.y);
	points[2] = sVector(+_BrickSize.x/2, +_BrickSize.y/2 + center.y);
	points[3] = sVector(-_BrickSize.x/2, +_BrickSize.y/2 + center.y);
	for(int i=0;i<4;i++)
	{
		points[i].Rotate(_Object->GetAngle());
		points[i] += pos+center;
	}
	//Проверка попадает ли point в квадратик посередине нижней стороны
	CGraphic graph(NULL);
	return graph.TestPtInRect(point, points);
}

}