#include "stdafx.h"
#include "ObjectMenager.h"
#include "ObjPolyline.h"
#include ".\manipulatorvertex.h"

namespace Primitives
{
	CManipulatorVertex::CManipulatorVertex(CObject* Obj)
		: CManipulator(Obj)
		, _MLButtonDown(false)
	{
		_BrickSize.x = MOVE_BLOCK_SIZE;
		_BrickSize.y = MOVE_BLOCK_SIZE;

		_Brush.CreateSolidBrush(RGB(255,0,0));
		_Pen.CreatePen(PS_SOLID, 1, RGB(255,255,255));
		_CurrentVertex = static_cast<CObjPolyline*>(Obj)->GetVertexes().end();
	}

	CManipulatorVertex::~CManipulatorVertex(void)
	{
	}

	//Рисовние манипулятора
	void CManipulatorVertex::Draw(HDC dc_)
	{
		ATLASSERT(_Object != NULL);

		CObjPolyline* poly = (CObjPolyline*)_Object;
		CDCHandle dc(dc_);
		CPen pold = dc.SelectPen(_Pen);
		CBrush bold = dc.SelectBrush(_Brush);

		CRect rc;
		//Получаем ссылку на вектор с координатами вершин (в относительных координатах)
		vector< sVector >& Vertexes = poly->GetVertexes();
		if (Vertexes.size() > 0) 
		{
			typedef vector< sVector >::iterator Iter;

			sVector pos	= _Object->GetPosition();
			sVector size	= _Object->GetSize();
			sVector center = pos + size/2;

			CObjectMenagerPtr mng;
			for(Iter it = Vertexes.begin(); it != Vertexes.end(); it++)
			{
				//Преобразуем из относительных в абсолютные координаты с учетом угла
				sVector point = (*it)*size - size/2;
				point.Rotate(_Object->GetAngle());
				point += center;
				point = mng->GlobalToScreen(point);

				rc.left		= (LONG)(point.x - _BrickSize.x/2.0f);
				rc.right	= (LONG)(point.x + _BrickSize.x/2.0f);
				rc.top		= (LONG)(point.y - _BrickSize.y/2.0f);
				rc.bottom	= (LONG)(point.y + _BrickSize.y/2.0f);
				//Рисуем квадратики на вершинах
				dc.Rectangle(&rc);
			}
		}
		dc.SelectBrush(bold);
		dc.SelectPen(pold);
	}

	//Обработчик сообщения нажатия левой кнопки мыши
	BOOL CManipulatorVertex::OnMouseLButtonDown(sVector Point)
	{
		if(HitTest(Point))
		{
			_LastPosition = Point;
			_MLButtonDown = true;
			CObjectMenagerPtr mng;
			//Захват полилинии ObjectMenager-ом
			//Все сообщения будут приходить к манипулятору
			mng->CaptureMouse(this);

			//Если при нажатии на кнопку мыши указатель мыши не попал 
			//на одну из вершин
			CObjPolyline* poly = static_cast<CObjPolyline*>(_Object);
			if(_CurrentVertex == poly->GetVertexes().end())
				//Если указаталь мыши попал на линию
				//_CurrentVertex - iterator указывающий на вершину перед которой попал 
				//указатель мыши
				if(HitLine(Point, _CurrentVertex))
				{					
					vector< sVector >& Vertexes = poly->GetVertexes();
					if (!Vertexes.empty())
					{
						sVector size = _Object->GetSize();
						sVector pos = _Object->GetPosition();
						sVector center = pos + size/2;

						//Преобразуем координаты точки Point из абсолютных координат
						//в относительные
						sVector pt = Point - center;
						pt.Rotate(-_Object->GetAngle());
						pt = (pt+size/2)/size;

						//Добавляем новую точку в вектор вершин
						vector< sVector >::iterator tmp = 
							Vertexes.insert(_CurrentVertex, pt);

						_CurrentVertex = tmp;
					}
				}
		}

		return TRUE;
	}

	//Обработчик нажатия правой кнопки мыши
	BOOL CManipulatorVertex::OnMouseRButtonDown(sVector Point)
	{
		//Если нажали на одной из вершин
		if(HitTest(Point))
		{
			if(_CurrentVertex == static_cast<CObjPolyline*>(_Object)->GetVertexes().end()) 
				return TRUE;
			vector< sVector >& Vertexes = ((CObjPolyline*)_Object)->GetVertexes();
			if(Vertexes.size() < 2) return TRUE;
			if(_CurrentVertex == Vertexes.begin() || _CurrentVertex == Vertexes.end()-1) 
				return TRUE;
			//Удаляем вершину если это не крайняя вершина
			Vertexes.erase(_CurrentVertex);
		}
		return TRUE;
	}

	//Обработчик отпускания левой кнопки мыши
	BOOL CManipulatorVertex::OnMouseLButtonUp(sVector Point)
	{
		_CurrentVertex = static_cast<CObjPolyline*>(_Object)->GetVertexes().end();
		_MLButtonDown = false;
		CObjectMenagerPtr mng;
		//Освобождение полилинии ObjectMenager-ом
		mng->ReleaseCaptureMouse();
		return TRUE;
	}

	//Обработчик перемещения мыши
	BOOL CManipulatorVertex::OnMouseMove(sVector Point)
	{
		//если захвачена какая-нибудь вершина
		if((_MLButtonDown) && (_CurrentVertex != static_cast<CObjPolyline*>(_Object)->GetVertexes().end()))
		{
			sVector size = _Object->GetSize();
			sVector pos = _Object->GetPosition();
			sVector center = pos + size/2;

			//Преобразуем из абсолютных координат в относительные и 
			//присваиваем текущей вершине новые координаты
			sVector pt = Point - center;
			pt.Rotate(-_Object->GetAngle());
			*_CurrentVertex = (pt + size/2)/size;

		}
		return TRUE;
	}

