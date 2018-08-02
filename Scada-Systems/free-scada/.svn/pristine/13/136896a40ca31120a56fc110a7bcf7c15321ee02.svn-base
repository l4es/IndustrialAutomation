#include "StdAfx.h"
#include ".\select.h"

namespace Primitives
{
	CSelect::CSelect(void)
	{
	}

	CSelect::~CSelect(void)
	{
	}

	//Обработчик нажатия левой кнопки мыши
	BOOL CSelect::OnMouseLButtonDown(sVector Point)
	{
		return CBaseObject<CSelect>::OnMouseLButtonDown(Point);
	}

	//Обработчик нажатия правой кнопки мыши
	BOOL CSelect::OnMouseRButtonDown(sVector Point)
	{
		return CBaseObject<CSelect>::OnMouseRButtonDown(Point);
	}

	//Обработчик отпускания левой кнопки мыши
	BOOL CSelect::OnMouseLButtonUp(sVector Point)
	{
		return CBaseObject<CSelect>::OnMouseLButtonUp(Point);
	}

	//Обработчик перемещения мыши
	BOOL CSelect::OnMouseMove(sVector Point)
	{
		return CBaseObject<CSelect>::OnMouseMove(Point);
	}

	//Обработчик изменения параметров объекта
	void CSelect::OnEdit()
	{
		CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
		//Обновление параметров объекта в окне свойств 
		prop->RefreshProperty();
		prop->FreeInst();
	}

	//обработчик двойного нажатия левой кнопки мыши
	BOOL CSelect::OnMouseLButtonDblClk(sVector Point)
	{
		return CBaseObject<CSelect>::OnMouseLButtonDblClk(Point);
	}

	//обработчик нажатия клавиши клавиатуры
	BOOL CSelect::OnKeyDown(WPARAM wParam, LPARAM lParam)
	{
		return CBaseObject<CSelect>::OnKeyDown(wParam, lParam);
	}

	//Рисование Selection
	void CSelect::Draw(HDC dc)
	{
		CSelection::Draw(dc);
		CBaseObject<CSelect>::Draw(dc);
	}

	//Добавление объекта в Selection
	void CSelect::AddObject(CObject* obj)
	{
		//Приводим объект к CBase
		CBase* Base = GetBase(obj);
		//Если в Selection нет ни одного объекта, заполняем карту 
		//категория-список sCtrlParam параметрами этого объекта
		if(_Objects.empty())
		{
			_Categories.clear();
			TCategoryList CategoryList = Base->GetCategories();
			for(TCategoryListIter it = CategoryList.begin(); it != CategoryList.end(); it++)
			{
				_Categories[*it] = Base->GetParameters(*it);
			}
			 
			CSelection::AddObject(obj);
			return;
		}


		TCategoryList ObjCategoryList = Base->GetCategories();
		TCategoryList CategoryList = GetCategories();
		TCategoryList tmp;


		//Формируем список из категорий, которые есть и в Selection, и в объекте
		for(TCategoryListIter it = ObjCategoryList.begin(); it != ObjCategoryList.end(); it++)
		{
			TCategoryListIter find = find_if(CategoryList.begin(), CategoryList.end(), sFindCategory(*it));
			if(find != CategoryList.end()) tmp.push_back(*find);
		}
		CategoryList = tmp;

		TCategoryMap CategoryMap;

		//Формируем карту категория-список sCtrlParam из категорий и параметром, 
		//которые есть и в Selection, и в объекте

		for(TCategoryListIter it = CategoryList.begin(); it != CategoryList.end(); it++)
		{
			TCtrlParamList ParamList = GetParameters(*it);
			TCtrlParamList ObjParamList = Base->GetParameters(*it);
			TCtrlParamList temp;
			for(TCtrlParamListIter iter = ObjParamList.begin(); iter != ObjParamList.end(); iter++)
			{
				TCtrlParamListIter find = find_if(ParamList.begin(), ParamList.end(), sFindCtrlParam(*iter));
				if(find != ParamList.end())
				{
					if((*it == CAT_EMPTY) && (find->Param == L"Name"))
						continue;
					temp.push_back(*find);
				}
			}
			CategoryMap[*it] = temp;
		}
		_Categories.clear();
        _Categories = CategoryMap;
		CSelection::AddObject(obj);
	}

	//Обработчик получения объектом фокуса
	void CSelect::OnSetFocus()
	{
		CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
		//Заполняем окно свойств параметрами объекта
		prop->SetObject(this);
		prop->FreeInst();
		CBaseObject<CSelect>::OnSetFocus();
	}


	void CSelect::OnLostFocus()
	{
		_Categories.clear();
		CBaseObject<CSelect>::OnLostFocus();
	}

	//Получить значение параметра
	bool CSelect::GetParamValue(enCatID Category, wstring Name, sParam& Param )
	{
		//Отображаем параметры первого объекта из Selection
		CBase* Base = GetBase(*_Objects.begin());
		TCategoryMapIter MapIter = _Categories.find(Category);
		if(MapIter != _Categories.end())
		{
			TCtrlParamList ParamList = MapIter->second;
			TCtrlParamListIter ParamIter = find_if(ParamList.begin(), ParamList.end(), sFindCtrl(Name));
			if(ParamIter != ParamList.end())
			{
				//Преобразуем координаты из относительных в абсолютные
				RelateToAbsolute(Base->GetObject());
				//Получаем значение параметра
				Base->GetParamValue(Category, Name, Param);
				//Преобразуем координаты из абсолютных в относительные
				AbsoluteToRelate(Base->GetObject());
			}
		}
		return true;
	}

	//Установить значение параметра
	bool CSelect::SetParamValue(enCatID Category, wstring Name, sParam Param)
	{
		//Устанавливаем новое значение параметра для всех объектов в Selection
		for(list<CObject*>::iterator it = _Objects.begin(); it != _Objects.end(); it++)
		{
			CBase* Base = GetBase(*it);
			//Преобразуем из относительных координат в абсолютные
			RelateToAbsolute(Base->GetObject());
			//Устанавливаем новое значение параметра
			Base->SetParamValue(Category, Name, Param);
			//Преобразуем из абсолютных в относительные координаты
			AbsoluteToRelate(Base->GetObject());
		}
		//Пересчет размера и позиции Selection
		CalcSizeEndPos();
		return false;
	}
}
