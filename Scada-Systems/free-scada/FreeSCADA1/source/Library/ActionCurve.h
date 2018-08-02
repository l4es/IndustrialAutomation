#pragma once
/********************************************************************
	Создан:	        2005/02/08
	Создан:	        8.2.2005   10:56
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\ActionColor.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Library
	Имя файла:	    ActionColor
	Расширение:	    h
	Автор(ы):	    Михаил
    Класс(ы):       CActionCurve
	
	Описание:	Данный action изменяет цвет объекта  в зависимости 
	от значения выражения. Цвет плавно изменяется от начального к конечному.
*********************************************************************/


#include "action.h"
#include "Object.h"


class CActionCurve
	: public CAction
{
public:
	//Конструктор/деструктор
	CActionCurve(CObject* Owner_);
	~CActionCurve(void);

	//Функция "обновления"
	virtual void Update();
	
	inline void SetColor(sRGB Color){_Color = Color;};
	
	inline sRGB GetColor(){return _Color;};

	//Функция загрузки/сохранения
	virtual void Serialize(CMemFile& file, BOOL IsLoading = TRUE);

	//Получить значение параметра экшена по его символьному названию
	virtual bool GetParamValue(wstring Name, CComVariant& Variant);
	//Установить значение параметра экшена по его символьному названию
	virtual bool SetParamValue(wstring Name, CComVariant Variant);
	
	typedef list<pair<time_t,float>> _GraphData_t;
	const _GraphData_t & GetCurveData(){ return _CurveData;};
protected:
	sRGB _Color;
	unsigned int update;

	_GraphData_t _CurveData;
	//Constructor/Destructor

};
