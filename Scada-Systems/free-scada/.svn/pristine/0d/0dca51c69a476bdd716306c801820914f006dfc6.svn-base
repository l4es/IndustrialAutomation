#include "common.h"
#include "MemFile.h"
#include "ObjectMenager.h"
//#include "ManipulatorVertex.h"
#include ".\objpolyline.h"
#include "Action.h"

CObjPolyline::CObjPolyline(void)
{
	SetColor(0,0,0);
	MoveTo(0,0);
	SetName(GenerateUniqueObjectName(L"Polyline_"));
	SetTypeID(_POLYLINE);
	SetActionFlags(AT_ALL_VISUAL);
}

CObjPolyline::~CObjPolyline(void)
{

}

//Добавление точки к полилинии
void CObjPolyline::AddVertex(sVector vertex)
{
	sVector delta1(0,0);

	//если добавляем первую точку то устанавливаем позицию полилинии равной
	// этой точке. Относительные координаты точки устанавливаем 0,0
	if(_Vertex.size() == 0)
	{
		_Vertex.push_back(delta1);
		CObject::MoveTo(vertex);
		return;
	}

	sVector pos = GetPosition();
	sVector size = GetSize();
//расчет размера если добавили вторую точку
	if(_Vertex.size() == 1)
	{
		size.x = abs(vertex.x - pos.x);
		size.y = abs(pos.y - vertex.y);
	}
//расчет нового размера если добавляем 3-ю или более точку
	else
	{
		if(vertex.x > pos.x + GetSize().x) size.x = abs(pos.x - vertex.x); 
		if(vertex.x < pos.x) size.x = abs(pos.x - vertex.x) + GetSize().x; 
		if(vertex.y > pos.y + GetSize().y) size.y = abs(pos.y - vertex.y); 
		if(vertex.y < pos.y) size.y = abs(pos.y - vertex.y) + GetSize().y; 
	}
//расчет новой позиции (если новая точка левее или выше)
	if(vertex.x < pos.x) 
	{
		delta1.x = pos.x - vertex.x;
		pos.x = vertex.x;
	}

	if(vertex.y < pos.y) 
	{
		delta1.y = pos.y - vertex.y;
		pos.y = vertex.y;
	}

//расчет относительных координат новой точки и
//пересчет относительных координат всех точек с 
//учетом изменения размера и позиции (из-за добавления новой точки)
	if((size.x != 0) && (size.y != 0))
	{
		vertex.x = (vertex.x - pos.x)/size.x; 
		vertex.y = (vertex.y - pos.y)/size.y; 
		for(Iter it= _Vertex.begin(); it != _Vertex.end(); it++)
		{
			it->x = (it->x*GetSize().x + delta1.x)/size.x;
			it->y = (it->y*GetSize().y + delta1.y)/size.y;
		}
	}

//добавляем новую точку и устанавливаем новые размер и позицию
	_Vertex.push_back(vertex);

	CObject::MoveTo(pos);
	CObject::SetSize(size);
}

//Добавление точки к полилинии
void CObjPolyline::AddVertex(float x, float y)
{
	sVector vertex(x,y);
	AddVertex(vertex);
}

//Сохранение - восстановление полилинии
void CObjPolyline::Serialize(CMemFile& file, BOOL IsLoading)
{
	CObject::Serialize(file, IsLoading);

	size_t count;
	if(IsLoading)
	{
		file.ReadBlock(&count, sizeof(size_t));//Считываем количество точек
		_Vertex.resize(count);			
		file.ReadBlock(&_Vertex[0],  (UINT)sizeof(sVector)*count);//Считываем точки
	}
	else
	{
		count = _Vertex.size();
		file.WriteBlock(&count, sizeof(size_t)); //Записываем количество точек
		file.WriteBlock(&_Vertex[0],  (UINT)sizeof(sVector)*count); //Записываем точки
	}
	
}


//рисование полилинии
void CObjPolyline::Draw(HDC dc_)
{
	if(!IsVisible())
		return;

	CDCHandle dc(dc_);
	
	CObjectMenagerPtr mng;
	CPen _Pen;
	_Pen.CreatePen(PS_SOLID, 1, GetColor().AsCOLORREF());
	CPenHandle pold = dc.SelectPen(_Pen);

	if(!_Vertex.empty())
	{
		CPoint pt;
		sVector vpt;
		
		sVector pos	 = GetPosition();
		sVector size = GetSize();
		sVector center = pos + size/2;

//	Расчет координат первой точки в абсолютных координатах
// с учетом угла поворота
		Iter it = _Vertex.begin();
		vpt = *it;
		vpt = *it*size + pos;

		float angle = atan2((vpt.y-center.y), (vpt.x-center.x));
		float Vector = sqrt(pow((vpt-center).y,2)+pow((vpt-center).x,2));
		angle += GetAngle();
		vpt = sVector(Vector*cos(angle), Vector*sin(angle))+center;

		pt = mng->GlobalToScreen(vpt);

//устанавливаем начало первой линии
		dc.MoveTo(pt);
		it++;

		while(it != _Vertex.end())
		{
//пересчет всех точек полилинии в абсолютные координаты с учетом угла
//и рисование линий
			vpt.x = it->x*size.x + pos.x;
			vpt.y = it->y*size.y + pos.y;

			angle = atan2((vpt.y-center.y), (vpt.x-center.x));
			Vector = sqrt(pow((vpt-center).y,2)+pow((vpt-center).x,2));
			angle += GetAngle();
			vpt = sVector(Vector*cos(angle), Vector*sin(angle))+center;
		
			pt = mng->GlobalToScreen(vpt);
			dc.LineTo(pt);
            it++;
		}
	}
	dc.SelectPen(pold);
}

