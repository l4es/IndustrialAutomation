#pragma once
#include "object.h"
#include "graph/Scatter.h"

/********************************************************************
	создан:	        2004/12/07
	создан:	        7:12:2004   18:05
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\ObjRectangle.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Library
	Имя файла:	    ObjRectangle
	Расширение:	    h
	Автор(ы):	    Алексей
    Класс(ы):       CObjRectangle
	
Описание: Класс визуального объекта "График"
*********************************************************************/
class CObjGraph
	: public CObject
{
public:
	CObjGraph(void);
	virtual ~CObjGraph(void);
	void recalcBounds();
	//Прорисовка объекта
	virtual void Draw(HDC dc);
	inline void SetGraphTime(int t){_GraphTime=t;};
	inline long GetGraphTime(){return _GraphTime;};
	void Serialize(CMemFile& file,  BOOL IsLoading = TRUE);

protected:
		CTimeGraph<double> m_Graph;
		long _GraphTime;// в секундах
	
};