#pragma once
/********************************************************************
	Создан:	        2005/02/08
	Создан:	        8.2.2005   15:54
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Visualizator\Doc.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Visualizator
	Имя файла:	    Doc
	Расширение:	    h
	Автор(ы):	    Михаил
    Класс(ы):       CDoc
	
Описание:	Документ реализует функции загрузки проекта и подключения к OPC.
*********************************************************************/


#include "ArrayOfOPCConnection.h"

class CDoc
	: public DVF::CBaseDocument
{
public:
	CArchiveManager		m_Archive;			//архив
	sSchemaSettings		m_SchemaSettings;	//параметры текущей схемы

	CDoc(void);
	~CDoc(void);

	//Загрузить проект
	bool LoadProject(wstring FileName);
	
	//Выбрать схему
	bool SelectSchema(wstring Name);

	//Очистить вид
	void EmptyView();

	//Подключиться к OPC
	void ConnectToOPC();

	/// Refresh channels (COPCDataCallback sould be called for all tags)
	void RefreshOPC();

	//Получить имя переменной по OPC Handle
	inline wstring GetVariableNameByOPCHandle(OPCHANDLE handle){return _OPCHandles[handle];};

	//void UpdateTagTimer(OPCHANDLE handle);
	//BOOL TestTagTimer();
	inline bool TestCon(){return _OPCConnections->TestServers();};


protected:
	CArrayOfOPCConnection*	_OPCConnections;
	map<OPCHANDLE,wstring>	_OPCHandles;
	map<OPCHANDLE,ULONG>	_OPCTagTimer;
	CComAutoCriticalSection _OPCTimerCS;

	void UpdateMainCaption();
};