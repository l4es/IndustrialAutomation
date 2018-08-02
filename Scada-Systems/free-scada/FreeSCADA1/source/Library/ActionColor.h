#pragma once
/********************************************************************
	Создан:	        2005/02/08
	Создан:	        8.2.2005   10:56
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\ActionColor.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Library
	Имя файла:	    ActionColor
	Расширение:	    h
	Автор(ы):	    Михаил
    Класс(ы):       CActionColor
	
	Описание:	Данный action изменяет цвет объекта  в зависимости 
	от значения выражения. Цвет плавно изменяется от начального к конечному.
*********************************************************************/


#include "action.h"
#include "Object.h"


class CActionColor
	: public CAction
{
public:
	//Конструктор/деструктор
	CActionColor(CObject* Owner_);
	~CActionColor(void);

	//Функция "обновления"
	virtual void Update();

	//Установить начальное значение цвета
	inline void SetStartColor(sRGB Color){_StartColor = Color;};
	//Получить начальное значение цвета
	inline sRGB GetStartColor(){return _StartColor;};
	
	//Установить конечное значение цвета
	inline void SetEndColor(sRGB Color){_EndColor = Color;};
	//Получить конечное значение цвета
	inline sRGB GetEndColor(){return _EndColor;};
	
	//Функция загрузки/сохранения
	virtual void Serialize(CMemFile& file, BOOL IsLoading = TRUE);

	//Получить значение параметра экшена по его символьному названию
	virtual bool GetParamValue(wstring Name, CComVariant& Variant);
	//Установить значение параметра экшена по его символьному названию
	virtual bool SetParamValue(wstring Name, CComVariant Variant);

protected:
	sRGB _StartColor;
	sRGB _EndColor;
};
