#include "common.h"
#include "ObjectMenager.h"
#include "Graphic.h"
#include "selection.h"


CSelection::CSelection(void)
{
	_MLButtonDown = false;
	//CSelection рисуется пунктирной линией
	_Pen.CreatePen(PS_DOT, 1, RGB(0,0,0));
	_AutoDelete = false; //Объект не удаляется автоматически при вызове функции
						//CObjectMenager::RemoveObject или CObjectMenager::RemoveAndDeleteAll
	SetTypeID(_SELECTION);
	_OrderPos = POS_OLD;
}

CSelection::~CSelection(void)
{
}

//Рисование CSelection и объектов входящих в него
void CSelection::Draw(HDC dc_)
{
	sVector pos = GetPosition();
	sVector size = GetSize();
	sVector center = pos + size/2;

	CDCHandle dc(dc_);
	CPen oldPen = dc.SelectPen(_Pen);
	
	//Крайние точки объекта CSelection
	sVector points[4];
	points[0] = sVector(pos);
	points[1] = sVector((pos+size).x, pos.y);
	points[2] = sVector(pos+size);
	points[3] = sVector(pos.x, (pos+size).y);

	//Расчет крайних точек с учетом угла поворота
	CObjectMenagerPtr om;
	for(int i = 0; i <= 3; i++)
	{
		points[i] -= center;
		float angle = GetAngle() + atan2(points[i].y, points[i].x);
		float s = points[i].Length();
		points[i].x = s*cos(angle);
		points[i].y = s*sin(angle);
		points[i] += center;
		points[i] = om->GlobalToScreen(points[i]);
	}

	//Рисуем пунктирную рамку
	dc.MoveTo((int)points[3].x, (int)points[3].y);
	for(int i = 0; i <= 3; i++)
		dc.LineTo((int)points[i].x, (int)points[i].y);


	dc.SelectPen(oldPen);
//Рисуем объекта входящие в CSelection 
	for(list<CObject*>::reverse_iterator it = _Objects.rbegin();
		it != _Objects.rend(); it++)
	{
		//Переводим относительные координаты объектов в абсолютные
		//(Позицию, размер и поворачиваем на угол)
		RelateToAbsolute((*it));
		
		//Координата центра объекта
		sVector ptc = (*it)->GetPosition() + (*it)->GetSize()/2;
		sVector ptc1 = ptc;
		//Координата центр объекта относительно центра CSelection
		ptc -= center;
		//Угол расположения центра объекта
		float angle = atan2(ptc.y, ptc.x);
		//Угол расположения центра объекта с учетом поворота CSelection
		angle += GetAngle(); 

		//Координаты центра объекта относительно центра CSelection
		float s = ptc.Length();
		ptc.x = s*cos(angle);
		ptc.y = s*sin(angle);
		//Абсолютные координаты центра объекта
		ptc += center;

		//Переносим объект в новое место с учетом расположения и угла CSelection
		(*it)->MoveBy(ptc - ptc1);
		//Рискем объект
		(*it)->Draw(dc);
		//Возвращаем объект обратно
		(*it)->MoveBy(ptc1 - ptc);

		//Переводим координаты объекта из абсолютных в относительные
		AbsoluteToRelate((*it));
	}
//	for_each(_Manipulators.begin(), _Manipulators.end(), bind2nd(mem_fun(&CObject::Draw), dc));
}



//Очистка списка _Object
void CSelection::ClearObject()
{
	if( !_Objects.empty())
	{
		_Objects.clear();
	}

	MoveTo(0,0);
	SetSize(0,0);
	Rotate(0);
}

//Добавление объекта в CSelection
void CSelection::AddObject(CObject* obj)
{
	ATLASSERT(obj != NULL);
//Если добавляем первый объект, размеры и позицию CSelection
//определяем по крайним точкам этого объекта
	if(_Objects.empty())
	{
		SetSize(obj->GetRBBound() - obj->GetLTBound());
		MoveTo(obj->GetLTBound());
		AbsoluteToRelate(obj);
		_Objects.push_back(obj);
		_Objects.sort(sCompOrderPos());
		return;
	}

	sVector pos = GetPosition();
	sVector size = GetSize();
//Переводим новый объект в относительные координаты и помещаем
//этот объект в массив
	AbsoluteToRelate(obj);
	_Objects.push_back(obj);
	CalcSizeEndPos();	

	
}


