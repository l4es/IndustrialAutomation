#pragma once
/********************************************************************
	Создан:	        2005/02/08
	Создан:	        8.2.2005   11:25
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\Action.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Library
	Имя файла:	    Action
	Расширение:	    h
	Автор(ы):	    Алексей, Михаил
    Класс(ы):       CAction
	
Описание:	Базовый класс CAction для всех действий. 
Общая идея для всех действий: вычисляется значение выражения, обрезается в
соответствии с min/max, полученное значение используется для изменения 
параметра объекта-владельца.
*********************************************************************/


#include "Equation.h"
#include "MemFile.h"

//////////////////////////////////////////////////////////////////////////
//Типы действий
enum enActionType
{
	AT_NONE			= 0,
	AT_MOVER		= 1<<0,
	AT_SIZER		= 1<<1,
	AT_ROTATE		= 1<<2,
	AT_VISUAL		= 1<<3,
	AT_COLOR		= 1<<4,
	AT_PLAY_SOUND	= 1<<5,
	AT_CHANGE_IMAGE = 1<<6,
	AT_CURVE		= 1<<7
};
//////////////////////////////////////////////////////////////////////////
//Наборы флагов поддерживаемых объектами действий
//Все действия
#define AT_ALL					0xff
//Все визуальные действия
#define AT_ALL_VISUAL			(AT_MOVER|AT_SIZER|AT_ROTATE|AT_VISUAL|AT_COLOR)
//Все кроме вращения
#define AT_ALL_NOROTATE			(AT_ALL_VISUAL&(~AT_ROTATE))

//////////////////////////////////////////////////////////////////////////
//Карта обработки значений параметров действий по их символьному названию (получение)
#define BEGIN_GET_VALUE_MAP(Name_, Variant)	{wstring __n = Name_; CComVariant& __v = Variant;
#define GET_VALUE_STRING(Name_, Method)	if(Name_ == __n){__v = (##Method); __v.ChangeType(VT_BSTR);}
#define GET_VALUE_FLOAT(Name_, Method)	if(Name_ == __n){__v = (##Method); __v.ChangeType(VT_R4);}
#define GET_VALUE_INT(Name_, Method)	if(Name_ == __n){__v = (##Method); __v.ChangeType(VT_I2);}
#define GET_VALUE_COLOR(Name_, Method)	if(Name_ == __n){__v = (##Method); __v.ChangeType(VT_UI4);}
#define GET_VALUE_BOOL(Name_, Method)	if(Name_ == __n){__v = (##Method); __v.ChangeType(VT_I2);}
#define END_GET_VALUE_MAP()	}

//////////////////////////////////////////////////////////////////////////
//Карта обработки значений параметров действий по их символьному названию (установка)
#define BEGIN_SET_VALUE_MAP(Name, Variant)	{ wstring __n = Name;	CComVariant& __v = Variant;
#define SET_VALUE_STRING(Name, Method)	if(Name == __n){if(__v.vt == VT_BSTR){wstring val(_bstr_t(__v.bstrVal));	##Method; return true;}}
#define SET_VALUE_FLOAT(Name, Method)	if(Name == __n){if(__v.vt == VT_R4){float val = __v.fltVal ;	##Method; return true;}}
#define SET_VALUE_INT(Name, Method)		if(Name == __n){if(__v.vt == VT_I2){int val = __v.iVal ;		##Method; return true;}}
#define SET_VALUE_COLOR(Name, Method)	if(Name == __n){if(__v.vt == VT_UI4){ULONG val = __v.ulVal ;	##Method; return true;}}
#define SET_VALUE_BOOL(Name, Method)	if(Name == __n){if(__v.vt == VT_I2){bool val = __v.iVal ;		##Method; return true;}}
#define END_SET_VALUE_MAP() }

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CObject;
class CAction
{
public:
	//Конструктор/деструктор
	CAction(CObject* Owner_);
	virtual ~CAction(void);

	//Функция "обновления" (должна быть реализована в каждом наследнике)
	virtual void Update()=0;

	//Функция загрузки/сохранения
	virtual void Serialize(CMemFile& file, BOOL IsLoading=TRUE);

	//Установить владельца(объект) для действия
	virtual void SetOwner(CObject* Owner);

	double CalcControlValue();

	//Получить тип действия
	inline enActionType GetType(){return _Type;};

	//Установить пределы для результирующего значения выражения (min,max)
	inline void SetValueLimit(float Min, float Max){_MinVal = Min;_MaxVal = Max;};
	//Получить нижний предел для результирующего значения выражения
	inline float GetValueMin(){return _MinVal;};
	//Получить верхний предел для результирующего значения выражения
	inline float GetValueMax(){return _MaxVal;};

	//Задать выражение
	inline void SetExpression(wstring expr){_Expression = expr;};
	//Получить выражение
	inline wstring GetExpression(){return _Expression;};

	//Задать переменную
	inline void SetVariable(wstring var){_Variable = var;};
	//Получить переменную
	inline wstring GetVariable(){return _Variable;};
	inline void		SetName(wstring Name){_Name = Name;};
	inline wstring 	GetName(){return _Name ;};

	//Получить значение параметра экшена по его символьному названию
	virtual bool GetParamValue(wstring Name, CComVariant& Variant);
	//Установить значение параметра экшена по его символьному названию
	virtual bool SetParamValue(wstring Name, CComVariant Variant);
	

protected:
	CObject*		_Owner;
	enActionType	_Type;
	float			_MinVal;
	float			_MaxVal;
	wstring			_Expression;
	CEquation		_Equation;
	wstring			_Variable;
	wstring			_Name;
};
