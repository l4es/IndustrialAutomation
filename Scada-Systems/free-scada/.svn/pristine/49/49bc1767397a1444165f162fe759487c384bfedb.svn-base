#pragma once

/********************************************************************
Создан:	        2004/12/30 
Создан:	        30.12.2004   11:56
Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\Track.h
Директория:	    d:\Work\Projects\М-Немо\Source\Library
Имя файла:		Track
Расширение:	    h
Автор(ы):	    Алексей
Класс(ы):       CTrack

Описание:	Класс рисует пунктирную линию при выборе объектов с помощью рамки
*********************************************************************/

#include "object.h"

class CTrack :
	public CObject
{
public:
	CTrack(void);
	~CTrack(void);

	void SetVertex(sVector, sVector); //Определение размера и позиции CTrack 
	//по двум произвольным точкам указанным на экране

	virtual void Draw(HDC dc_); //Рисование рамки

protected:
	CPen _Pen;
};
