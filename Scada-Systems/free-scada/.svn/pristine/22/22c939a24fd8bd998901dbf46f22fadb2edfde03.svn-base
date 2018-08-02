#include "Common.h"
#include ".\variablemenager.h"
#include "ObjInput.h"
#include "ObjButton.h"
#include "ObjText.h"
#include "ActionMenager.h"

CAutoSingletonCreator<CVariableMenager> __VariableMenager;

TStrings CVariableMenager::GetVariableList()
{
	//Получить список всех переменных
	TStrings v;
	_CS.Lock();
	for(TVariables::const_iterator i=_Variables.begin();i!=_Variables.end();i++)
		v.push_back(i->first);
	_CS.Unlock();
	return v;
}

void CVariableMenager::AddOPCVariable(wstring Name, CComVariant Value, wstring TagName, GUID guid, wstring Computer)
{
	//Добавить переменную связанную с OPC
	sVariable var;
	var.Type	= VT_OPC;
	var.Value	= Value;
	var.Settings.OPC.Computer	= Computer;
	memcpy(&var.Settings.OPC.guid, &guid, sizeof(GUID));
	var.Settings.OPC.TagName	= TagName;
	_CS.Lock();
	_Variables[Name] = var;
	_CS.Unlock();
};

void CVariableMenager::AddSimpleVariable(wstring Name, CComVariant Value)
{
	//Добавить простую переменную
	sVariable var;
	var.Type	= VT_SIMPLE;
	var.Value	= Value;
	_CS.Lock();
	_Variables[Name] = var;
	_CS.Unlock();
}

CComVariant CVariableMenager::GetVariable(wstring Name, CComVariant Default)
{
	//Получить значение переменной. Если такой переменной нет возвращается 
	//значение по умолчанию

	CComVariant v;
	v = Default;
	_CS.Lock();
	TVariables::const_iterator it = _Variables.find(Name);
	if(it != _Variables.end())
		v = _Variables[Name].Value;
	_CS.Unlock();
	return v;
}

CVariableMenager::enVariableType CVariableMenager::GetVariableType(wstring Name)
{
	_CS.Lock();
	TVariables::const_iterator it = _Variables.find(Name);
	if(it != _Variables.end())
	{
		enVariableType Type = _Variables[Name].Type;
		_CS.Unlock();
		return Type;
	}
	_CS.Unlock();
	return VT_NONE;
}

TVariableSettings CVariableMenager::GetVariableSetting(wstring Name)
{
	TVariableSettings vs;
	vs.OPC.Computer.empty();
	vs.OPC.TagName.empty();
	ZeroMemory(&vs.OPC.guid, sizeof(GUID));

	_CS.Lock();
	TVariables::const_iterator it = _Variables.find(Name);
	if(it != _Variables.end())
		vs = _Variables[Name].Settings;
	_CS.Unlock();
	return vs;
}

void CVariableMenager::SetVariable(wstring Name, CComVariant Value, bool WriteToServer)
{
	//Записать значение в переменную. Если установлен флаг WriteToServer и переменная 
	//имеет тип OPC, то произойдет запись значения в OPC-канал

	BOOL PostToOPC_ = FALSE;
	sVariable v;

	_CS.Lock();
	//Поиск переменной
	TVariables::const_iterator it = _Variables.find(Name);
	if(it != _Variables.end())
	{
		//Переменная найдена. Записываем новое значение.
		_Variables[Name].Value = Value;

		if(_Variables[Name].Type == VT_OPC)
		{
			//Переменная связана с OPC. Копируем переменную в локальную область памяти
			//и устанавливаем флаг для ее отправки.
			PostToOPC_ = TRUE;
			v = _Variables[Name];
		}
	}
	_CS.Unlock();

	if(PostToOPC_ && WriteToServer)
	{
		//Отправить в OPC
//		PostToOPC(v);
		SendToOPC(v);
	}
}

void CVariableMenager::PostToOPC(sVariable &var)
{
	ATLASSERT(var.Type == VT_OPC);
	ATLASSERT(!var.Settings.OPC.Computer.empty());
	ATLASSERT(var.Settings.OPC.guid != GUID_NULL);
	ATLASSERT(!var.Settings.OPC.TagName.empty());

	//Создаем новый блок памяти для передачи в поток.
	sVariable *param = new sVariable;
	*param = var;

	//Создание нового потока для отправки значения переменной в OPC
	DWORD ThreadID;
	CreateThread(	NULL,
					NULL,
					PostToOPCThread,
					param,
					NULL,
					&ThreadID);
}

