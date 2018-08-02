#include "common.h"
#include "Object.h"
#include ".\objectmenager.h"
#include "ActionMenager.h"

CAutoSingletonCreator<CObjectMenager> __ObjectMenager;

CObjectMenager::CObjectMenager(void)
{
	//Инициализация переменных
	m_ViewPort.BottomRight.x = 0;
	m_ViewPort.BottomRight.y = 0;
	m_ViewPort.TopLeft.x		= 0;
	m_ViewPort.TopLeft.y		= 0;
	_ObjCaptureMouse		= NULL;
	_Selected				= NULL;
	_LMButtonDown			= false;

	//Инициализация вспомогательных объектов
	_Selection = new CSelection();	//Группа объектов
	_Track = new CTrack();			//Рамка выделения
}

CObjectMenager::~CObjectMenager(void)
{
	//Снимаем выделение с выделенного объекта
	if(_Selected != NULL)
	{
		_Selected->OnLostFocus();
		_Selected = NULL;
	}

	if(_Selection != NULL)
	{
		_Selection->OnLostFocus();
		_Selection->ClearObject();
		RemoveObject(_Selection);
		delete _Selection;
		_Selection = NULL;
	}
	if(_Track != NULL)
		delete _Track;

	RemoveAll();
}

//Удалить объект
void CObjectMenager::RemoveObject(CObject* obj)
{
	//Если удаляемый объект выделен, снимаем с него выделение
	if(_Selected == obj)
	{
		_Selected->OnLostFocus();
		_Selected = NULL;
	}

	//Удаляем объект из списка
	_Objects.remove(obj);

	//Если нужно освобождать память - освобождаем
	if(obj->_AutoDelete)
		delete obj;
}

//Удалить все объекты
void CObjectMenager::RemoveAll()
{
	//Если есть выделенный объект, снимаем с него выделение
	if(_Selected)
	{
		if(_Selected == _Selection)
		{
			for(list<CObject*>::iterator i=_Selection->_Objects.begin();
				i!=_Selection->_Objects.end();i++) 
			{
				if((*i)->_AutoDelete)
					delete *i; 
			}
		}
		_Selected->OnLostFocus();
		_Selected = NULL;
	}

	//Если нужно освобождать память - освобождаем
	for(list<CObject*>::iterator i=_Objects.begin();i!=_Objects.end();i++) 
	{
		if((*i)->_AutoDelete)
			delete *i; 
	}
	//Удаляем все объекты из списка
	_Objects.clear();
}

//Перевод из глобальных координат в экранные
CPoint CObjectMenager::GlobalToScreen(sVector v)
{
	CPoint pt;
	pt.x = (LONG)(v.x - m_ViewPort.TopLeft.x);
	pt.y = (LONG)(v.y - m_ViewPort.TopLeft.y);
	return pt;
};

//Перевод из экранных координат в глобальные
sVector CObjectMenager::ScreenToGlobal(CPoint pt)
{
	sVector v;
	v.x = pt.x + m_ViewPort.TopLeft.x;
	v.y = pt.y + m_ViewPort.TopLeft.y;
	return v;
}

//Обработка/маршрутизация сообщения WM_LBUTTONDOWN
BOOL CObjectMenager::ProcessMouseLButtonDown(WPARAM wParam, CPoint lParam)
{
	_LMButtonDown = true;	//Флаг нажатия кнопки
	
	//Точка нажатия клавиши в глобальных координатах
	sVector point;
	point.x = m_ViewPort.TopLeft.x + lParam.x;
	point.y = m_ViewPort.TopLeft.y + lParam.y;

	typedef list<CObject*>::iterator it;

	//Если объект перехватил все "мышиные" сообщения - перенаправляем сообщение ему
	if(_ObjCaptureMouse)
		return _ObjCaptureMouse->OnMouseLButtonDown(point);

	//Обработка сообщения
	for(it i=_Objects.begin(); i!=_Objects.end(); i++)
	{
		//Определяем какой объект находится в точке нажатия
		if((*i)->HitTest(point))
		{
			if(wParam & MK_SHIFT)
			{
				//Зажат SHIFT. Добавляем объект к группе выделенных объектов
				if(*i == _Selection) return TRUE;
				AddToSelected(*i);
				return TRUE; //Сообщение обработано
			}
			else
			{
				//Выделяем объект
				SelectObject(*i);
				//Перенаправляем сообщение объекту
				if((*i)->OnMouseLButtonDown(point) == TRUE)
					return TRUE;
			}
		}
	}

	//Если ни один из объектов не обработал сообщение
	//Снимаем выделение
	SelectObject(NULL);

	//Добавляем объект "рамку"
	AddObject(_Track);
	//Точка начала рамки
	_BeginPoint = point;
	
	return FALSE; //Сообщение не обработано
}

