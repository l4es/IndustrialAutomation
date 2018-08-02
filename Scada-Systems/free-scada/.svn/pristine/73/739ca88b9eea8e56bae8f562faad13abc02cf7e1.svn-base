#pragma once

/********************************************************************
Создан:	        2004/12/30
Создан:	        30.12.2004   11:56
Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\ObjPolyline.h
Директория:	    d:\Work\Projects\М-Немо\Source\Library
Имя файла:	    ObjPolyline
Расширение:	    h
Автор(ы):	    Алексей, Михаил
Класс(ы):       CObjPolyline

Описание:	Класс реализует объект полилинии
*********************************************************************/

#include "object.h"

class CObjPolyline
	: public CObject
{
public:
	typedef vector< sVector >::iterator Iter;

	CObjPolyline(void);
	~CObjPolyline(void);

	virtual void Draw(HDC dc); //Рисование полилинии
	virtual void Serialize(CMemFile& file, BOOL IsLoading = 1);
	virtual BOOL OnMouseLButtonDblClk(sVector Point);
	virtual BOOL OnKeyDown(WPARAM wParam, sVector Point);
	void CalcSizeAndPos(); //Расчет размера и расположения полилинии

	virtual void OnLostFocus(); 
	virtual void OnSetFocus();

	
	void AddVertex(sVector vertex); //Добавлние точки к полилинии
	void AddVertex(float x, float y); //Добавлние точки к полилинии
	vector< sVector >& GetVertexes(){ return _Vertex; }; //Получение вектора вершин 
														//в относительных координатах
	vector<sVector> GetVertexesGlobal(); //Получение вектора вершин 
											//в абсолютных координатах
protected:
	vector< sVector > _Vertex; //Вектор с точками в относительных координатах
	
};