void CVariableMenager::SendToOPC(sVariable &var)
{
	ATLASSERT(var.Type == VT_OPC);
	ATLASSERT(!var.Settings.OPC.Computer.empty());
	ATLASSERT(var.Settings.OPC.guid != GUID_NULL);
	ATLASSERT(!var.Settings.OPC.TagName.empty());

	//Инициализируем COM
	CoInitialize(NULL);

	CComPtr<IOPCServer>		Server;
	CComPtr<IOPCItemMgt>	Group;
	CComPtr<IOPCSyncIO>		Sync;
	OPCHANDLE				hGroup;

	COSERVERINFO	si;
	MULTI_QI		qi;

	ZeroMemory(&si,sizeof(COSERVERINFO));
	ZeroMemory(&qi,sizeof(MULTI_QI));

	//Запрашиваем интерфейс IOPCServer
	si.pwszName = CComBSTR(var.Settings.OPC.Computer.c_str());
	qi.pIID	= &__uuidof(IOPCServer);
	HRESULT hr;
	hr = CoCreateInstanceEx(var.Settings.OPC.guid, NULL, CLSCTX_ALL, &si, 1, &qi);
	TEST_HR(hr);
	TEST_HR(qi.hr);

	hr = qi.pItf->QueryInterface(__uuidof(IOPCServer), (void**)&Server);
	qi.pItf->Release();
	TEST_HR(hr);

	//Создаем новую группу на сервере
	LONG TimeBias			= NULL;
	FLOAT PercentDeadBand	= NULL;
	DWORD UpdateRate		= NULL;
	GUID riid				= IID_IUnknown;
	CComPtr<IUnknown>		pUnk;
	TEST_HR(Server->AddGroup(	L"",
		TRUE,
		99999999,
		0,
		&TimeBias,
		&PercentDeadBand,
		LCID_RUSSIAN,
		&hGroup,
		&UpdateRate,
		riid,
		&pUnk));
	TEST_HR(pUnk.QueryInterface(&Sync));
	TEST_HR(pUnk.QueryInterface(&Group));

	//Добавляем канал для обновления в группу на сервере
	OPCITEMDEF		item;
	OPCITEMRESULT	*results;
	HRESULT			*Errors;

	ZeroMemory(&item, sizeof(OPCITEMDEF));
	item.bActive	= TRUE;
	item.hClient	= 0;
	item.szItemID	= CComBSTR(var.Settings.OPC.TagName.c_str());

	hr = Group->AddItems(1, &item, &results, &Errors);
	TEST_HR(hr);

	OPCHANDLE hServerTag = results[0].hServer;

	LocalFree(results);
	LocalFree(Errors);

	//Записываем новое значение в канал (синхронная запись)
	HRESULT		*res;
	VARIANT		val;
	VariantInit(&val);
	VariantCopy(&val, &var.Value);
	TEST_HR(Sync->Write(1, &hServerTag, &val, &res));
	LocalFree(res);

	Sync = NULL;

	//Удаляем добавленный ранее канал
	TEST_HR(Group->RemoveItems(1, &hServerTag, &res));
	LocalFree(res);

	//Удаляем группу
	hr = Server->RemoveGroup(hGroup, FALSE);
	if(FAILED(hr))
		TEST_HR(hr);

	//Отключаемся от сервера
	Group = NULL;
	Server = NULL;

	CoUninitialize();
}

DWORD WINAPI CVariableMenager::PostToOPCThread(LPVOID p)
{
	//Поток для отправки значения переменной в OPC
	sVariable *var = (sVariable*)p;
	ATLASSERT(var != NULL);

	//Инициализируем COM
	CoInitialize(NULL);

	CComPtr<IOPCServer>		Server;
	CComPtr<IOPCItemMgt>	Group;
	CComPtr<IOPCSyncIO>		Sync;
	OPCHANDLE				hGroup;

	COSERVERINFO	si;
	MULTI_QI		qi;

	ZeroMemory(&si,sizeof(COSERVERINFO));
	ZeroMemory(&qi,sizeof(MULTI_QI));

	//Запрашиваем интерфейс IOPCServer
	si.pwszName = CComBSTR(var->Settings.OPC.Computer.c_str());
	qi.pIID	= &__uuidof(IOPCServer);
	HRESULT hr;
	hr = CoCreateInstanceEx(var->Settings.OPC.guid, NULL, CLSCTX_ALL, &si, 1, &qi);
	TEST_HR(hr);
	TEST_HR(qi.hr);

	hr = qi.pItf->QueryInterface(__uuidof(IOPCServer), (void**)&Server);
	qi.pItf->Release();
	TEST_HR(hr);

	//Создаем новую группу на сервере
	LONG TimeBias			= NULL;
	FLOAT PercentDeadBand	= NULL;
	DWORD UpdateRate		= NULL;
	GUID riid				= IID_IUnknown;
	CComPtr<IUnknown>		pUnk;
	TEST_HR(Server->AddGroup(	L"",
								TRUE,
								99999999,
								0,
								&TimeBias,
								&PercentDeadBand,
								LCID_RUSSIAN,
								&hGroup,
								&UpdateRate,
								riid,
								&pUnk));
	TEST_HR(pUnk.QueryInterface(&Sync));
	TEST_HR(pUnk.QueryInterface(&Group));

	//Добавляем канал для обновления в группу на сервере
	OPCITEMDEF		item;
	OPCITEMRESULT	*results;
	HRESULT			*Errors;

	ZeroMemory(&item, sizeof(OPCITEMDEF));
	item.bActive	= TRUE;
	item.hClient	= 0;
	item.szItemID	= CComBSTR(var->Settings.OPC.TagName.c_str());

	hr = Group->AddItems(1, &item, &results, &Errors);
	TEST_HR(hr);

	OPCHANDLE hServerTag = results[0].hServer;

	LocalFree(results);
	LocalFree(Errors);

	//Записываем новое значение в канал (синхронная запись)
	HRESULT		*res;
	VARIANT		val;
	VariantInit(&val);
	VariantCopy(&val, &var->Value);
	TEST_HR(Sync->Write(1, &hServerTag, &val, &res));
	LocalFree(res);

	Sync = NULL;

	//Удаляем добавленный ранее канал
	TEST_HR(Group->RemoveItems(1, &hServerTag, &res));
	LocalFree(res);

	//Удаляем группу
	hr = Server->RemoveGroup(hGroup, FALSE);
	if(FAILED(hr))
		TEST_HR(hr);

	//Отключаемся от сервера
	Group = NULL;
	Server = NULL;
	
	//Освобождаем память и отключаем COM
	delete var;
	CoUninitialize();
	return 0;
}

