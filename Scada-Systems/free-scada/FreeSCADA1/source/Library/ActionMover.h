#pragma once
/********************************************************************
	Создан:	        2005/02/08
	Создан:	        8.2.2005   10:43
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\ActionMover.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Library
	Имя файла:	    ActionMover
	Расширение:	    h
	Автор(ы):	    Михаил
    Класс(ы):       CActionMover
	
	Описание:	Данный action поворачивает перемещает объект по заданной 
	траектории (Polyline) в зависимости от значения выражения.
*********************************************************************/

#include "action.h"
#include "Object.h"
#include "ObjPolyline.h"

class CActionMover :
	public CAction
{
public:
	//Конструктор/деструктор
	CActionMover(CObject* Owner_);
	~CActionMover(void);

	//Функция "обновления"
	virtual void Update();

	//Функция загрузки/сохранения
	virtual void Serialize(CMemFile& file, BOOL IsLoading);

	//Получить значение параметра экшена по его символьному названию
	virtual bool GetParamValue(wstring Name, CComVariant& Variant);
	//Установить значение параметра экшена по его символьному названию
	virtual bool SetParamValue(wstring Name, CComVariant Variant);

	//Задать имя объекта, который будет использоваться в качестве пути
	void SetPathObject(wstring PathObject);
	//Получить имя объекта, который будет использоваться в качестве пути
	wstring GetPathObject();

protected:
	float			_CurrentPos;//0..1
	wstring			_PathObject;
};
