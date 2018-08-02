#pragma once
/********************************************************************
	создан:	        2004/12/07
	создан:	        7.12.2004   18:03
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\VariableMenager.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Library
	Имя файла:	    VariableMenager
	Расширение:	    h
	Автор(ы):	    Михаил
    Класс(ы):       CVariableMenager
	
	  Класс "Менеждер переменных".
	Содержит список переменных. Переменная задается в текстовом виде.Дублирование 
	переменных недопустимо. Клас допускает многопоточное использование.

	  Если переменная связана с каким-то каналом OPC, то запись	этой переменной 
	вызывает изменение соответствующего канала OPC. Обновление связанных с OPC 
	переменных должно осуществлятья из вне класса (соответствующий поток визуализатора)
*********************************************************************/

#include "MemFile.h"
class CVariableMenager
	:public CSingleton<CVariableMenager>
{
public:
	//Тип переменной
	enum enVariableType
	{
		VT_NONE,	//Тип не определен
		VT_SIMPLE,	//Просто переменная
		VT_OPC		//Переменная связана с OPC
	};
	//Структура описывающая переменную
	struct sVariable
	{
		enVariableType			Type;		//Тип переменной
		CComVariant				Value;		//Значение переменной
		struct sSettings
		{
			//Структура описывает связанный с переменной OPC-канал и OPC-сервер
			struct sOPC
			{
				wstring	TagName;			// Нзвание канала
				GUID	guid;				// CLSID сервера
				wstring	Computer;			// Имя компьютера с OPC
			}OPC;
		}Settings;
	};

	//Получить список всех переменных
	TStrings GetVariableList();

	//Добавить переменную связанную с OPC
	void AddOPCVariable(wstring Name, CComVariant Value, wstring TagName, GUID guid, wstring Computer);
	//Добавить простую переменную
	void AddSimpleVariable(wstring Name, CComVariant Value);

	//Получить значение переменной. Если такой переменной нет возвращается 
	//значение по умолчанию
	CComVariant GetVariable(wstring Name, CComVariant Default = NULL);
	const CComVariant operator[](wstring Name){return GetVariable(Name);};

	//Получить тип переменной. 
	CVariableMenager::enVariableType GetVariableType(wstring Name);

	sVariable::sSettings GetVariableSetting(wstring Name);

	//Записать значение в переменную. Если установлен флаг WriteToServer и переменная 
	//имеет тип OPC, то произойдет запись значения в OPC-канал
	void SetVariable(wstring Name, CComVariant Value, bool WriteToServer = false);

	void RemoveVariable(wstring Name);

	void Serialize(CMemFile& file, BOOL IsLoading);

	bool TestVariableInUse(wstring Name);

protected:
	typedef map<wstring, sVariable> TVariables;	//Карта переменных

	//Constructor/Destructor
	CVariableMenager(void){};
	~CVariableMenager(void){};
	DEF_SINGLETON(CVariableMenager);

	//Послать переменную на OPC сервер
	void PostToOPC(sVariable &var);
	void SendToOPC(sVariable &var);
	//Поток отправки переменной на OPC сервер
	static DWORD WINAPI PostToOPCThread(LPVOID);

	TVariables _Variables;			//Список переменных
	CComAutoCriticalSection _CS;	//Критическая секция для доступа к данным из разных потоков
};

typedef CVariableMenager::sVariable::sSettings TVariableSettings;
typedef CSingletonPtr<CVariableMenager> CVariableMenagerPtr;