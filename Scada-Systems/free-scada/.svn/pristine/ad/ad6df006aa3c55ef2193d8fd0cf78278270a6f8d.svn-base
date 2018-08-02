#pragma once

/********************************************************************
Создан:	        2005/02/02  
Создан:	        2.2.2005   13:12
Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Designer\Primitives\BaseObject.h
Директория:	    d:\Work\Projects\М-Немо\Source\Designer\Primitives
Имя файла:	    BaseObject
Расширение:	    h
Автор(ы):	    Алексей
Класс(ы):       CBase, CBaseObject

Описание:	Базовые классы для примитивов
*********************************************************************/

#include "Manipulator.h"
#include "ManipulatorMover.h"
#include "ManipulatorSizeLeft.h"
#include "ManipulatorSizeRight.h"
#include "ManipulatorSizeTop.h"
#include "ManipulatorSizeBottom.h"
#include "ManipulatorRotate.h"

namespace Primitives
{
	//Макросы для реализации функции GetParamValue
	#define BEGIN_GET_PROP_MAP(Cat, Name, Param)	{enCatID __c = Cat; wstring __n = Name; sParam& __p = Param;
	#define GET_PROP_STRING(Cat, Name, Method)	if(__c == Cat && Name == __n){__p.Type = T_STRING; __p.SetString(##Method);}
	#define GET_PROP_FLOAT(Cat, Name, Method)	if(__c == Cat && Name == __n){__p.Type = T_FLOAT; __p.SetFloat(##Method);}
	#define GET_PROP_INT(Cat, Name, Method)		if(__c == Cat && Name == __n){__p.Type = T_INTEGER; __p.SetInt(##Method);}
	#define GET_PROP_BOOL(Cat, Name, Method)	if(__c == Cat && Name == __n){__p.Type = T_BOOLEAN; __p.SetBool(##Method);}
	#define GET_PROP_RGB(Cat, Name, Method)		if(__c == Cat && Name == __n){__p.Type = T_RGB; __p.SetRGB(##Method);}
	#define END_GET_PROP_MAP()	}

	//Макросы для реализации функции SetParamValue
	#define BEGIN_SET_PROP_MAP(Cat, Name, Param)	{enCatID __c = Cat; wstring __n = Name;	sParam& __p = Param;
	#define SET_PROP_STRING(Cat, Name, Method)	if(__c == Cat && Name == __n){wstring val;	if(__p.GetString(val)){##Method; return true;}}
	#define SET_PROP_FLOAT(Cat, Name, Method)	if(__c == Cat && Name == __n){float val;	if(__p.GetFloat(val)){##Method; return true;}}
	#define SET_PROP_INT(Cat, Name, Method)		if(__c == Cat && Name == __n){int val;		if(__p.GetInt(val)){##Method; return true;}}
	#define SET_PROP_BOOL(Cat, Name, Method)	if(__c == Cat && Name == __n){bool val;		if(__p.GetBool(val)){##Method; return true;}}
	#define SET_PROP_RGB(Cat, Name, Method)		if(__c == Cat && Name == __n){sRGB val; 	if(__p.GetRGB(val)){##Method; return true;}}
	#define END_SET_PROP_MAP() }

	//Базовый класс (интерфейс) для примитивов
	class CBase
	{
	public:
		//Получить список всех категорий для данного объекта
		virtual TCategoryList GetCategories() = 0;
		//Получить список параметров для определенной категории
		virtual TCtrlParamList GetParameters(enCatID Category) = 0;
		//Получить значение параметра с категорией Category и названием Name
		virtual bool GetParamValue(enCatID Category, wstring Name, sParam& Param) = 0;
		//Установить значение параметра с категорией Category и названием Name
		virtual bool SetParamValue(enCatID Category, wstring Name, sParam Param) = 0;
		//Преобразовать объект к CObject*
		virtual CObject* GetObject() = 0;
		//Получить строковое значение категории
		virtual wstring StrFromCatID(enCatID id) = 0;
	};

