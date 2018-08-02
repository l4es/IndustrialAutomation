#pragma once
/********************************************************************
	Создан:	        2005/02/08
	Создан:	        8.2.2005   10:24
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\ActionVisual.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Library
	Имя файла:	    ActionVisual
	Расширение:	    h
	Автор(ы):	    Алексей
    Класс(ы):       CActionVisual
	
Описание:	Данный action изменяет флаг видимости объекта в зависимости от
значения выражения.
*********************************************************************/


#include "action.h"
#include "Object.h"

class CActionVisual
	: public CAction
{
public:
	//Конструктор/деструктор
	CActionVisual(CObject* Owner_);
	~CActionVisual(void);

	//Функция "обновления"
	virtual void Update();
};
