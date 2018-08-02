#include "common.h"
#include ".\object.h"
#include "Action.h"
#include "ObjectMenager.h"
#include "Graphic.h"
#include "MemFile.h"

CObject::CObject(bool AutoDelete)
	:_AutoDelete(AutoDelete)
{
	_Scale.x = _Scale.y = 1.0f;
	_Angle = 0;
	_MLButtonDown = false;
	_Name.clear();
	_ActionFlags = 0;
	_Visible = true;
}

CObject::~CObject(void)
{
}

void CObject::Serialize(CMemFile& file, BOOL IsLoading)
{
	if(IsLoading)
	{
		file>>_Name;
		file>>_TypeID;
		file>>_Color;
		file>>_Position;
		file>>_Size;
		file>>_Scale;
		file>>_Visible;
		file>>_Angle;
		file>>_OrderPos;
	}
	else
	{
		file<<_Name;
		file<<_TypeID;
		file<<_Color;
		file<<_Position;
		file<<_Size;
		file<<_Scale;
		file<<_Visible;
		file<<_Angle;
		file<<_OrderPos;
	}
}

void CObject::MoveTo(const sVector Dest)
{
	_Position = Dest;
	CalculateBounds();
}

void CObject::MoveTo(const float x, const float y)
{
	_Position.x = x;
	_Position.y = y;
	CalculateBounds();
}

void CObject::MoveBy(const sVector Offset)
{
	_Position += Offset;
	CalculateBounds();
}

void CObject::MoveBy(const float x, const float y)
{
	_Position.x += x;
	_Position.y += y;
	CalculateBounds();
}

void CObject::SetSize(const sVector Size)
{
	_Size = Size;
	CalculateBounds();
}

void CObject::SetSize(const float cx, const float cy)
{
	_Size.x = cx;
	_Size.y = cy;
	CalculateBounds();
}

void CObject::SetScale(const sVector Scale)
{
	_Scale = Scale;
	CalculateBounds();
}

void CObject::SetScale(const float x, const float y)
{
	_Scale.x = x;
	_Scale.y = y;
	CalculateBounds();
}

void CObject::SetColor(sRGB Color)
{
	_Color = Color;
}

void CObject::SetColor(BYTE r, BYTE g, BYTE b)
{
	_Color.r = r;
	_Color.g = g;
	_Color.b = b;
}

void CObject::SetColor(COLORREF Color)
{
	_Color.r = GetGValue(Color);
	_Color.g = GetGValue(Color);
	_Color.b = GetBValue(Color);
}

bool CObject::HitTest(sVector point)
{
	sVector pos		= GetPosition();
	sVector size	= GetSize();
	sVector center	= size/2;

	sVector points[4];
	points[0] = sVector(-center.x, -center.y);
	points[1] = sVector(+center.x, -center.y);
	points[2] = sVector(+center.x, +center.y);
	points[3] = sVector(-center.x, +center.y);

	for(int i=0;i<4;i++)
	{
		points[i].Rotate(GetAngle());
		points[i] += pos+center;
	}

	CGraphic graph(NULL);
	return graph.TestPtInRect(point, points);
}

BOOL CObject::OnMouseLButtonDblClk(sVector Point)
{

	return FALSE;
}

BOOL CObject::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

BOOL CObject::OnMouseLButtonDown(sVector Point)
{
	return FALSE;
}


BOOL CObject::OnMouseRButtonDown(sVector Point)
{
	return FALSE;
}



BOOL CObject::OnMouseLButtonUp(sVector Point)
{
	return FALSE;
}

BOOL CObject::OnMouseMove(sVector Point)
{
	return FALSE;
}

void CObject::OnSetFocus()
{

}

void CObject::OnLostFocus()
{

}

void CObject::OnEdit()
{
}

void CObject::CalculateBounds()
{

	sVector center = GetPosition() + GetSize()/2;
	sVector points[4];
	points[0] = GetPosition();
	points[1] = sVector((GetPosition() + GetSize()).x, GetPosition().y);
	points[2] = GetPosition() + GetSize();
	points[3] = sVector(GetPosition().x, (GetPosition() + GetSize()).y); 

	for(int i = 0; i <= 3; i++)
	{
		points[i] -= center;
		float angle = GetAngle() + atan2(points[i].y, points[i].x);
		float s = points[i].Length();
		points[i].x = s*cos(angle);
		points[i].y = s*sin(angle);
		points[i] += center;
	}

	_BoundLT = points[0];
	_BoundRB = points[0];

	for(int i = 1; i <= 3; i++)
	{
		if(points[i].x < _BoundLT.x)_BoundLT.x = points[i].x;
		if(points[i].y < _BoundLT.y)_BoundLT.y = points[i].y;
		if(points[i].x > _BoundRB.x)_BoundRB.x = points[i].x;
		if(points[i].y > _BoundRB.y)_BoundRB.y = points[i].y;
	}

}



