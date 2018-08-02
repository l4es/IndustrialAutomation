#include "common.h"
#include ".\actionmenager.h"
#include "ObjectMenager.h"
#include "ActionMover.h"
#include "ActionSizer.h"
#include "ActionRotate.h"
#include "ActionVisual.h"
#include "ActionColor.h"
#include "ActionPlaySound.h"
#include "ActionChangeImage.h"
#include "ActionCurve.h"

CAutoSingletonCreator<CActionMenager> __ActionMenager;

CActionMenager::CActionMenager(void)
{
	_ArcManager=NULL;
}

CActionMenager::~CActionMenager(void)
{
	//Удаляем все действия
	RemoveAll();
}

void CActionMenager::Serialize(CMemFile& file, BOOL IsLoading)
{
	_ArcManager=file.GetArchive();
	if(IsLoading)
	{
		
		//Загрузка
		RemoveAll();
		DWORD count;
		file>>count;
		for(DWORD i = 0; i < count; i++)
		{
			std::wstring ObjName;
			file>>ObjName;

			DWORD countAct;
			file>>countAct;

			TActions actions;
			
			for(DWORD j = 0; j < countAct; j++)
			{
				DWORD Type;
				file>>Type;
				CAction* Action;
				switch(static_cast<enActionType>(Type))
				{
				case AT_MOVER:
					Action = new CActionMover(NULL);
					Action->Serialize(file, IsLoading);
					actions.push_back(Action);
					break;
				case AT_SIZER:
					Action = new CActionSizer(NULL);
					Action->Serialize(file, IsLoading);
					actions.push_back(Action);
					break;
				case AT_ROTATE:
					Action = new CActionRotate(NULL);
					Action->Serialize(file, IsLoading);
					actions.push_back(Action);
					break;
				case AT_VISUAL:
					Action = new CActionVisual(NULL);
					Action->Serialize(file, IsLoading);
					actions.push_back(Action);
					break;
				case AT_COLOR:
					Action = new CActionColor(NULL);
					Action->Serialize(file, IsLoading);
					actions.push_back(Action);
					break;
				case AT_PLAY_SOUND:
					Action = new CActionPlaySound(NULL);
					Action->Serialize(file, IsLoading);
					actions.push_back(Action);
					break;
				case AT_CHANGE_IMAGE:
					Action = new CActionChangeImage(NULL, file.GetArchive());
					Action->Serialize(file, IsLoading);
					actions.push_back(Action);
					break;
				case AT_CURVE:
					Action = new CActionCurve(NULL);
					Action->Serialize(file, IsLoading);
					actions.push_back(Action);
					break;
				}
				
			}

			_Actions[ObjName] = actions;
		}
	}
	else
	{
		//Сохранение
		file<<(DWORD)_Actions.size();
		for(TMapObjActions::iterator it = _Actions.begin();
			it != _Actions.end(); it++)
		{
			wstring ObjName = it->first;
			file<<ObjName;

			TActions actions = it->second;
			file<<(DWORD)actions.size();
			for(TActions::iterator iter = actions.begin(); iter != actions.end(); iter++)
			{
				CAction* act = (*iter);
				file<<(DWORD)act->GetType();
				act->Serialize(file, IsLoading);
			}
		}
	}
}

CAction * CActionMenager::AddAction(wstring Object, enActionType Type)
{
	//Добавить действие
	ATLASSERT(!Object.empty());
	
	
	//Ищем указанный объекта
	CObjectMenagerPtr mng;
	CObject* obj = mng->FindObject(Object);

	if(obj == NULL)
	{
		//Объект не найден
		ATLTRACE("Объект «%s» не найден.\n", Object.c_str());
		return FALSE;
	}

/*	if(FindAction(Object, Action->GetName()))
	{
		//Имена не могут совпадать
		ATLTRACE("Action типа %u для объекта «%s» уже присутствует.\n", Action->GetType(), Object.c_str());
		return FALSE;
	}
*/
	//Выделить память под действие
	CAction *act=NULL;
	switch(Type) 
	{
	case AT_MOVER:	act = new CActionMover(obj);	break;
	case AT_SIZER:	act = new CActionSizer(obj);	break;
	case AT_ROTATE:	act = new CActionRotate(obj);	break;
	case AT_VISUAL:	act = new CActionVisual(obj);	break;
	case AT_COLOR:	act = new CActionColor(obj);	break;
	case AT_PLAY_SOUND:	act = new CActionPlaySound(obj);	break;
	case AT_CHANGE_IMAGE: 
		if(_ArcManager)
			act = new CActionChangeImage(obj,_ArcManager);	
		break;
		
	case AT_CURVE: act = new CActionCurve(obj);	break;
	default: act = NULL;
	}
	if(act != NULL)
		_Actions[Object].push_back(act);
		
	return act;

	
}