void CVariableMenager::Serialize(CMemFile& file, BOOL IsLoading = 1)
{
	if(IsLoading)
	{
		_Variables.clear();
		long count;
		file>>count;
		for(long i = 0; i < count; i++)
		{
			std::wstring Name;
			file>>Name;

			sVariable Var;
			file>>Var.Value;

			sVariable::sSettings::sOPC& OPC = Var.Settings.OPC;

			long var_type;
			file>>var_type;
			Var.Type = static_cast<enVariableType>(var_type);

			file>>OPC.Computer;
			file>>OPC.TagName;
			file>>OPC.guid;

			_Variables[Name] = Var;
		}
	}
	else
	{
		long count = (long)_Variables.size();
		file<<count;
		for(TVariables::iterator it = _Variables.begin(); it != _Variables.end(); it++)
		{
			wstring Name = it->first;
			file<<Name;

			sVariable& Var = it->second;
			file<<Var.Value;

			sVariable::sSettings::sOPC& OPC = Var.Settings.OPC;
			file<<(long)Var.Type;

			file<<OPC.Computer;
			file<<OPC.TagName;
			file<<OPC.guid;
		}
	}
}


void CVariableMenager::RemoveVariable(wstring Name)
{
	_Variables.erase(Name);
}

bool CVariableMenager::TestVariableInUse(wstring Name)
{
	typedef list<CObject*> TObjects;

	CObjectMenagerPtr om;
	TObjects Inputs = om->FindObjects(_INPUT);
	TObjects Buttons = om->FindObjects(_BUTTON);
	TObjects Text = om->FindObjects(_TEXT);

	for(TObjects::iterator i=Inputs.begin();i!=Inputs.end();i++)
	{
		CObjInput* obj = static_cast<CObjInput*>(*i);
		if(obj == NULL)
			continue;
		if(obj->GetControlVariable() == Name)
			return true;
	}
	for(TObjects::iterator i=Buttons.begin();i!=Buttons.end();i++)
	{
		CObjButton* obj = static_cast<CObjButton*>(*i);
		if(obj == NULL)
			continue;
		if(obj->GetControlVariable() == Name)
			return true;
	}
	for(TObjects::iterator i=Text.begin();i!=Text.end();i++)
	{
		CObjText* obj = static_cast<CObjText*>(*i);
		if(obj == NULL)
			continue;
		wstring expr = obj->GetExpression();
		const wstring::size_type pos = expr.find(Name);
		if(pos != expr.npos)
			return true;
	}

	CActionMenagerPtr am;
	TObjects objects = om->GetObjectsList();
	for(TObjects::iterator i=objects.begin();i!=objects.end();i++)
	{
		if(*i == NULL)
			continue;
		CActionMenager::TActions actions;
		typedef CActionMenager::TActions::iterator it;
		am->GetAllActions((*i)->GetName(), actions);
		for(it j=actions.begin();j!=actions.end();j++)
		{
			wstring variable = (*j)->GetVariable();
			wstring expr = (*j)->GetExpression();
			if(variable == Name)
				return true;

			const wstring::size_type pos = expr.find(Name);
			if(pos != expr.npos)
				return true;
		}
	}

	return false;
}