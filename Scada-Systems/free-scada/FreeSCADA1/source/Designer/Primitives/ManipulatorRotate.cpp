#include "stdafx.h"
#include "ObjectMenager.h"
#include ".\manipulatorrotate.h"
#include "resource.h"

namespace Primitives
{

	CManipulatorRotate::CManipulatorRotate(CObject* Obj)
		: CManipulator(Obj)
		, _MLButtonDown(false)
	{
		_Brush.CreateSolidBrush(RGB(0,0,255));
		_Pen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	}

	CManipulatorRotate::~CManipulatorRotate(void)
	{
	}

	//Рисование манипулятора
	void CManipulatorRotate::Draw(HDC dc_)
	{
		ATLASSERT(_Object != NULL);

		CDCHandle dc(dc_);
		CPen pold = dc.SelectPen(_Pen);
		CBrush bold = dc.SelectBrush(_Brush);

		sVector pos	= _Object->GetPosition();
		sVector size	= _Object->GetSize();

		sVector center = pos + size/2;

		sVector point[4]; //Крайние точки объекта
		point[0] = -size/2;
		point[1] = sVector(size.x/2, -(size.y)/2);
		point[2] = size/2;
		point[3] = sVector(-(size.x)/2, size.y/2);

		CObjectMenagerPtr mng;
		for(int i = 0; i<= 3; i++)
		{
			point[i].Rotate(_Object->GetAngle());//Поворачиваем крайние точки
			point[i] += center;
			point[i] = mng->GlobalToScreen(point[i]);

			CPoint pt((int)point[i].x, (int)point[i].y);
			CRect rc(pt.x-5, pt.y-5, pt.x+5, pt.y+5);
			dc.Ellipse(&rc);//Рисуем кружки вокруг крайних точек
		}
		dc.SelectBrush(bold);
		dc.SelectPen(pold);
	}

	//Обработчик нажатия левой кнопки мыши
	BOOL CManipulatorRotate::OnMouseLButtonDown(sVector Point)
	{
		if(HitTest(Point))
		{
			_LastPosition = Point;
			_MLButtonDown = true;
			CObjectMenagerPtr mng;
			mng->CaptureMouse(this); //Захват манипулятора ObjectMenager-ом
									 //Манипулятор будет получать все сообщения
		}
		return TRUE;
	}

	//Обработчик отпускания кнопки мыши
	BOOL CManipulatorRotate::OnMouseLButtonUp(sVector Point)
	{
		_MLButtonDown = false;
		CObjectMenagerPtr mng;
		mng->ReleaseCaptureMouse(); //Освобождение манипулятора ObjectMenager-ом

		_Object->OnEdit(); //Обновляем параметры объекта в окне свойств
		return TRUE;
	}

	//Обработчик перемещения мыши
	BOOL CManipulatorRotate::OnMouseMove(sVector Point)
	{
		if(_MLButtonDown)
		{
			sVector sz = _Object->GetSize();
			sVector pos = _Object->GetPosition();

			sVector center = pos + sz/2;

			_LastPosition -= center;
			Point -= center;

			float angle = AngleBetweenVectorEx(_LastPosition, Point);

			_Object->RotateBy(angle); //Поворачиваем объект
			_LastPosition = Point + center;
			_Object->OnEdit();

		}

		::SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
		return TRUE;
	}

	//Проверка попадает ли point на объект (на угловые точки объекта)
	bool CManipulatorRotate::HitTest(sVector Point)
	{
		CPoint _point((int)Point.x, (int)Point.y);
		sVector pos	= _Object->GetPosition();
		sVector size	= _Object->GetSize();
		sVector center = pos + size/2;

		sVector point[4];

		point[0] = -size/2;
		point[1] = sVector(size.x/2, -size.y/2);
		point[2] = size/2;
		point[3] = sVector(-size.x/2, size.y/2);

		for(int i = 0; i<= 3; i++)
		{
			point[i].Rotate(_Object->GetAngle());
			point[i] += center;
			CPoint pt((int)point[i].x, (int)point[i].y);
			CRect rc(pt.x-5, pt.y-5, pt.x+5, pt.y+5);
			if(rc.PtInRect(_point))return TRUE;
		}

		return FALSE;
	}

}