	//Проверка, попадает ли point на одну из вершин полилинии
	bool CManipulatorVertex::HitTest(sVector point)
	{
		CObjPolyline* poly = (CObjPolyline*)_Object;
		CRect rc;
		CPoint pPoint((int)point.x, (int)point.y);

		vector< sVector >& _Vertex = poly->GetVertexes();
		if (!_Vertex.empty()) 
		{
			typedef vector< sVector >::iterator Iter;

			sVector pos	= _Object->GetPosition();
			sVector size	= _Object->GetSize();
			sVector center = pos + size/2;

			for(Iter it = _Vertex.begin(); it != _Vertex.end(); it++)
			{
				//переводим координаты точки из относительных координат в абсолютные
				sVector pt = (*it)*size - size/2;
				pt.Rotate(_Object->GetAngle());
				pt += center;

				//рассчитываем квадратик возле вершины
				rc.left		= (LONG)(pt.x - _BrickSize.x/2.0f);
				rc.right	= (LONG)(pt.x + _BrickSize.x/2.0f);
				rc.top		= (LONG)(pt.y - _BrickSize.y/2.0f);
				rc.bottom	= (LONG)(pt.y + _BrickSize.y/2.0f);

				//Если point попадает на квадратик возле вершины 
				//текущая вершина (_CurrentVertex) становится этой вершиной
				if (rc.PtInRect(pPoint)) 
				{
					_CurrentVertex = it;
					return TRUE;
				}
				else 
					_CurrentVertex = poly->GetVertexes().end();
			}
		}
		vector< sVector >::iterator tmp;
		if(HitLine(point, tmp))
			return TRUE;
		return FALSE;
	}

	//Проверка попадает ли point на один из отрезков полилинии
	bool CManipulatorVertex::HitLine(sVector point, vector< sVector >::iterator& vertex)
	{
		CObjPolyline* poly = (CObjPolyline*)_Object;

		vector< sVector >& _Vertex = poly->GetVertexes();
		if (!_Vertex.empty()) 
		{
			typedef vector< sVector >::iterator Iter;

			sVector pos	= _Object->GetPosition();
			sVector size	= _Object->GetSize();
			sVector center = pos + size/2;

			sVector lastPt, firstPt, newPt;


			Iter it = _Vertex.begin();

			//преобразуем вершину из относительных координат в абсолютные
			lastPt = (*it)*size - size/2;
			lastPt.Rotate(_Object->GetAngle());
			lastPt += center;

			it++;

			while( it != _Vertex.end())
			{
				//преобразуем вершину из относительных координат в абсолютные
				firstPt = (*it)*size - size/2;
				firstPt.Rotate(_Object->GetAngle());
				firstPt += center;

				//Если отрезок полилинии вертикальный
				if(firstPt.x == lastPt.x)
				{
					float Min = min(firstPt.y ,lastPt.y);
					float Max = max(firstPt.y ,lastPt.y);

					if((point.y < Max) && (point.y > Min))
					{
						//Если указатель мыши попал между двух вершин
						//и находится на расстоянии не более BrickSize.x от линии
						if((firstPt.x - point.x)<_BrickSize.x)
						{
							vertex = it;
							return TRUE;
						}
					}

					lastPt = firstPt;
					it++;
					continue;
				}

				//Если отрезок полилинии горизонтальный
				if(firstPt.y == lastPt.y)
				{
					float Min = min(firstPt.x ,lastPt.x);
					float Max = max(firstPt.x ,lastPt.x);

					if((point.x < Max) && (point.x > Min))
					{
						//Если указатель мыши попал между двух вершин
						//и находится на расстоянии не более BrickSize.x от линии
						if((firstPt.y - point.y)<_BrickSize.x)
						{
							vertex = it;
							return TRUE;
						}

						lastPt = firstPt;
						it++;
						continue;
					}

				}
				//Определяем точку(newPt) лежащую на пересечении отрезка полилинии
				//и нормали опущенной из точки point на этот отрезок
				float X1 = firstPt.x;
				float Y1 = firstPt.y;
				float X2 = lastPt.x;
				float Y2 = lastPt.y;
				float X3 = point.x;
				float Y3 = point.y;
				float K = (Y1-Y2)/(X1-X2);

				newPt.x = (K*X1 + X3/K - Y1 + Y3)/(K + 1/K);
				newPt.y = K*(newPt.x - X1) + Y1;

				float MinX = min(firstPt.x ,lastPt.x);
				float MaxX = max(firstPt.x ,lastPt.x);
				float MinY = min(firstPt.y ,lastPt.y);
				float MaxY = max(firstPt.y ,lastPt.y);

				//Если newPt не попадает на отрезок полилинии, продолжаем итерацию
				if((newPt.x > MaxX) || (newPt.x < MinX) || 
					(newPt.y > MaxY) || (newPt.y < MinY))
				{
					lastPt = firstPt;
					it++;
					continue;
				}

				//Определяем расстояние по нормали от точки point до отрезка
				float delta = sqrt(pow(newPt.x - point.x,2) + pow(newPt.y - point.y,2));

				//если расстояние меньше _BrickSize.x/2 возвращаем iterator,
				//указывающий на вершину перед которой можно было бы вставить
				//вершину point
				if(delta < _BrickSize.x/2)
				{
					vertex = it;
					return TRUE;
				}

				lastPt = firstPt;
				it++;
			}
		}
		vertex = poly->GetVertexes().end();
		return FALSE;
	}
}