#include "stdafx.h"
#include "ObjectMenager.h"
#include ".\manipulatormover.h"
#include "Graphic.h"
namespace Primitives
{

	CManipulatorMover::CManipulatorMover(CObject* Obj)
		: CManipulator(Obj)
		, _MLButtonDown(false)
	{
	}

	CManipulatorMover::~CManipulatorMover(void)
	{
	}

	
	void CManipulatorMover::Draw(HDC dc_)
	{
	}

	//Обрботчик нажатия левой кнопки мыши
	BOOL CManipulatorMover::OnMouseLButtonDown(sVector Point)
	{
		_LastPosition = Point;
		_MLButtonDown = true;
		CObjectMenagerPtr mng;
		//Захват манипулятора ObjectMenager-ом 
		//Манипулятор будет получать все сообщения
		mng->CaptureMouse(this);

		return TRUE;
	}

	//Обработчик отпукания левой кнопки мыши
	BOOL CManipulatorMover::OnMouseLButtonUp(sVector Point)
	{
		_MLButtonDown = false;
		CObjectMenagerPtr mng;
		//Освобождение манипулятора ObjectMenager-ом
		mng->ReleaseCaptureMouse();

		return TRUE;
	}

	//Обработчик перемещения мыши
	BOOL CManipulatorMover::OnMouseMove(sVector Point)
	{
		if(_MLButtonDown)
		{
			sVector Delta;
			Delta = Point - _LastPosition; //Вектор перемещения
			sVector sz = _Object->GetSize();
			sVector pos = _Object->GetPosition();
			//Если выходим за границы графической области - не перемещаем
			if(pos.x + Delta.x < 0)
			{
				Delta.x = -pos.x;
				_LastPosition.x += Delta.x;
			}
			else
				_LastPosition.x = Point.x;

			if(pos.y + Delta.y < 0)
			{
				Delta.y = -pos.y;
				_LastPosition.y += Delta.y;
			}
			else
				_LastPosition.y = Point.y;

			pos += Delta;
			_Object->MoveTo(pos); //Задаем объекту новую позицию
			_Object->OnEdit(); //Обновляем парамерты с окне свойств
		}

		::SetCursor(LoadCursor(NULL, IDC_SIZEALL));
		return TRUE;
	}

	//Проверка попадает ли point на объект
	bool CManipulatorMover::HitTest(sVector point)
	{
		sVector pos		= _Object->GetPosition();
		sVector size	= _Object->GetSize();
		
		sVector center;
		if(size.x < MOVE_BLOCK_SIZE) 
			center.x = MOVE_BLOCK_SIZE/2;
		else
			center.x = size.x/2;

		if(size.y < MOVE_BLOCK_SIZE) 
			center.y = MOVE_BLOCK_SIZE/2;
		else
			center.y = size.y/2;

		sVector points[4];
		points[0] = sVector(-center.x, -center.y);
		points[1] = sVector(+center.x, -center.y);
		points[2] = sVector(+center.x, +center.y);
		points[3] = sVector(-center.x, +center.y);

		for(int i=0;i<4;i++)
		{
			points[i].Rotate(_Object->GetAngle());
			points[i] += pos+center;
		}

		CGraphic graph(NULL);
		return graph.TestPtInRect(point, points);
	}

}