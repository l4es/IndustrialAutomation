#pragma once
/********************************************************************
	Создан:	        2005/02/08
	Создан:	        8.2.2005   11:04
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\ActionMenager.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Library
	Имя файла:	    ActionMenager
	Расширение:	    h
	Автор(ы):	    Алексей, Михаил
    Класс(ы):       CActionMenager
	
Описание:	Данный клас представляет собой менеджер экшенов(действий). 
Реализован по патерну Singleton. 
Реализует: обновление, загрузку/восстановление, добавление, удаление и т.д.

  Каждый объект может содержать сколько угодно действий, но эти действия 
должны быть разных типов. Реализовано в виде крты <объект,список действий>.
*********************************************************************/

#include "Action.h"
class CArchiveManager;

class CActionMenager
	: public CSingleton<CActionMenager>
{
public:
	typedef list<CAction*>			TActions;
	typedef map<wstring, TActions>	TMapObjActions;
	typedef list<enActionType>		TActionTypes;

	//Добавить действие к объекту
	CAction * CActionMenager::AddAction(wstring Object, enActionType Type);
	//Найти действие заданного типа у объекта
	CAction * FindAction(wstring Object,const wstring &name);
	CAction* FindAction(const wstring &name);

	//Удалить действие заданного типа у объекта
	BOOL RemoveAction(wstring Object, const wstring & name);
	//Удалить все действия
	BOOL RemoveAll();
	//Удалить все действия у заданного объекта
	BOOL RemoveObject(wstring Object);
	
	//Обновить параметры действия
	BOOL ModifyAction(wstring Object, CAction* Action);

	//Получить список всех действий у заданного объекта
	BOOL GetAllActions(wstring Object, TActions& Actions);
	TActions GetAllActions(wstring Object, enActionType Type);
	//Обновить все действия (вызов OnUpdate для каждого действия)
	void UpdateActions();

	//Загрузка/сохранение
	virtual void Serialize(CMemFile& file, BOOL IsLoading=TRUE);

protected:
	CActionMenager(void);
	~CActionMenager(void);
	DEF_SINGLETON(CActionMenager);
	CArchiveManager *_ArcManager;
	TMapObjActions _Actions;
};

typedef CSingletonPtr<CActionMenager> CActionMenagerPtr;