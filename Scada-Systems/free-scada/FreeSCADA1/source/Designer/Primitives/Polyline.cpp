#include "StdAfx.h"
#include ".\polyline.h"
#include "ManipulatorVertex.h"

namespace Primitives
{
	CPolyline::CPolyline(void)
	{
		_EditVertex = FALSE;
	}

	CPolyline::~CPolyline(void)
	{
	}

	//Обработчик нажатия левой кнопки мыши
	BOOL CPolyline::OnMouseLButtonDown(sVector Point)
	{
		return CBaseObject<CPolyline>::OnMouseLButtonDown(Point);
	}

	//Обработчик нажатия правой кнопки мыши
	BOOL CPolyline::OnMouseRButtonDown(sVector Point)
	{
		return CBaseObject<CPolyline>::OnMouseRButtonDown(Point);
	}

	//Обработчик отпускания левой кнопки мыши
	BOOL CPolyline::OnMouseLButtonUp(sVector Point)
	{
		return CBaseObject<CPolyline>::OnMouseLButtonUp(Point);
	}

	//Обработчик перемещения мыши
	BOOL CPolyline::OnMouseMove(sVector Point)
	{
		return CBaseObject<CPolyline>::OnMouseMove(Point);
	}

	//Обработчик двойного нажатия левой кнопки мыши
	BOOL CPolyline::OnMouseLButtonDblClk(sVector Point)
	{
		CObjectMenagerPtr om;
		if(!om->IsSelected(this)) return FALSE;

		//Удаляем все манипуляторы
		OnLostFocus();

		//Создаем манипулятор точек полилинии
		CManipulatorVertex	*vertex = new CManipulatorVertex(this);

		_Manipulators.push_front(vertex);
		_EditVertex = TRUE;
		return CBaseObject<CPolyline>::OnMouseLButtonDblClk(Point);
	}

	//Обработчик нажатия клавиши клавиатуры
	BOOL CPolyline::OnKeyDown(WPARAM wParam, LPARAM lParam)
	{
		UINT key = (UINT)wParam;
		// Если нажали Esc выходим из режима редактирования точек
		if (key == 27)
		{
			//	_EditVertex = FALSE;

			OnLostFocus();
			OnSetFocus();
		}
		return CBaseObject<CPolyline>::OnKeyDown(wParam, lParam);
	}

	//Обработчик получения фокуса объектом
	void CPolyline::OnSetFocus()
	{
		CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
		//Устанавлиаем в окне свойств параметры этого объекта 
		prop->SetObject(this);
		prop->FreeInst();
	//	CObjPolyline::OnSetFocus();
		return CBaseObject<CPolyline>::OnSetFocus();
		//Создаем манипулятор точек полилинии
		//CManipulatorVertex	*vertex = new CManipulatorVertex(GetObject());

		//_Manipulators.push_front(vertex);
		//_EditVertex = TRUE;
	}

	//Обработчик потери фокуса объектом
	void CPolyline::OnLostFocus()
	{
		CObjPolyline::OnLostFocus();
		CBaseObject<CPolyline>::OnLostFocus();
	}

	//Обработчик изменения объекта
	void CPolyline::OnEdit()
	{
		CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
		//Обновляем параметры в окне свойств
		prop->RefreshProperty();
		prop->FreeInst();

		return CObjPolyline::OnEdit();
	}

	//Рисование полилинии
	void CPolyline::Draw(HDC dc) 
	{
		const bool Visible = IsVisible();
		//В дизайнере объект всегда видимый
		SetVisible(true);

		CObjPolyline::Draw(dc);
		//Рисуем все манипуляторы которые в данный момент имеются в объекте
		for_each(_Manipulators.begin(),_Manipulators.end(), bind2nd(mem_fun(&CObject::Draw), dc));
		//Восстанавливаем исходное значение видимости
		SetVisible(Visible);
	}

	//Проверка попадает ли point на объект
	bool CPolyline::HitTest(sVector point)
	{
		if(CBaseObject<CPolyline>::HitTest(point))
			return true;

		sVector pos		= GetObject()->GetPosition();
		sVector size	= GetObject()->GetSize();

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
			points[i].Rotate(GetObject()->GetAngle());
			points[i] += pos+center;
		}

		CGraphic graph(NULL);
		return graph.TestPtInRect(point, points);
	}
	
}