	//Реализация CBase
	template<class T>
	class CBaseObject
		: public CBase
	{
	public:
		//Манипуляторы
		typedef list<CManipulator*> TManipulators;
		virtual ~CBaseObject(void){};

		CBaseObject(void)
		{
			//Заполняем карту категория-список sCtrlParam(имя-тип контрола)
			TCtrlParamList Empty;
			Empty.push_back(sCtrlParam(CT_TEXT, L"Name"));
			_Categories[CAT_EMPTY] = Empty;

			TCtrlParamList Geometry;
			Geometry.push_back(sCtrlParam(CT_FLOAT, L"Position X"));
			Geometry.push_back(sCtrlParam(CT_FLOAT, L"Position Y"));
			Geometry.push_back(sCtrlParam(CT_FLOAT, L"Width"));
			Geometry.push_back(sCtrlParam(CT_FLOAT, L"Height"));
			Geometry.push_back(sCtrlParam(CT_FLOAT, L"Angle"));
			_Categories[CAT_GEOMETRY] = Geometry;

			TCtrlParamList Visual;
			Visual.push_back(sCtrlParam(CT_FLAG, L"Visible"));
			Visual.push_back(sCtrlParam(CT_COLOR, L"Color"));
			_Categories[CAT_APPEARANCE] = Visual;	
			
			//для каждой категории сортируем список sCtrlParam
			for(TCategoryMapIter it = _Categories.begin(); it != _Categories.end(); it++)
				it->second.sort(sCompareName());
		}

		inline void RemoveParam(enCatID Category, LPCTSTR ParamName)
		{
			TCategoryMapIter CatIt = _Categories.find(Category);
			if(CatIt != _Categories.end())
			{
				TCtrlParamListIter it = find_if(CatIt->second.begin(), CatIt->second.end(), sFindCtrl(ParamName));
				if(it != CatIt->second.end())
					CatIt->second.erase(it);
			}
		};

		//Получить список всех категорий для данного объекта
		virtual TCategoryList GetCategories()
		{
			TCategoryList tmp;
			if(_Categories.empty()) return tmp;
			typedef TCategoryMap::iterator it;
			for(it i = _Categories.begin(); i != _Categories.end(); i++)
			{
				tmp.push_back(i->first);
			}
			tmp.sort();
			return tmp;
		}
		
		//Получить список параметров для определенной категории
		virtual TCtrlParamList GetParameters(enCatID Category)
		{
			TCtrlParamList tmp;
			
			TCategoryMapIter CategoryIter = _Categories.find(Category);
			if((CategoryIter == _Categories.end()) || (CategoryIter->second.empty()))
				return tmp;
			tmp = CategoryIter->second;

			return tmp;
		}

		//Получить значение параметра с категорией Category и названием Name
		virtual bool GetParamValue(enCatID Category, wstring Name, sParam& Param )
		{
			BEGIN_GET_PROP_MAP(Category, Name, Param)
				GET_PROP_STRING(CAT_EMPTY, L"Name", static_cast<T*>(this)->GetName().c_str())
				GET_PROP_FLOAT(CAT_GEOMETRY, L"Position X", static_cast<T*>(this)->GetPosition().x)
				GET_PROP_FLOAT(CAT_GEOMETRY, L"Position Y", static_cast<T*>(this)->GetPosition().y)
				GET_PROP_FLOAT(CAT_GEOMETRY, L"Width", static_cast<T*>(this)->GetSize().x)
				GET_PROP_FLOAT(CAT_GEOMETRY, L"Height", static_cast<T*>(this)->GetSize().y)
				GET_PROP_FLOAT(CAT_GEOMETRY, L"Angle", RadToDeg(static_cast<T*>(this)->GetAngle()))
				GET_PROP_BOOL(CAT_APPEARANCE, L"Visible", static_cast<T*>(this)->IsVisible())
				GET_PROP_RGB(CAT_APPEARANCE, L"Color", static_cast<T*>(this)->GetColor())
			END_GET_PROP_MAP()
			return true;
		}

		//Установить значение параметра с категорией Category и названием Name
		virtual bool SetParamValue(enCatID Category, wstring Name, sParam Param)
		{
			BEGIN_SET_PROP_MAP(Category, Name, Param)
				SET_PROP_STRING(CAT_EMPTY, L"Name", static_cast<T*>(this)->SetName(val))
				SET_PROP_FLOAT(CAT_GEOMETRY, L"Position X", static_cast<T*>(this)->MoveTo(val, static_cast<T*>(this)->GetPosition().y))
				SET_PROP_FLOAT(CAT_GEOMETRY, L"Position Y", static_cast<T*>(this)->MoveTo(static_cast<T*>(this)->GetPosition().x, val))
				SET_PROP_FLOAT(CAT_GEOMETRY, L"Width", static_cast<T*>(this)->SetSize(val, static_cast<T*>(this)->GetSize().y))
				SET_PROP_FLOAT(CAT_GEOMETRY, L"Height", static_cast<T*>(this)->SetSize(static_cast<T*>(this)->GetSize().x, val))
				SET_PROP_FLOAT(CAT_GEOMETRY, L"Angle", static_cast<T*>(this)->Rotate(DegToRad(val)))
				SET_PROP_BOOL(CAT_APPEARANCE, L"Visible", static_cast<T*>(this)->SetVisible(val))
				SET_PROP_RGB(CAT_APPEARANCE, L"Color", static_cast<T*>(this)->SetColor(val))
			END_SET_PROP_MAP()
			return false;
		}
		
		//Преобразовать объект к CObject*
		virtual CObject* GetObject()
		{
			return static_cast<T*>(this);
		}

		//Обработчик нажания левой кнопки мыши 
		virtual BOOL OnMouseLButtonDown(sVector Point)
		{
			typedef TManipulators::iterator it;
			for(it i=_Manipulators.begin(); i!=_Manipulators.end();i++)
			{
				if((*i)->HitTest(Point))
					if((*i)->OnMouseLButtonDown(Point))
						return TRUE;
			}
			return FALSE;
		}

		//Обработчик нажания правой кнопки мыши 
		virtual BOOL OnMouseRButtonDown(sVector Point)
		{
			typedef TManipulators::iterator it;
			for(it i=_Manipulators.begin(); i!=_Manipulators.end();i++)
			{
				if((*i)->HitTest(Point))
					if((*i)->OnMouseRButtonDown(Point))
						return TRUE;
			}
			return FALSE;
		}

		//Обработчик отпускания левой кнопки мыши 
		virtual BOOL OnMouseLButtonUp(sVector Point)
		{
			typedef TManipulators::iterator it;
			for(it i=_Manipulators.begin(); i!=_Manipulators.end();i++)
			{
				if((*i)->HitTest(Point))
					if((*i)->OnMouseLButtonUp(Point))
						return TRUE;
			}
			return FALSE;
		}

		//Обработчик перемещения мыши 
		virtual BOOL OnMouseMove(sVector Point)
		{
			typedef TManipulators::iterator it;
			for(it i=_Manipulators.begin(); i!=_Manipulators.end();i++)
			{
				if((*i)->HitTest(Point))
					if((*i)->OnMouseMove(Point))
						return TRUE;
			}
			return FALSE;
		}

		//Обработчик двойного нажатия левой кнопки мыши 
		virtual BOOL OnMouseLButtonDblClk(sVector Point)
		{
			return FALSE;
		}

		//Обработчик нажатия клавиши клавиатуры
		virtual BOOL OnKeyDown(WPARAM wParam, LPARAM lParam)
		{
			return FALSE;
		}

		//Обработчик получения объектом фокуса
		virtual void OnSetFocus()
		{
			CObject* obj = GetObject();

			CManipulatorMover		*move	= new CManipulatorMover(obj);
			CManipulatorSizeLeft	*left	= new CManipulatorSizeLeft(obj);
			CManipulatorSizeRight	*right	= new CManipulatorSizeRight(obj);
			CManipulatorSizeTop		*top	= new CManipulatorSizeTop(obj);
			CManipulatorSizeBottom	*bottom = new CManipulatorSizeBottom(obj);
			CManipulatorRotate		*rotate = new CManipulatorRotate(obj);

			_Manipulators.push_front(move);
			_Manipulators.push_front(left);
			_Manipulators.push_front(right);
			_Manipulators.push_front(top);
			_Manipulators.push_front(bottom);
			_Manipulators.push_front(rotate);
		}

		//Обработчик события потери фокуса объектом
		virtual void OnLostFocus()
		{
			CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
			prop->SaveObjectValue();
			prop->FreeInst();

			typedef TManipulators::iterator it;
			for(it i=_Manipulators.begin();i!=_Manipulators.end();i++)
			{
				delete *i;
			}
			_Manipulators.clear();
		}

		//Проверка попадает ли point на объект
		virtual bool HitTest(sVector point)
		{
			typedef TManipulators::iterator it;
			for(it i=_Manipulators.begin();i!=_Manipulators.end();i++)
			{
				if((*i)->HitTest(point))
					return TRUE;
			}

			sVector pos		= GetObject()->GetPosition();
			sVector size	= GetObject()->GetSize();
			sVector center	= size/2;

			sVector points[4];
			points[0] = sVector(-center.x, -center.y);
			points[1] = sVector(+center.x, -center.y);
			points[2] = sVector(+center.x, +center.y);
			points[3] = sVector(-center.x, +center.y);

			for(int i=0;i<4;i++)
			{
				points[i].Rotate(GetObject()->GetAngle());
				points[i] += pos+center;
			}

			CGraphic graph(NULL);
			return graph.TestPtInRect(point, points);
		}

		//Рисование объекта
		virtual void Draw(HDC dc)
		{
			//Прорисовка манипуляторов
			for_each(_Manipulators.begin(),_Manipulators.end(), bind2nd(mem_fun(&CObject::Draw), dc));

		}

		//Получить строковое значение категории
		virtual wstring StrFromCatID(enCatID id)
		{
			wstring str;
			switch(id) 
			{
			case CAT_EMPTY:			str = L"";break;
			case CAT_GEOMETRY:		str = L"Geometry"; 	break;
			case CAT_APPEARANCE:	str = L"Appearance"; break;
			case CAT_FONT:			str = L"Font";  break;
			case CAT_DATA:		    str = L"Data"; break;
			}
			return str;
		}


	protected:
		//Карта Категория-список(Имя параметра, имя контрола)
		TCategoryMap _Categories;

		//Манипуляторы
		TManipulators	_Manipulators; 

		 
	};
}