//Расчет размеров и позиции CSelection
void CSelection::CalcSizeEndPos()
{
	list< CObject* >::iterator it = _Objects.begin();
//	CObject* ooo = *it;

	sVector pos = (*it)->GetPosition()*GetSize() + GetPosition();
	sVector size = (*it)->GetSize()*GetSize();

	list< CObject* > tmp;

	while(it != _Objects.end())
	{
		//Переводим позицию и размер объектов в CSelection в абсолютные координаты	
		sVector ipos = (*it)->GetPosition()*GetSize() + GetPosition();
		sVector isize;

		isize.x = (*it)->GetSize().x*GetSize().x;
		isize.y = (*it)->GetSize().y*GetSize().y;

		(*it)->SetSize(isize);
		(*it)->MoveTo(ipos);
		tmp.push_back(*it);
		sVector LT = (*it)->GetLTBound();
		sVector RB = (*it)->GetRBBound();

		//Расчитываем новые размер и позицию CSelection
		if((*it)->GetLTBound().x < pos.x)
		{
			size.x += pos.x - (*it)->GetLTBound().x;
			pos.x = (*it)->GetLTBound().x;
		}
		if((*it)->GetLTBound().y < pos.y)
		{
			size.y += pos.y - (*it)->GetLTBound().y;
			pos.y = (*it)->GetLTBound().y;
		}

		//	int sx = (*it)->GetPosition().x + (*it)->GetSize().x;

		if((*it)->GetRBBound().x > pos.x + size.x)
		{
			size.x = (*it)->GetRBBound().x - pos.x;
		}

		//	int sy = (*it)->GetPosition().y + (*it)->GetSize().y ;

		if((*it)->GetRBBound().y > pos.y + size.y)
		{
			size.y = (*it)->GetRBBound().y - pos.y;
		}
		it++;
	}
	_Objects.clear();

	//Пересчитываем размеры и позиции объектов в относительные координаты

	for(list< CObject* >::iterator i = tmp.begin();
		i != tmp.end(); i++)
	{
		(*i)->SetSize((*i)->GetSize().x/size.x, (*i)->GetSize().y/size.y);
		(*i)->MoveTo(((*i)->GetPosition() - pos).x/size.x, 
			((*i)->GetPosition() - pos).y/size.y);
		_Objects.push_back(*i);
	}

	MoveTo(pos);
	SetSize(size);

}

//Перевод размера и позиции объекта в абсолютные координаты
//Поворот объекта на угол поворота CSelection
void CSelection::RelateToAbsolute(CObject* obj)
{
	obj->MoveTo(obj->GetPosition()*GetSize() + GetPosition());
	obj->SetSize(obj->GetSize()*GetSize());
	obj->RotateBy(GetAngle());
}

//Перевод размера и позиции объекта в относительные координаты
//Установка угла поворота объекта без учета поворота CSelection
void CSelection::AbsoluteToRelate(CObject* obj)
{
	obj->MoveTo((obj->GetPosition() - GetPosition()).x/GetSize().x,
		(obj->GetPosition() - GetPosition()).y/GetSize().y);
	obj->SetSize(obj->GetSize().x/GetSize().x,
		obj->GetSize().y/GetSize().y);
	obj->RotateBy(-GetAngle());
}

//Перевод размера и позиции объекта в абсолютные координаты
//с учетом смещения объекта, вызванного поворотом CSelection
void CSelection::Convert(CObject* obj)
{
	sVector center = GetPosition() + GetSize()/2;
	RelateToAbsolute(obj);

	sVector ptc = obj->GetPosition() + obj->GetSize()/2;
	sVector ptc1 = ptc;
	ptc -= center;

	float angle = atan2(ptc.y, ptc.x);

	angle += GetAngle(); 

	float s = ptc.Length();
	ptc.x = s*cos(angle);
	ptc.y = s*sin(angle);
	ptc += center;
	obj->MoveBy(ptc - ptc1);
}


//Получить список всех объектов
TStrings CSelection::GetObjects()
{
	TStrings Names;
	if(!_Objects.empty())
	{
		for(list<CObject*>::iterator it = _Objects.begin(); it != _Objects.end(); it++)
		{
			Names.push_back((*it)->GetName());
		}
	}
	return Names;
}

//Найти объект по имени
CObject* CSelection::FindObject(wstring Name)
{
	list<CObject*>::iterator it = 
		find_if(_Objects.begin(), _Objects.end(), sFindByName(Name));
	
	if(it != _Objects.end())
	{
		return *it;
	}
	return NULL;
}




