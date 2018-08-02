#include "common.h"
#include "ObjectMenager.h"
#include "graphic.h"
#include "ObjEllipse.h"
#include "Action.h"


CObjEllipse::CObjEllipse(void)
{
	//Создание "карандаша"
	_DrawBorder = false;
	_BorderColor = sRGB(0,0,0);

	_Pen.CreatePen(PS_SOLID, 1, GetColor().AsCOLORREF());
	//Генерируем уникальное имя для объекта
	SetName(GenerateUniqueObjectName(L"Ellipse_"));
	SetTypeID(_ELLIPSE);
	SetActionFlags(AT_ALL_VISUAL);
}

CObjEllipse::~CObjEllipse(void)
{
	//Удаляем "карандаш"
	_Pen.DeleteObject();
}

void CObjEllipse::Draw(HDC dc_)
{
	if(!IsVisible())
		return;

	CDCHandle dc(dc_);

	//Получить инстанцию менеджера объектов
	CObjectMenagerPtr mng;

	sVector pos = GetPosition();
	sVector size = GetSize();
	float a = size.x/2;
	float b = size.y/2;
	sVector center = pos + size/2;
	

	

	//Создание и выбор "Кисти"
	CBrush Brush;
	Brush.CreateSolidBrush(GetColor().AsCOLORREF());

	_Pen.DeleteObject();
	if(_DrawBorder)
	{
		_Pen.CreatePen(PS_SOLID, 1, _BorderColor.AsCOLORREF());
	}
	else
	{
		_Pen.CreatePen(PS_SOLID, 0, GetColor().AsCOLORREF());
	}

	CBrushHandle bold = dc.SelectBrush(Brush);
	CPenHandle pold = dc.SelectPen(_Pen);
	
	//Массив вершин по периметру элипса
	POINT points[360]; 
	
	//Расчет вершин
	for(int Fi = 0; Fi < 360; Fi++)
	{
		float angle = PI*Fi/180 - GetAngle();
		float r = a*b/sqrt((pow(a*sin(angle),2) + pow(b*cos(angle),2)));
		points[Fi].x = (LONG)(center.x + cos(PI*Fi/180)*r);
		points[Fi].y = (LONG)(center.y + sin(PI*Fi/180)*r);
		//Перевод из глобальных координат в экранные
		points[Fi] = mng->GlobalToScreen(sVector(points[Fi]));
	}
	
	//Вывод элипса на экран
	::Polygon(dc, (LPPOINT)points, 360);

	//Освобождение памяти
	dc.SelectBrush(bold);
	dc.SelectPen(pold);
}


void CObjEllipse::Serialize(CMemFile& file, BOOL IsLoading)
{
	if(IsLoading)
	{
		file>>_BorderColor;
		file>>_DrawBorder;
	}
	else
	{
		file<<_BorderColor;
		file<<_DrawBorder;
	}
	CObject::Serialize(file, IsLoading);
}



