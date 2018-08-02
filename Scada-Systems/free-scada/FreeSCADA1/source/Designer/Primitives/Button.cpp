#include "StdAfx.h"
#include "../MainFrm.h"
#include "BaseObject.h"
#include ".\button.h"

namespace Primitives
{
	CButton::CButton(void)
	{
		//Дополняем карту категория-список sCtrlParam(имя-тип контрола)
		TCtrlParamList Data;
		Data.push_back(sCtrlParam(CT_VARIABLE, L"Variable"));
		Data.sort(sCompareName());
		_Categories[CAT_DATA] = Data;

		TCtrlParamList Font;
		Font.push_back(sCtrlParam(CT_FONT,	L"Font"));
		Font.push_back(sCtrlParam(CT_INT,	L"Size"));
		Font.push_back(sCtrlParam(CT_FLAG,	L"Bold"));
		Font.push_back(sCtrlParam(CT_FLAG,	L"Italic"));
		Font.push_back(sCtrlParam(CT_FLAG,	L"Strikethrough"));
		Font.push_back(sCtrlParam(CT_FLAG,	L"Underline"));
		Font.sort(sCompareName());
		_Categories[CAT_FONT] = Font;

		//Удаляем из карты параметр Угол
		TCategoryMapIter it = _Categories.find(CAT_GEOMETRY);
		if(it !=  _Categories.end())
		{
			TCtrlParamList& Geometry = it->second;
			TCtrlParamListIter iter = find_if(Geometry.begin(), Geometry.end(), sFindCtrl(L"Angle"));
			if(iter != Geometry.end())
			{
				Geometry.erase(iter);
			}
		}

		it = _Categories.find(CAT_APPEARANCE);
		if(it !=  _Categories.end())
		{
			TCtrlParamList& Appearance = it->second;
			Appearance.push_back(sCtrlParam(CT_TEXT,	L"Text"));
		}
	}

	CButton::~CButton(void)
	{
	}
	
	//Рисование кнопки
	void CButton::Draw(HDC dc_) 
	{
		const bool Visible = IsVisible();
		//В дизайнере объект всегда виден
		SetVisible(true);

		const wstring VariableName = GetControlVariable();
		SetControlVariable(L"");
		_IsDown = false;

		CObjButton::Draw(dc_);
		for_each(_Manipulators.begin(),_Manipulators.end(), bind2nd(mem_fun(&CObject::Draw), dc_));

		//Восстанавливаем значения видимости и переменной
		SetVisible(Visible);
		SetControlVariable(VariableName);
	}

	//Обработчик получения объектом фокуса
	void CButton::OnSetFocus()
	{
		CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
		//Отображение параметров в окне свойств и окне действий
		prop->SetObject(this);
		prop->FreeInst();

		CObjButton::OnSetFocus();

		CObject* obj = GetObject();

		CManipulatorMover		*move	= new CManipulatorMover(obj);
		CManipulatorSizeLeft	*left	= new CManipulatorSizeLeft(obj);
		CManipulatorSizeRight	*right	= new CManipulatorSizeRight(obj);
		CManipulatorSizeTop		*top	= new CManipulatorSizeTop(obj);
		CManipulatorSizeBottom	*bottom = new CManipulatorSizeBottom(obj);

		_Manipulators.push_front(move);
		_Manipulators.push_front(left);
		_Manipulators.push_front(right);
		_Manipulators.push_front(top);
		_Manipulators.push_front(bottom);


	}

	//Обработчик потери фокуса объектом
	void CButton::OnLostFocus()
	{
		CObjButton::OnLostFocus();
		CBaseObject<CButton>::OnLostFocus();
	}


	//Событие изменение объекта
	void CButton::OnEdit()
	{
		CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
		//Обновление содержимого окна свойств
		prop->RefreshProperty();
		prop->FreeInst();
		return CObjButton::OnEdit();
	}



	//Сохранение-загрузка объекта
	void CButton::Serialize(CMemFile& file, BOOL IsLoading)
	{
		CObjButton::Serialize(file, IsLoading);
	}

