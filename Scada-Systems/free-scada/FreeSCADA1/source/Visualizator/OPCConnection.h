#pragma once
/********************************************************************
	Создан:	        2005/02/08
	Создан:	        8.2.2005   17:15
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Visualizator\OPCConnection.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Visualizator
	Имя файла:	    OPCConnection
	Расширение:	    h
	Автор(ы):	    Михаил
    Класс(ы):       COPCConnection
	
Описание:	Класс реализует подключение к OPC серверу и создает 
			локальную группу. Далее этот клас должен 
			использоваться для подключения каналов.
*********************************************************************/

class CDoc;
class COPCConnection
{
protected:
	wstring					_Computer;
	CLSID					_Clsid;
	CComPtr<IOPCServer>		_Server;
	CComPtr<IOPCItemMgt>	_Group;
	OPCHANDLE				_hGroup;
	DWORD					_Advise;

public:
	COPCConnection(void);
	~COPCConnection(void);

	//Получить CLSID
	inline CLSID GetCLSID(){return _Clsid;}
	//Получить имя компьютера к которому подключен OPC
	inline wstring GetComputer(){return _Computer;}
	//Получить IUnknown группы
	inline CComPtr<IUnknown> GetGroupIUnknown(){return (CComPtr<IUnknown>)_Group;};

	//Подключиться к OPC серверу
	BOOL Connect(wstring Computer, CLSID &Clsid, CComPtr<IUnknown> Callback, CDoc* pDoc);
	//Отключиться от OPC сервера
	void Disconnect();
	bool TestServer();

	//Добавить канал для обновления
	// Tag	- полное название канала
	// hTag	- ID канала со стороны клиента
	// Результат (hServerTag) - ID канала со стороны сервера
	BOOL AddTag(wstring Tag, OPCHANDLE hTag, OPCHANDLE* hServerTag);
	//Отключить канал
	void RemoveTag(OPCHANDLE hServerTag);
};
