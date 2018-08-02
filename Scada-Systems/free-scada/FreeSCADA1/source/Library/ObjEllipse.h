#pragma once
/********************************************************************
	Создан:	        2004/12/08
	Создан:	        8.12.2004   9:46
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\ObjEllipse.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Library
	Имя файла:	    ObjEllipse
	Расширение:	    h
	Автор(ы):	    Алексей
    Класс(ы):       CObjEllipse
	
Описание:	Класс визуального объекта "Элипс"
*********************************************************************/

#include "object.h"

class CObjEllipse
	: public CObject
{
public:
	//Constructor/Destructor
	CObjEllipse(void);
	~CObjEllipse(void);
	
	//Прорисовка объекта
	virtual void Draw(HDC dc);
	virtual void Serialize(CMemFile& file, BOOL IsLoading = 1);

	inline sRGB GetBorderColor(){return _BorderColor;};
	inline void SetBorderColor(sRGB Color){_BorderColor = Color;};
	inline void DrawBorder(bool Draw=true){_DrawBorder = Draw;};
	inline bool IsDrawBorder(){return _DrawBorder;};
protected:
	CPen _Pen;		//"Карандаш" для прорисовки рамки объекта

	sRGB	_BorderColor;
	bool	_DrawBorder;
};