	//Получить значение параметра с категорией Category и названием Name
	bool CButton::GetParamValue(enCatID Category, wstring Name, sParam& Param)
	{
		BEGIN_GET_PROP_MAP(Category, Name, Param)
			GET_PROP_STRING(CAT_FONT,	L"Font",		GetFontName().c_str())
			GET_PROP_INT(CAT_FONT,		L"Size",		GetHeight())
			GET_PROP_BOOL(CAT_FONT,		L"Bold",		GetBold())
			GET_PROP_BOOL(CAT_FONT,		L"Italic",	GetItalic())
			GET_PROP_BOOL(CAT_FONT,		L"Strikethrough",	GetStrikeOut())
			GET_PROP_BOOL(CAT_FONT,		L"Underline",	GetUnderline())
			GET_PROP_STRING(CAT_DATA,	L"Variable",	GetControlVariable().c_str())
			GET_PROP_STRING(CAT_APPEARANCE,	L"Text",	GetCaption().c_str())
		END_GET_PROP_MAP()
		return CBaseObject<CButton>::GetParamValue(Category, Name, Param);	
	}

	//Установить значение параметра с категорией Category и названием Name
	bool CButton::SetParamValue(enCatID Category, wstring Name, sParam Param)
	{
		BEGIN_SET_PROP_MAP(Category, Name, Param)
			SET_PROP_STRING(CAT_FONT,	L"Font",		SetFontName(val))
			SET_PROP_INT(CAT_FONT,		L"Size",		SetHeight(val))
			SET_PROP_BOOL(CAT_FONT,		L"Bold",		SetBold(val))
			SET_PROP_BOOL(CAT_FONT,		L"Italic",	SetItalic(val))
			SET_PROP_BOOL(CAT_FONT,		L"Strikethrough",	SetStrikeOut(val))
			SET_PROP_BOOL(CAT_FONT,		L"Underline",	SetUnderline(val))

			SET_PROP_STRING(CAT_DATA,	L"Variable",	SetControlVariable(val))

			SET_PROP_STRING(CAT_APPEARANCE,	L"Text",	SetCaption(val))
		END_SET_PROP_MAP()
		return CBaseObject<CButton>::SetParamValue(Category, Name, Param);
	}


	//Обработчик нажания левой кнопки мыши 
	BOOL CButton::OnMouseLButtonDown(sVector Point)
	{
		return CBaseObject<CButton>::OnMouseLButtonDown(Point);
	}

	//Обработчик нажания правой кнопки мыши
	BOOL CButton::OnMouseRButtonDown(sVector Point)
	{
		return CBaseObject<CButton>::OnMouseRButtonDown(Point);
	}

	//Обработчик отпускания левой кнопки мыши 
	BOOL CButton::OnMouseLButtonUp(sVector Point)
	{
		return CBaseObject<CButton>::OnMouseLButtonUp(Point);
	}

	//Обработчик перемещения мыши
	BOOL CButton::OnMouseMove(sVector Point)
	{
		return CBaseObject<CButton>::OnMouseMove(Point);
	}

	//Обработчик двойного нажатия левой кнопки мыши 
	BOOL CButton::OnMouseLButtonDblClk(sVector Point)
	{
		return CObjButton::OnMouseLButtonDblClk(Point);
	}

	//Обработчик нажатия клавиши клавиатуры
	BOOL CButton::OnKeyDown(WPARAM wParam, LPARAM lParam)
	{	
		return CBaseObject<CButton>::OnKeyDown(wParam, lParam);
	}

	//Проверка попадает ли point на объект
	bool CButton::HitTest(sVector point)
	{
		return CBaseObject<CButton>::HitTest(point);
	}

	//Получить строковое значение категории
	wstring CButton::StrFromCatID(enCatID id)
	{
		wstring str = CBaseObject<CButton>::StrFromCatID(id);
		switch(id) 
		{
		case CAT_FONT:			str = L"Font";  break;
		case CAT_DATA:		    str = L"Data"; break;
		}
		return str;
	}
}