//Обработка/маршрутизация сообщения WM_RBUTTONDOWN
BOOL CObjectMenager::ProcessMouseRButtonDown(WPARAM wParam, CPoint lParam)
{
	//Точка нажатия клавиши в глобальных координатах
	sVector point;
	point.x = m_ViewPort.TopLeft.x + lParam.x;
	point.y = m_ViewPort.TopLeft.y + lParam.y;

	//Если объект перехватил все "мышиные" сообщения - перенаправляем сообщение ему
	if(_ObjCaptureMouse)
		return _ObjCaptureMouse->OnMouseRButtonDown(point);

	typedef list<CObject*>::iterator it;
	//Обработка сообщения
	for(it i=_Objects.begin(); i!=_Objects.end(); i++)
	{
		//Определяем какой объект находится в точке нажатия
		if((*i)->HitTest(point))
		{
			//Перенаправляем сообщение объекту
			if((*i)->OnMouseRButtonDown(point) == TRUE)
				return TRUE;
		}
	}

	return FALSE; //Сообщение не обработано
}


BOOL CObjectMenager::ProcessMouseLButtonDblClk(WPARAM wParam, CPoint lParam)
{
	//Точка нажатия клавиши в глобальных координатах
	sVector point;
	point.x = m_ViewPort.TopLeft.x + lParam.x;
	point.y = m_ViewPort.TopLeft.y + lParam.y;

	//Если объект перехватил все "мышиные" сообщения - перенаправляем сообщение ему
	if(_ObjCaptureMouse)
		return _ObjCaptureMouse->OnMouseLButtonDblClk(point);

	typedef list<CObject*>::iterator it;
	//Обработка сообщения
	for(it i=_Objects.begin(); i!=_Objects.end(); i++)
	{
		//Определяем какой объект находится в точке нажатия
		if((*i)->HitTest(point))
		{
			//Перенаправляем сообщение объекту
			if((*i)->OnMouseLButtonDblClk(point) == TRUE)
				return TRUE;
		}
	}
	return FALSE; //Сообщение не обработано
}

BOOL CObjectMenager::ProcessKeyDown(WPARAM wParam, LPARAM lParam)
{
	//Если объект перехватил все "мышиные" сообщения - перенаправляем сообщение ему
	if(_ObjCaptureMouse)
		return _ObjCaptureMouse->OnKeyDown(wParam, lParam);

	//Обработка сообщения
	typedef list<CObject*>::iterator it;
	for(it i=_Objects.begin(); i!=_Objects.end(); i++)
	{
		//Если объект выделен перенаправляем ему сообщение
		if(_Selected != NULL && *i == _Selected)
		{
			UINT key = (UINT)wParam;
		// Если нажали Del удаляем объект
			if (key == 46)
			{
				if( *i == _Selection)
				{
					CActionMenagerPtr act;
					for(list<CObject*>::iterator it = _Selection->_Objects.begin();
						it != _Selection->_Objects.end(); it++)
					{
						act->RemoveObject((*it)->GetName());
					}

					for(list<CObject*>::iterator it = _Selection->_Objects.begin();
						it != _Selection->_Objects.end(); it++)
					{
						delete *it;
					}

					_Selection->_Objects.clear();

					RemoveObject(*i);
				}
				else
				{
					CActionMenagerPtr act;
					act->RemoveObject((*i)->GetName());
					RemoveObject(*i);
				}
				SortObjects();
				SetObjOrderPos();
				return TRUE;
			}
			else
				if((*i)->OnKeyDown(wParam, lParam) == TRUE)
					return TRUE;
		}
	}
	return FALSE;
}


