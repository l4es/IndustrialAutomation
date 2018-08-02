#pragma once
/********************************************************************
	Создан:	        2004/12/08
	Создан:	        8.12.2004   9:57
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\ObjectMenager.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Library
	Имя файла:	    ObjectMenager
	Расширение:	    h
	Автор(ы):	    Алексей, Михаил
    Класс(ы):       CObjectMenager
	
Описание:	Класс "Менеджер объектов" реализует операции по маршрутизации сообщений
между объектами. Хранению объектов и освобождению памяти если необходимо. Реализованы
функции для манипуляции с группой объектов (выделение).
*********************************************************************/

#include "Object.h"
#include "Selection.h"
#include "Track.h"

class CObjectMenager
	:public CSingleton<CObjectMenager>
{
friend class CArchiveManager;
public:
	//Добавить объект
	inline void AddObject(CObject* obj){ATLASSERT(obj != NULL);_Objects.push_front(obj);};
	//Удалить объект
	void RemoveObject(CObject* obj);
	//Удалить все объекты
	void RemoveAll();
	//Найти объект по имени
	CObject* FindObject(wstring Name);
	//Найти объекты заданного типа
	list<CObject*> FindObjects(enTypeID TypeID);
	//Получить список всех объектов
	list<CObject*> GetObjectsList(){return _Objects;};
	//Узнать выделен ли указанный объект
	bool IsSelected(CObject* obj);

	//Нарисовать все объекты
	inline void DrawObjects(HDC dc){for_each(_Objects.rbegin(),_Objects.rend(), bind2nd(mem_fun(&CObject::Draw), dc));};

	//Перевод из глобальных координат в экранные
	CPoint GlobalToScreen(sVector);
	//Перевод из экранных координат в глобальные
	sVector ScreenToGlobal(CPoint);
	//Получить имена всех объектов
	TStrings GetObjectNames();

	//////////////////////////////////////////////////////////////////////////
	// Обработка сообщений
	BOOL ProcessMouseLButtonDown(WPARAM wParam, CPoint lParam);		//WM_LBUTTONDOWN
	BOOL ProcessMouseRButtonDown(WPARAM wParam, CPoint lParam);		//WM_RBUTTONDOWN
	BOOL ProcessMouseLButtonDblClk(WPARAM wParam, CPoint lParam);	//WM_LBUTTONDBLCLK
	BOOL ProcessMouseLButtonUp(WPARAM wParam, CPoint lParam);		//WM_LBUTTONUP
	BOOL ProcessMouseMove(WPARAM wParam, CPoint lParam);			//WM_MOUSEMOVE
	BOOL ProcessKeyDown(WPARAM wParam, LPARAM lParam);				//WM_KEYDOWN
	//////////////////////////////////////////////////////////////////////////
	
	//Перенаправлять все сообщения от мышки в заданный объект
	void CaptureMouse(CObject* obj){_ObjCaptureMouse=obj;};
	//Вернуть маршрутизацию "мышиных" сообщений в нормальный режим
	void ReleaseCaptureMouse(){_ObjCaptureMouse=NULL;};
	//Получить объект, который принимает все "мышиные" сообщения
	CObject* GetCaptureMouse(){return _ObjCaptureMouse;};

	//Выбрать объекты в заданной области
	void SelectWindow(sVector LeftTop, sVector RightBottom);
	//Перещение объекта из группу выделенных
	void MoveToSelection(CObject* obj);
	//Перещение объекта из группы выделенных
	void MoveFromSelection(CObject* obj);
	//Перенести объекты из CSelection в CObjectMenager и очистить CSelection
	void ClearSelection(CSelection::enPosition Pos);
	//Присвоить CSelection новому объекту
	void SetSelection(CSelection* Selection){ delete _Selection;  _Selection = Selection; }
	//Получить указатель на CSelection
	CSelection* GetSelection(){ return _Selection; }
	//Получить указатель на выделенный объект
	CObject* GetSelected(){ return _Selected; }
	//Выбрать объект
	void SelectObject(CObject* obj);
	//Добавить объект к выделенным
	void AddToSelected(CObject* obj);	
	//Сгенерировать новое значение порядкового номера объекта (максимальное)
	DWORD GenerateOrderPos();
	//Установить порядковые значения объектов в соответствии с возрастанием
	//порядковых номеров (образуя непрерывную последовательность)
	void SetObjOrderPos();
	//Отсортировать объекты в соответствии с возрастанием порядковых номеров
	void SortObjects();

	struct sViewPort
	{
		sVector TopLeft,BottomRight;
	}m_ViewPort;	//Видимая область (область экрана в глобальных координатах)

protected:
	list<CObject*>	_Objects;			//Список объектов
	CObject*		_ObjCaptureMouse;	//Объект в который перенаправляются сообщения от мышки
	bool			_LMButtonDown;		//Флаг нажатия левой клавиши мышки
	sVector			_BeginPoint;		

	CSelection*		_Selection;
	CObject*		_Selected;			//Выделенный объект
	CTrack*			_Track;

protected:
	//Constructor/Destructor
	CObjectMenager(void);
	virtual ~CObjectMenager(void);
	DEF_SINGLETON(CObjectMenager);
};

typedef CSingletonPtr<CObjectMenager> CObjectMenagerPtr;