//Получение абсолютных координат точек с учетом угла
vector<sVector> CObjPolyline::GetVertexesGlobal()
{
	vector<sVector> res;

	if(!_Vertex.empty())
	{
		CPoint pt;
		sVector vpt;

		sVector pos	 = GetPosition();
		sVector size = GetSize();
		sVector center = pos + size/2;

		
		for(Iter it = _Vertex.begin(); it != _Vertex.end(); it++)
		{
			//расчет абс. координат без учета угла
			vpt.x = it->x*size.x + pos.x;
			vpt.y = it->y*size.y + pos.y;
			//расчет угла относительно центра полилинии
			float angle = atan2((vpt.y-center.y), (vpt.x-center.x));
			float Vector = sqrt(pow((vpt-center).y,2)+pow((vpt-center).x,2));
			angle += GetAngle();
			//расчет с учетом угла
			vpt = sVector(Vector*cos(angle), Vector*sin(angle))+center;

			res.push_back(vpt);
		}
	}
	return res;
}

BOOL CObjPolyline::OnMouseLButtonDblClk(sVector Point)
{
	
	return FALSE;
}

//Пересчет расположения и размера полилинии
void CObjPolyline::CalcSizeAndPos()
{
	if(!_Vertex.empty())
	{
		sVector size = GetSize();
		sVector pos = GetPosition();
		sVector center = pos + size/2;
		sVector newSize;
		sVector newPos;
		sVector newRightBottom;

//Создаем вектор для абсолютных координат точек полилинии
		list< sVector > tmp;
		for(vector< sVector >::iterator it = _Vertex.begin();
			it != _Vertex.end(); it++)
		{
			//Расчитываем абсолютные координаты точек полилинии с учетом угла
			//Помещаем их в вектор tmp

			sVector pt;
			pt.x = it->x*size.x + pos.x;
			pt.y = it->y*size.y + pos.y;

			float angle = atan2((pt-center).y, (pt-center).x);
			float Vector = sqrt(pow((pt-center).y,2)+pow((pt-center).x,2));
			angle += GetAngle();
			pt = sVector(Vector*cos(angle), Vector*sin(angle))+center;

			if(it == _Vertex.begin())
			{ 
				newPos = pt; 
				newRightBottom = pt;
			}
			if(pt.x < newPos.x)newPos.x = pt.x;
			if(pt.y < newPos.y)newPos.y = pt.y;
			if(pt.x > newRightBottom.x)newRightBottom.x = pt.x;
			if(pt.y > newRightBottom.y)newRightBottom.y = pt.y;
			tmp.push_back(pt);
		}
//Задаем новые размер и позицию полилинии
		MoveTo(newPos);
		SetSize(newRightBottom - newPos);
		Rotate(0);
		size = GetSize();
		pos = GetPosition();
		center = pos + size/2;
		_Vertex.clear();
//Помещаем в вектор вершин относительные координаты точек
		for(list< sVector >::iterator it = tmp.begin();
			it != tmp.end(); it++)
		{
			sVector pt, Point = *it;
			float Vector = sqrt(pow((Point-center).y,2)+pow((Point-center).x,2));
			float angle = atan2((Point-center).y, (Point-center).x);
			angle -= GetAngle();
			Point = sVector(Vector*cos(angle), Vector*sin(angle))+center;

			if(size.x == 0)
				pt.x = 0;
			else
				pt.x = (Point.x - pos.x)/size.x;

			if(size.y == 0)
				pt.y = 0;
			else
				pt.y = (Point.y- pos.y)/size.y;
			_Vertex.push_back(pt);
		}
	}

}


BOOL CObjPolyline::OnKeyDown(WPARAM wParam, sVector Point)
{
	return FALSE;
}

void CObjPolyline::OnLostFocus()
{
	CalcSizeAndPos();
	CObject::OnLostFocus();
}

void CObjPolyline::OnSetFocus()
{
	CObject::OnSetFocus();
}