BOOL CObjectMenager::ProcessMouseLButtonUp(WPARAM wParam, CPoint lParam)
{
	_LMButtonDown = false;	//Флаг нажатия кнопки

	//Точка нажатия клавиши в глобальных координатах
	sVector point;
	point.x = m_ViewPort.TopLeft.x + lParam.x;
	point.y = m_ViewPort.TopLeft.y + lParam.y;

	//Если объект перехватил все "мышиные" сообщения - перенаправляем сообщение ему
	if(_ObjCaptureMouse)
		_ObjCaptureMouse->OnMouseLButtonUp(point);

	//Обработка сообщения
	typedef list<CObject*>::iterator it;
	for(it i=_Objects.begin(); i!=_Objects.end(); i++)
	{
		//Определяем какой объект находится в точке нажатия
		if((*i)->HitTest(point))
		{
			//Перенаправляем сообщение объекту
			if((*i)->OnMouseLButtonUp(point) == TRUE)
				return TRUE;
		}
	}

	if(_Selected == NULL)
		SelectWindow(_Track->GetPosition(), _Track->GetPosition() + _Track->GetSize());

	_Track->SetVertex(sVector(0,0), sVector(0,0));
	RemoveObject(_Track);

	return FALSE;
}

BOOL CObjectMenager::ProcessMouseMove(WPARAM wParam, CPoint lParam)
{
	sVector point;
	point.x = m_ViewPort.TopLeft.x + lParam.x;
	point.y = m_ViewPort.TopLeft.y + lParam.y;

	if(_ObjCaptureMouse)
	{
		 _ObjCaptureMouse->OnMouseMove(point);
	}

	typedef list<CObject*>::iterator it;
	for(it i=_Objects.begin(); i!=_Objects.end(); i++)
	{
		if((*i)->HitTest(point))
		{
			if((*i)->OnMouseMove(point) == TRUE)
				return TRUE;
		}
	}

	if(_LMButtonDown)
	{
		_Track->SetVertex(_BeginPoint, point);
	}

	return FALSE;
}

void CObjectMenager::SelectObject(CObject* obj)
{
	if(obj == _Selection)return;
	if(_Selection != NULL)
	{
		ClearSelection(_Selection->_OrderPos);
	}

	if(_Selected != obj)
	{
		if(_Selected != NULL)
			_Selected->OnLostFocus();
		
		_Selected = obj;
		if(_Selected != NULL)
			_Selected->OnSetFocus();
	}
}

void CObjectMenager::AddToSelected(CObject* obj)
{
	if(_Selected != _Selection)
	{
		AddObject(_Selection);
		if(_Selected != NULL)
		{
			_Selection->AddObject(_Selected);
			_Selected->OnLostFocus();
			_Objects.remove(_Selected);
		}
		_Selected = _Selection;
		_Selection->OnSetFocus();
	}
	
	MoveToSelection(obj);
}

void CObjectMenager::MoveToSelection(CObject* obj)
{
	if(_Selected == obj)
		_Selected->OnLostFocus();
	_Selection->AddObject(obj);
	_Selection->OnSetFocus();
	_Objects.remove(obj);
}

void CObjectMenager::MoveFromSelection(CObject* obj)
{
	if(!_Selection->IsEmpty())
	{
		_Selection->Convert(obj);
		_Objects.push_back(obj);
		_Selection->RemoveObject(obj);
	}
	_Objects.sort(sCompOrderPos());
}

//Перенести объекты из CSelection в CObjectMenager и очистить CSelection
void CObjectMenager::ClearSelection(CSelection::enPosition Pos)
{
	if(!_Selection->_Objects.empty())
	{
		{
			switch(Pos) 
			{
				//Если порядковая позиция CSelection - на передний план - 
				//добавляем к началу списка
			case CSelection::POS_FRONT: 
				{
					for(list<CObject*>::reverse_iterator it = _Selection->_Objects.rbegin();
						it != _Selection->_Objects.rend(); it++)
					{
						_Selection->Convert(*it);
						_Objects.push_front(*it);
					}
				}
				break;
				//Если порядковая позиция CSelection - на задний план - 
				//добавляем к началу списка
			case CSelection::POS_BACK:
				{
					for(list<CObject*>::iterator it = _Selection->_Objects.begin();
						it != _Selection->_Objects.end(); it++)
					{
						_Selection->Convert(*it);
						_Objects.push_back(*it);
					}
				}
				break;
			default:
				{
					for(list<CObject*>::iterator it = _Selection->_Objects.begin();
						it != _Selection->_Objects.end(); it++)
					{
						_Selection->Convert(*it);
						_Objects.push_back(*it);
					}
				}
			}
		}
		//Удаляем все объекты из CSelection
		_Selection->ClearObject();
	}

	_Selection->OnLostFocus();
	RemoveObject(_Selection);

	//Если добавили к началу или к концу списка устанавливаем новые 
	//значения порядковых позиций для всех объектов
	if(Pos == CSelection::POS_FRONT || Pos == CSelection::POS_BACK)
	{
		SetObjOrderPos();
	}
	//сортировка объектов в соответствии с порядковыми номерами
	_Objects.sort(sCompOrderPos());
}


