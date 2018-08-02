#pragma once
/********************************************************************
	Создан:	        2005/02/08
	Создан:	        8.2.2005   10:29
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\ActionRotate.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Library
	Имя файла:	    ActionRotate
	Расширение:	    h
	Автор(ы):	    Алексей
    Класс(ы):       CActionRotate
	
	Описание:	Данный action поворачивает объект на угол в зависимости от
	значения выражения.
*********************************************************************/

#include "action.h"
#include "Object.h"


class CActionRotate :
	public CAction
{
public:
	//Конструктор/деструктор
	CActionRotate(CObject* Owner_);
	~CActionRotate(void);

	//Функция "обновления"
	virtual void Update();

	//Функция загрузки/сохранения
	virtual void Serialize(CMemFile& file, BOOL IsLoading = TRUE);

	//Получить значение параметра экшена по его символьному названию
	virtual bool GetParamValue(wstring Name, CComVariant& Variant);
	//Установить значение параметра экшена по его символьному названию
	virtual bool SetParamValue(wstring Name, CComVariant Variant);

	//Перегружаем функцию задания владельца (получаем угол для начала отсчета)
	void SetOwner(CObject* Owner);

	//Установить значение угла для минимального значения
	void SetStartAngle(float Start);
	//Получить значение угла для минимального значения
	float GetStartAngle();

	//Установить значение угла для максимального значения
	void SetFinishAngle(float Finish);
	//Получить значение угла для максимального значения
	float GetFinishAngle();

protected:
	float _BaseAngle;
	float _StartAngle;
	float _FinishAngle;
};
