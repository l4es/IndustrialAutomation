#include "StdAfx.h"
#include ".\dataprocessors.h"
#include <functional>

namespace DataProcessors
{
	fGenEvntOnChange::fGenEvntOnChange()
		:fDataProcessor()
	{
		_Events = NULL;
		_LastData.Clear();
	}

	fGenEvntOnChange::~fGenEvntOnChange()
	{
		_Events = NULL;
	}

	void fGenEvntOnChange::Process(sData& data)
	{
		//Если подключение к БД не установлено, подключаемся
		if(!IsConnected())
		{
			if(!Connect())
			{
				CEventLoggerPtr log;
				log->LogMessage(L"Ошибка при подключении к БД");
				throw E_FAIL;
			}
			//Запрашиваем интерфейс IEvents
			HRESULT hr;
			hr = _Connection->GetEvents(&_Events);
			if(FAILED(hr))
			{
				CEventLoggerPtr log;
				log->DecodeIError(_Connection);
				throw hr;
			}
		}

		//Сравниваем данные только если есть предыдущее значение
		if(_LastData.vt != VT_EMPTY)
		{
			_LastData.ChangeType(data.Data.vt);
			if(_LastData == data.Data)
				return;	//Данные равны. Никаких сообщений генерировать ненадо
		
			//Для типа BOOL надо перевести -1 в 1
			_LastData.ChangeType(data.Data.vt);
			if(_LastData.vt == VT_BOOL)
				_LastData = _LastData.boolVal?1:0;
			if(data.Data.vt == VT_BOOL)
				data.Data = data.Data.boolVal?1:0;

			//Генерируем событие
			if(_LastData < data.Data)
				_Events->AddEvent(data.ItemID, ET_CHANGE_MIN_TO_MAX, CComVariant());
			else
				_Events->AddEvent(data.ItemID, ET_CHANGE_MAX_TO_MIN, CComVariant());
		}

		//Обновляем "прошлое" значение
		_LastData = data.Data;
	}

	fGenEvntOnCrossValue::fGenEvntOnCrossValue(CComVariant Value,ULONG EvntUp, ULONG EvntDown)
		: fDataProcessor()
		, _Value(Value)
		, _Reach(FALSE)
		, _Leave(FALSE)
		, _EvntUp(EvntUp)
		, _EvntDown(EvntDown)
	{
		
	}
	fGenEvntOnCrossValue::~fGenEvntOnCrossValue()
	{
		_Events = NULL;
	}

	void fGenEvntOnCrossValue::Process(sData& data)
	{
		//Если подключение к БД не установлено, подключаемся
		if(!IsConnected())
		{
			if(!Connect())
			{
				CEventLoggerPtr log;
				log->LogMessage(L"Ошибка при подключении к БД");
				throw E_FAIL;
			}
			//Запрашиваем интерфейс IEvents
			HRESULT hr;
			hr = _Connection->GetEvents(&_Events);
			if(FAILED(hr))
			{
				CEventLoggerPtr log;
				log->DecodeIError(_Connection);
				throw hr;
			}
		}

		//Для типа BOOL надо перевести -1 в 1
		if(_Value.vt == VT_BOOL)
			_Value = _Value.boolVal?1:0;
		if(data.Data.vt == VT_BOOL)
			data.Data = data.Data.boolVal?1:0;

		//Генерируем событие только в момент пересечения уставки
		// во всех других случаях ничего не генерируется
		if(data.Data > _Value && !_Reach)
		{
			//Значение канала только что пересекло границу вверх
			_Events->AddEvent(data.ItemID, _EvntUp, CComVariant());
			_Reach = TRUE;
			_Leave = FALSE;
		}
		else 
			if(data.Data < _Value && !_Leave)
			{
				//Значение канала только что пересекло границу вниз
				_Events->AddEvent(data.ItemID, _EvntDown, CComVariant());
				_Reach = FALSE;
				_Leave = TRUE;
			}
	}
}