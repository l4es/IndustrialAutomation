#pragma once

/********************************************************************
Создан:	        2005/01/25
Создан:	        25.1.2005   10:06
Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\ActionSizer.h
Директория:	    d:\Work\Projects\М-Немо\Source\Library
Имя файла:	    ActionSizer
Расширение:	    h
Автор(ы):	    Алексей, Михаил
Класс(ы):       CActionSizer

Описание:	Класс реалиует действие изменение размеров
*********************************************************************/

#include "action.h"
#include "Object.h"

//Тип действия измиенения размров
enum enSizerType 
{
	ST_TOP =	0,
	ST_BOTTOM = 1,
	ST_LEFT =	2,
	ST_RIGHT =	3,
	ST_ALL =	4,
	ST_NONE =	5,
};

class CActionSizer
	:public CAction
{
public:
	CActionSizer(CObject* Owner_);
	~CActionSizer(void);

	//Установка типа действия изменения размеров
	void SetSizerType(enSizerType Type);
	//Получение типа действия изменения размеров
	enSizerType GetSizerType(){ return _SizerType;}
	//Обновление размеров и позиции объекта-владельца
	virtual void Update();
	//Сохранение загрузка действия
	virtual void Serialize(CMemFile& file, BOOL IsLoading);
	//Установка объекта-владельца
	virtual void SetOwner(CObject* Owner);
	//Получение значения объекта 
	virtual bool GetParamValue(wstring Name, CComVariant& Variant);
	//Установка значения объекта
	virtual bool SetParamValue(wstring Name, CComVariant Variant);
	//Получить строковое название типа ActionSizer
	wstring GetSizerTypeAsText();
	//Установить тип ActionSizer через строковую переменную
	void SetSizerTypeAsText(wstring Sizer);

protected:
	//Тип ActionSizer
	enSizerType _SizerType;
	//Текущее положение
	float		_CurrentPos;
	//Размеры и позиция объекта владельца до изменеия его ActionSizer
	sVector		_BaseSize;
	sVector		_BasePos;
};
