#pragma once
#include "DataLayer.h"

namespace DataProcessors
{
	//Типы событий
	enum enEventType
	{
		ET_CHANGE				= 0,	//Изменение канала
		ET_CHANGE_MIN_TO_MAX	= 1,	//Изменение канала в большую сторону
		ET_CHANGE_MAX_TO_MIN	= 2,	//Изменение канала в меньшую сторону
		ET_CROSS_VAL1_UP		= 3,	//Пересечение уставки 1 вверх
		ET_CROSS_VAL1_DOWN		= 4,	//Пересечение уставки 1 вниз
		ET_CROSS_VAL2_UP		= 5,	//Пересечение уставки 2 вверх
		ET_CROSS_VAL2_DOWN		= 6		//Пересечение уставки 2 вниз
	};

	//Генерация событий на изменение канала
	struct fGenEvntOnChange :public fDataProcessor
	{
		fGenEvntOnChange();
		~fGenEvntOnChange();
		void Process(sData&);

	protected:
		CComVariant _LastData;
		IEventsPtr	_Events;
	};

	//Генерировать событие на пересечение границы
	struct fGenEvntOnCrossValue :public fDataProcessor
	{
		fGenEvntOnCrossValue(CComVariant Value,ULONG EvntUp, ULONG EvntDown);
		~fGenEvntOnCrossValue();
		void Process(sData&);

	protected:
		CComVariant _Value;
		IEventsPtr	_Events;
		BOOL		_Reach;
		BOOL		_Leave;
		ULONG		_EvntUp;
		ULONG		_EvntDown;
	};

	//Безусловная архивация
	struct fArchive : public fDataProcessor
	{
		fArchive() : fDataProcessor()
		{
			_Channels	= NULL;
			ZeroMemory(&_LastTime,sizeof(FILETIME));
			_LastID		= NULL;
		}
		~fArchive()
		{
			_Channels = NULL;
		}

		void Process(sData& data)
		{
			HRESULT hr;
			//Если подключение к БД не установлено, подключаемся
			if(!IsConnected())
			{
				if(!Connect())
				{
					CEventLoggerPtr log;
					log->LogMessage(L"Ошибка при подключении к БД");
					throw E_FAIL;
				}
				//Запрашиваем интерфейс IChannels
				hr = _Connection->GetChannels(&_Channels);
				if(FAILED(hr))
				{
					CEventLoggerPtr log;
					log->DecodeIError(_Connection);
					throw hr;
				}
			}

			//Переводим время в формат VARIANT
			CComVariant tmp;
			SYSTEMTIME st;
			FileTimeToSystemTime(&data.Time, &st);
			tmp.vt = VT_DATE;
			SystemTimeToVariantTime(&st, &tmp.date);

			//помещаем данные в БД
			if(tmp.date<0) return;
			hr = _Channels->AddData(data.ItemID, tmp, data.Quality, data.Data);
			if(hr != S_OK)
			{
				CEventLoggerPtr log;
				log->DecodeIError(_Channels);
				throw hr;
			}
			//TRACE(L"Data processed\n\r");
		}

	protected:
		IChannelsPtr _Channels;
		FILETIME _LastTime;
		ULONG _LastID;
	};

	//Условная архивация
	//Пример: fArchiveByOp< less<CComVariant> > arc(NULL, CComVariant(10));
	//Пример: arc.Process(Data);
	template<class Func, typename ValueT = CComVariant>
	struct fArchiveByOp : public fDataProcessor
	{
		fArchiveByOp(ValueT Value)
			: fDataProcessor()
			, _Value(Value)
		{
			_Channels = NULL;
		}
		~fArchiveByOp()
		{
			_Channels = NULL;
		}
		void Process(sData& data)
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
				HRESULT hr;
				hr = _Connection->GetChannels(&_Channels);
				if(FAILED(hr))
				{
					CEventLoggerPtr log;
					log->DecodeIError(_Connection);
					throw hr;
				}
			}

			//Вызываем функцию(задается в качестве параметра) 
			// для проверки помещать ли данные в БД
			if(!Func()(data.Data, _Value))
				return;

			//Преобразуем время к формату VARIANT
			CComVariant tmp;
			SYSTEMTIME st;
			FileTimeToSystemTime(&data.Time, &st);
			tmp.vt = VT_DATE;
			SystemTimeToVariantTime(&st, &tmp.date);

			//Помещаем данные в БД
			_Channels->AddData(data.ItemID, tmp, data.Quality, data.Data);
		
		}

	protected:
		ValueT _Value;
		IChannelsPtr _Channels;
	};
}