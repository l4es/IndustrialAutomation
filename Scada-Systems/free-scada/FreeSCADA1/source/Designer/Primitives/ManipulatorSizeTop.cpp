#include "stdafx.h"
#include "ObjectMenager.h"
#include "Graphic.h"
#include ".\manipulatorsizetop.h"

namespace Primitives
{

CManipulatorSizeTop::CManipulatorSizeTop(CObject* Obj)
	: CManipulator(Obj)
	, _MLButtonDown(false)
{
	_BrickSize.x = MOVE_BLOCK_SIZE;
	_BrickSize.y = MOVE_BLOCK_SIZE;

	_Brush.CreateSolidBrush(RGB(0,50,0));
	_Pen.CreatePen(PS_SOLID, 1, RGB(255,255,255));
}

CManipulatorSizeTop::~CManipulatorSizeTop(void)
{
	_Brush.DeleteObject();
	_Pen.DeleteObject();
}

//Рисование манипулятора
void CManipulatorSizeTop::Draw(HDC dc_)
{
	ATLASSERT(_Object != NULL);
	CDCHandle dc(dc_);
	CRect rc;

	sVector pos	= _Object->GetPosition();
	sVector size	= _Object->GetSize();
	sVector center = size/2;
	//Расчет вершин квадратика посередине верхней стороны
	sVector points[4];
	points[0] = sVector(-_BrickSize.x/2, -_BrickSize.y/2 - center.y);
	points[1] = sVector(+_BrickSize.x/2, -_BrickSize.y/2 - center.y);
	points[2] = sVector(+_BrickSize.x/2, +_BrickSize.y/2 - center.y);
	points[3] = sVector(-_BrickSize.x/2, +_BrickSize.y/2 - center.y);
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
	//Рисование квадратика посередине верхней стороеы
	graph.Rectangle(points[0], points[1], points[2], points[3]);

	dc.SelectBrush(bold);
	dc.SelectPen(pold);
}

//Обработчик нажатия левой кнопки мыши
BOOL CManipulatorSizeTop::OnMouseLButtonDown(sVector Point)
{
	_LastPosition = Point;
	_MLButtonDown = true;
	CObjectMenagerPtr mng;
	//Захват манипулятора ObjectMenager-ом
	//Все сообщения будут приходить к манипулятору
	mng->CaptureMouse(this);

	return TRUE;
}

//Обработчик отпускания кнопки мыши
BOOL CManipulatorSizeTop::OnMouseLButtonUp(sVector Point)
{
	_MLButtonDown = false;
	CObjectMenagerPtr mng;
	//Освобождение  манипулятора ObjectMenager-ом
	mng->ReleaseCaptureMouse();
	return TRUE;
}

//Обработчик перемещения мыши
BOOL CManipulatorSizeTop::OnMouseMove(sVector Point)
{
	if(_MLButtonDown)
	{
		sVector pos	= _Object->GetPosition();
		sVector size	= _Object->GetSize();
		float angle = _Object->GetAngle();

		sVector delta = Point - _LastPosition;
		//Расчет вектора перемещения по нормали к верхней стороне
		sVector normal(-sin(angle), cos(angle));
		sVector tmp;
		float scalar = (delta.x*normal.x + delta.y*normal.y);
		tmp.x = scalar*normal.y ;  
		tmp.y = scalar*normal.y ; 

		delta = tmp;
		
		//Если высота объекта после перемещения меньше минимальной
		//изменение размеров не выполняется
		if(cos(angle) != 0)
		{
			if((size.y - delta.y/cos(angle))<5)
				delta.y = 0;
		}
		else
			if((size.y - delta.y)<5)
				delta.y = 0;

		//Расчет новых размеров и позиции объекта
		//Изменение размеров и позиции объекта
		pos.y += delta.y/2;
		if(cos(angle) != 0)
			pos.x -= delta.y/2*sin(angle)/cos(angle);
		_Object->MoveTo(pos);

		sVector sizeNew = size;
		
		if(cos(angle) != 0)
			sizeNew.y -= delta.y/cos(angle);
		else
			sizeNew.y -= delta.y;

		if(sizeNew.x < _MinSize.x)
			sizeNew.x = _MinSize.x;
		if(sizeNew.y < _MinSize.y)
			sizeNew.y = _MinSize.y;

		_Object->SetSize(sizeNew);
		_Object->MoveBy((size - sizeNew)/2);

		//Обновление параметров объекта в окне свойств
		_Object->OnEdit();

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

//Проверка попадает ли вектор point на манипулятор
bool CManipulatorSizeTop::HitTest(sVector point)
{
	sVector pos	= _Object->GetPosition();
	sVector size	= _Object->GetSize();
	sVector center = size/2;

	//расчет вершин квадратика посередине верхней стороны
	sVector points[4];
	points[0] = sVector(-_BrickSize.x/2, -_BrickSize.y/2 - center.y);
	points[1] = sVector(+_BrickSize.x/2, -_BrickSize.y/2 - center.y);
	points[2] = sVector(+_BrickSize.x/2, +_BrickSize.y/2 - center.y);
	points[3] = sVector(-_BrickSize.x/2, +_BrickSize.y/2 - center.y);
	for(int i=0;i<4;i++)
	{
		points[i].Rotate(_Object->GetAngle());
		points[i] += pos+center;
	}

	CGraphic graph(NULL);
	//Рисование квадратика посередине верхней стороны
	return graph.TestPtInRect(point, points);
}

}