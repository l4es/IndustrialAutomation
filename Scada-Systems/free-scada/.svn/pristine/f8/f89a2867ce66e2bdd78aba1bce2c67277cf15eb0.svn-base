#pragma once
#include "object.h"
/********************************************************************
	создан:	        2004/12/07
	создан:	        7:12:2004   18:05
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\ObjRectangle.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Library
	Имя файла:	    ObjRectangle
	Расширение:	    h
	Автор(ы):	    Михаил
    Класс(ы):       CObjRectangle
	
Описание: Класс визуального объекта "Прямоугольник"
*********************************************************************/

class CObjRectangle
	: public CObject
{
public:
	//Constructor/Destructor
	CObjRectangle(void);
	virtual ~CObjRectangle(void);

	//Прорисовка объекта
	virtual void Draw(HDC dc);

	//Получить цвет рамки
	inline sRGB GetBorderColor(){return _BorderColor;};
	//Установить цвет рамки
	inline void SetBorderColor(sRGB Color){_BorderColor = Color;};
	//Установить рисовать или нет рамку
	inline void DrawBorder(bool Draw=true){_DrawBorder = Draw;};
	//Определить рисуется рамка или нет
	inline bool IsDrawBorder(){return _DrawBorder;};
	void Serialize(CMemFile& file, BOOL IsLoading = TRUE);

protected:
	sRGB	_BorderColor;
	bool	_DrawBorder;
};