BOOL CActionMenager::RemoveAll()
{
	//Удалить все действия
	for(TMapObjActions::iterator it = _Actions.begin(); it != _Actions.end(); it++)
	{
		TActions acts = it->second;
		if(!acts.empty())
		{
			for(TActions::iterator iter = acts.begin(); iter != acts.end(); iter++)
				delete *iter;
			acts.clear();
		}
	}
	_Actions.clear();
	return TRUE;
}

BOOL CActionMenager::RemoveObject(wstring Object)
{
	ATLASSERT(!Object.empty());

	//Ищем указанный объект
	CObjectMenagerPtr mng;
	CObject* obj = mng->FindObject(Object);

	if(obj == NULL)
	{
		ATLTRACE("Объект «%s» не найден.\n", Object.c_str());
		return FALSE;
	}

	//Пытаемся найти действия привязанные к этопу объекту
	TMapObjActions::iterator it = _Actions.find(Object);
	if(it == _Actions.end())
		return TRUE;


	//Удаляем все действия для заданного объекта
	TActions actions = it->second;
	for(TActions::iterator iter = actions.begin(); iter != actions.end(); iter++)
	{
		CAction* act = *iter;
		delete(act);
	}

	_Actions.erase(it);

	return TRUE;
}

BOOL CActionMenager::RemoveAction(wstring Object, const wstring &name)
{
	ATLASSERT(!Object.empty());


	//Ищем указанный объект
	CObjectMenagerPtr mng;
	CObject* obj = mng->FindObject(Object);

	if(obj == NULL)
	{
		ATLTRACE("Объект «%s» не найден.\n", Object.c_str());
		return FALSE;
	}

	//Ищем указанное действие
	CAction *act = FindAction(Object, name);
	if(act == NULL) 
		return FALSE;
	else
	{
		//Удалить действие
		_Actions[Object].remove(act);
		delete act;	
	}
	return TRUE;
}

CAction* CActionMenager::FindAction(const wstring &name)
{
	CAction * act=NULL;
	TMapObjActions::const_iterator ci;
	for(ci = _Actions.begin(); ci != _Actions.end();ci++)
	{
		if(act == FindAction(ci->first, name))
			return act;
	
	}
	return act;

}

CAction* CActionMenager::FindAction(wstring Object, const wstring &name)
{
	ATLASSERT(!Object.empty());

	
	//Ищем список действие привязанных к заданному объекту
	TMapObjActions::const_iterator ci = _Actions.find(Object);
	if(ci == _Actions.end())
		return NULL; //Ничего не найдено

	//Ищем действие заданного типа
	for(TActions::const_iterator i = ci->second.begin(); i != ci->second.end(); i++)
	{
		if((*i)->GetName() == name)
		{
			return *i;
		}
	}

	return NULL; //Ничего не найдено
}

BOOL CActionMenager::ModifyAction(wstring Object, CAction* Action)
{
	ATLASSERT(!Object.empty());
	ATLASSERT(Action != NULL);

	//Ищем указанное действие у заданного объекта
	CAction* act = FindAction(Object, Action->GetName());
	if(act == NULL)
		return FALSE;

	*act = *Action; //Копируем параметры
	return TRUE;
}

void CActionMenager::UpdateActions()
{
	//Вызываем Update у каждого действия
	for(TMapObjActions::const_iterator j = _Actions.begin(); j!= _Actions.end();j++)
	{
		for_each(j->second.begin(), j->second.end(), mem_fun(&CAction::Update));
	}
}

BOOL CActionMenager::GetAllActions(wstring Object, TActions& Actions)
{
	//Получить список действия заданного объекта
	TMapObjActions::iterator it =  _Actions.find(Object);
	if(it != _Actions.end())
	{
		Actions = it->second;
	}
	return TRUE;
}
CActionMenager::TActions CActionMenager::GetAllActions(wstring Object, enActionType Type)
{
	//Получить список действия заданного объекта
	TActions acts;
	TMapObjActions::iterator it =  _Actions.find(Object);
	TActions::iterator itt;
	if(it != _Actions.end())
	{
		
		for(itt=it->second.begin();itt!=it->second.end();++itt)
		{
			if((*itt)->GetType()==Type)
				acts.push_back(*itt);
		}
		
	}
	return acts;
}