//Выбор объектов с помощью рамки
void CObjectMenager::SelectWindow(sVector LeftTop, sVector RightBottom)
{
	list< CObject* > tmp;
	typedef list< CObject* >::iterator Iter;
	for(Iter it = _Objects.begin(); it != _Objects.end(); it++)
	{
		sVector pos = (*it)->GetPosition();
		sVector size = (*it)->GetSize();
		bool InWindow = false;
		//Если объект полностью попадает в пределы рамки...
		if((LeftTop.x < RightBottom.x) && (LeftTop.y < RightBottom.y))
			InWindow = ((pos.x > LeftTop.x) && (pos.y > LeftTop.y) &&
						(pos.x + size.x < RightBottom.x) && (pos.y + size.y < RightBottom.y));
		//...то добавляем его в выбранные
		if(InWindow)
		{
			tmp.push_back(*it);
		}
	}

	if(!tmp.empty())
	{
		//если только один объект попал в рамку, выделяем его
		if(tmp.size() == 1)
		{
			SelectObject(*(tmp.begin()));
		}
		else
			//если более одного объекта попало в рамку, добавляем их в CSelection
		  for(Iter it = tmp.begin(); it != tmp.end(); it++)
			AddToSelected(*it);
	}
}

//Найти объект по имени
CObject* CObjectMenager::FindObject(wstring Name)
{
	list<CObject*>::iterator it = 
		find_if(_Objects.begin(), _Objects.end(), sFindByName(Name));
	
	if(it !=  _Objects.end())
	{
		return *it;
	};

	return NULL;
}

//Найти объекты заданного типа
list<CObject*> CObjectMenager::FindObjects(enTypeID TypeID)
{
	list<CObject*> tmp;
	if(!_Objects.empty())
	{
		for(list<CObject*>::iterator it = _Objects.begin();
			it != _Objects.end(); it++)
		{
			if((*it)->GetTypeID() == (UINT)TypeID)
			{
				tmp.push_back(*it);
			}
		}
	}
	return tmp;
}

//Узнать выделен ли указанный объект
bool CObjectMenager::IsSelected(CObject* obj)
{
	typedef list<CObject*>::iterator it;
	for(it i=_Selection->_Objects.begin();i!=_Selection->_Objects.end();i++)
	{
		if(*i == obj)
			return true;
	}
	return obj == _Selected;
}

//Получить имена всех объектов
TStrings CObjectMenager::GetObjectNames()
{
	TStrings Names;
	for(list<CObject*>::iterator it = _Objects.begin(); it != _Objects.end(); it++)
	{
		if((*it)->GetTypeID() != _SELECTION)
			Names.push_back((*it)->GetName());
	}
	return Names;
}

//Сгенерировать новое значение порядкового номера объекта (максимальное)
DWORD CObjectMenager::GenerateOrderPos()
{
	DWORD Pos = 1;
	for(list<CObject*>::iterator it = _Objects.begin(); it != _Objects.end(); it++)
	{
		if((*it)->GetOrderPos() >= Pos)
			Pos = (*it)->GetOrderPos()+1;
	}
	return Pos;
}

//Установить порядковые значения объектов в соответствии с возрастанием
//порядковых номеров (образуя непрерывную последовательность)
void CObjectMenager::SetObjOrderPos()
{
	DWORD Pos = 1;
	for(list<CObject*>::reverse_iterator it = _Objects.rbegin(); it != _Objects.rend(); it++, Pos++)
	{
		(*it)->SetOrderPos(Pos);
	}
}

//Отсортировать объекты в соответствии с возрастанием порядковых номеров
void CObjectMenager::SortObjects()
{
	_Objects.sort(sCompOrderPos());
}

