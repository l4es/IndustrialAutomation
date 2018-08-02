#include "stdafx.h"
#include "ObjectMenager.h"
#include "Graphic.h"
#include ".\manipulatorsizeleft.h"

namespace Primitives
{

CManipulatorSizeLeft::CManipulatorSizeLeft(CObject* Obj)
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

CManipulatorSizeLeft::~CManipulatorSizeLeft(void)
{
	_Brush.DeleteObject();
	_Pen.DeleteObject();
}

//Рисование манипулятора
void CManipulatorSizeLeft::Draw(HDC dc_)
{
	ATLASSERT(_Object != NULL);
	CDCHandle dc(dc_);
	CRect rc;
	
	sVector pos	= _Object->GetPosition();
	sVector size	= _Object->GetSize();
	sVector center = size/2;

	//Расчет вершин квадратика посередине левой стороны
	sVector points[4];
	points[0] = sVector(-_BrickSize.x/2 - center.x, -_BrickSize.y/2);
	points[1] = sVector(+_BrickSize.x/2 - center.x, -_BrickSize.y/2);
	points[2] = sVector(+_BrickSize.x/2 - center.x, +_BrickSize.y/2);
	points[3] = sVector(-_BrickSize.x/2 - center.x, +_BrickSize.y/2);
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

	//Рисование квадратика посередине левой стороны
	graph.Rectangle(points[0], points[1], points[2], points[3]);

	dc.SelectBrush(bold);
	dc.SelectPen(pold);
}

//Обработчик нажатия кнопки мыши
BOOL CManipulatorSizeLeft::OnMouseLButtonDown(sVector Point)
{
	_LastPosition = Point;
	_MLButtonDown = true;
	CObjectMenagerPtr mng;
	//Захват ObjectMenager-ом манипулятора
	//Все сообщения будет получать этот манипулятор
	mng->CaptureMouse(this);

	return TRUE;
}

//Обработчик отпускания кнопки мыши
BOOL CManipulatorSizeLeft::OnMouseLButtonUp(sVector Point)
{
	_MLButtonDown = false;
	CObjectMenagerPtr mng;
	//Освобождение манипулятора ObjectMenager-ом
	mng->ReleaseCaptureMouse();

	return TRUE;
}

//Обработчик перемещения мыши
BOOL CManipulatorSizeLeft::OnMouseMove(sVector Point)
{
	if(_MLButtonDown)
	{
		sVector pos		= _Object->GetPosition();
		sVector size	= _Object->GetSize();
		float angle		= _Object->GetAngle();

		sVector delta = Point - _LastPosition;
		//Вычисление вектора перемещения по нормали к левой стороне
		sVector normal(cos(angle), sin(angle));
		sVector tmp;
		float scalar = (delta.x*normal.x + delta.y*normal.y);
		tmp.x = scalar*normal.x ;  
		tmp.y = scalar*normal.x ; 

		delta = tmp;

		//Если размер становится меньше минимального после перемещния 
		//размеры объекта не изменяем
		if(cos(angle) != 0)
		{
			if((size.x - delta.x/cos(angle)) < _MinSize.x )
				delta.x = 0;
		}
		else
			if((size.x - delta.x) < _MinSize.x)
				delta.x = 0;

		//Расчет размеров и позиции объекта после перемещения мыши
		//Установка новых размера и позиции объекта
		pos.x += delta.x/2;
		if(cos(angle) != 0)
			pos.y += delta.x/2*sin(angle)/cos(angle);
		_Object->MoveTo(pos);

		sVector sizeNew = size;

		if(cos(angle) != 0)
			sizeNew.x -= delta.x/cos(angle);
		else
			sizeNew.x -= delta.x;

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
    
	//Установка кусора в зависимости от угла поворота объекта
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

//Проверка попадает ли вектор point на объект
bool CManipulatorSizeLeft::HitTest(sVector point)
{
	sVector pos	= _Object->GetPosition();
	sVector size	= _Object->GetSize();
	sVector center = size/2;

	//Расчет вершин квадратика посередине левой стороны
	sVector points[4];
	points[0] = sVector(-_BrickSize.x/2 - center.x, -_BrickSize.y/2);
	points[1] = sVector(+_BrickSize.x/2 - center.x, -_BrickSize.y/2);
	points[2] = sVector(+_BrickSize.x/2 - center.x, +_BrickSize.y/2);
	points[3] = sVector(-_BrickSize.x/2 - center.x, +_BrickSize.y/2);
	for(int i=0;i<4;i++)
	{
		points[i].Rotate(_Object->GetAngle());
		points[i] += pos+center;
	}
	//Проверка попадает ли point в квадратик посередине левой стороны
	CGraphic graph(NULL);
	return graph.TestPtInRect(point, points);
}


}