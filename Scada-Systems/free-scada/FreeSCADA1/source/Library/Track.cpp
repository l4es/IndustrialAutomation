#include "common.h"
#include ".\track.h"
#include "ObjectMenager.h"

CTrack::CTrack(void)
{
	_Pen.CreatePen(PS_DOT, 1, RGB(0,0,0));
	_AutoDelete = false;
}

CTrack::~CTrack(void)
{
}

//Рисование рамки
void CTrack::Draw(HDC dc_)
{
	CDCHandle dc(dc_);

	CPen oldPen = dc.SelectPen(_Pen);

	sVector points[4];
	sVector pos = GetPosition();
	sVector size = GetSize();

	points[0] = sVector(pos);
	points[1] = sVector((pos+size).x, pos.y);
	points[2] = sVector(pos+size);
	points[3] = sVector(pos.x, (pos+size).y);

	CObjectMenagerPtr om;
	for(int i = 0; i <= 3; i++)
		points[i] = om->GlobalToScreen(points[i]);

	dc.MoveTo((int)points[3].x, (int)points[3].y);
	for(int i = 0; i <= 3; i++)
		dc.LineTo((int)points[i].x, (int)points[i].y);
	dc.SelectPen(oldPen);
}

//Определение размера и позиции CTrack 
//по двум произвольным точкам указанным на экране
void CTrack::SetVertex(sVector first, sVector second)
{

	if((first.x  <= second.x) && (first.y  <= second.y))
	{
		MoveTo(first);
		SetSize(second - first);
	}

	if((first.x  > second.x) && (first.y  > second.y))
	{
		MoveTo(second);
		SetSize(first - second);
	}

	if((first.x  < second.x) && (first.y  > second.y))
	{
		MoveTo(first.x, second.y);
		SetSize((second-first).x, (first - second).y);
	}

	if((first.x  > second.x) && (first.y  < second.y))
	{
		MoveTo( second.x, first.y);
		SetSize((first - second).x, (second-first).y );
	}

}



