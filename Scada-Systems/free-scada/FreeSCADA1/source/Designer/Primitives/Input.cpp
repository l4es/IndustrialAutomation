#include "StdAfx.h"
#include "../MainFrm.h"
#include "BaseObject.h"
#include ".\input.h"

namespace Primitives
{
	CInput::CInput(void)
	{
		//Дополняем карту категория-список sCtrlParam(имя-тип контрола)
		TCtrlParamList Font;
		Font.push_back(sCtrlParam(CT_FONT,	L"Font"));
		Font.push_back(sCtrlParam(CT_INT,	L"Size"));
		Font.push_back(sCtrlParam(CT_FLAG,	L"Bold"));
		Font.push_back(sCtrlParam(CT_FLAG,	L"Italic"));
		Font.push_back(sCtrlParam(CT_FLAG,	L"Strikethrough"));
		Font.push_back(sCtrlParam(CT_FLAG,	L"Underline"));
		Font.sort(sCompareName());
		_Categories[CAT_FONT] = Font;

		TCtrlParamList Data;
		Data.push_back(sCtrlParam(CT_VARIABLE,	L"Variable"));
		Data.push_back(sCtrlParam(CT_TEXT,		L"Format"));
		Data.sort(sCompareName());
		_Categories[CAT_DATA] = Data;	

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
	}

	CInput::~CInput(void)
	{
	}

	//Рисование объекта
	void CInput::Draw(HDC dc_) 
	{
		const bool Visible = IsVisible();
		//В дизайнере объект всегда видимый
		SetVisible(true);
		CObjInput::Draw(dc_);
		for_each(_Manipulators.begin(),_Manipulators.end(), bind2nd(mem_fun(&CObject::Draw), dc_));
		//Восстанавливаем значение видимости
		SetVisible(Visible);
	}

	//Обработчик получения фокуса объектом
	void CInput::OnSetFocus()
	{
		CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
		prop->SetObject(this);
		prop->FreeInst();
		CObjInput::OnSetFocus();
		CObject* obj = GetObject();

		//Создание манипуляторов
		CManipulatorMover		*move	= new CManipulatorMover(obj);
		CManipulatorSizeLeft	*left	= new CManipulatorSizeLeft(obj);
		CManipulatorSizeRight	*right	= new CManipulatorSizeRight(obj);
		CManipulatorSizeTop		*top	= new CManipulatorSizeTop(obj);
		CManipulatorSizeBottom	*bottom	= new CManipulatorSizeBottom(obj);
		//Устанавливаем минимальные размеры объекта
		left->SetMinSize(sVector(20, 20));
		right->SetMinSize(sVector(20,20));
		top->SetMinSize(sVector(20,20));
		bottom->SetMinSize(sVector(20,20));

		_Manipulators.push_front(move);
		_Manipulators.push_front(left);
		_Manipulators.push_front(right);
		_Manipulators.push_front(top);
		_Manipulators.push_front(bottom);
	}

	void CInput::OnLostFocus()
	{
		CObjInput::OnLostFocus();
		//Освобождение манипуляторов
		CBaseObject<CInput>::OnLostFocus();
	}



	void CInput::OnEdit()
	{
		CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
		//Обновление параметров объекта в окне свойств
		prop->RefreshProperty();
		prop->FreeInst();
		return CObjInput::OnEdit();
	}


	//Обработчик двойного нажатия на левую кнопку мыши
	BOOL CInput::OnMouseLButtonDblClk(sVector Point)
	{
		return CObjInput::OnMouseLButtonDblClk(Point);
	}

	//Сохранение-загрузка
	void CInput::Serialize(CMemFile& file, BOOL IsLoading)
	{
		CObjInput::Serialize(file, IsLoading);
	}

	
	//Получить значение параметра с категорией Category и названием Name
	bool CInput::GetParamValue(enCatID Category, wstring Name, sParam& Param)
	{
		BEGIN_GET_PROP_MAP(Category, Name, Param)
			GET_PROP_STRING(CAT_FONT,	L"Font",		GetFontName().c_str())
			GET_PROP_INT(CAT_FONT,		L"Size",		GetHeight())
			GET_PROP_BOOL(CAT_FONT,		L"Bold",		GetBold())
			GET_PROP_BOOL(CAT_FONT,		L"Italic",	GetItalic())
			GET_PROP_BOOL(CAT_FONT,		L"Strikethrough",	GetStrikeOut())
			GET_PROP_BOOL(CAT_FONT,		L"Underline",	GetUnderline())

			GET_PROP_STRING(CAT_DATA,	L"Variable",	GetControlVariable().c_str())
			GET_PROP_STRING(CAT_DATA,	L"Format",		GetFormat().c_str())
		END_GET_PROP_MAP()
		return CBaseObject<CInput>::GetParamValue(Category, Name, Param);	
	}

	//Установить значение параметра с категорией Category и названием Name
	bool CInput::SetParamValue(enCatID Category, wstring Name, sParam Param)
	{
		BEGIN_SET_PROP_MAP(Category, Name, Param)
			SET_PROP_STRING(CAT_FONT,	L"Font",		SetFontName(val))
			SET_PROP_INT(CAT_FONT,		L"Size",		SetHeight(val))
			SET_PROP_BOOL(CAT_FONT,		L"Bold",		SetBold(val))
			SET_PROP_BOOL(CAT_FONT,		L"Italic",	SetItalic(val))
			SET_PROP_BOOL(CAT_FONT,		L"Strikethrough",	SetStrikeOut(val))
			SET_PROP_BOOL(CAT_FONT,		L"Underline",	SetUnderline(val))

			SET_PROP_STRING(CAT_DATA,	L"Variable",	SetControlVariable(val))
			SET_PROP_STRING(CAT_DATA,	L"Format",		SetFormat(val))
		END_SET_PROP_MAP()
		return CBaseObject<CInput>::SetParamValue(Category, Name, Param);
	}

	//Обработчик нажатия на левую кнопку мыши
	BOOL CInput::OnMouseLButtonDown(sVector Point)
	{
		return CBaseObject<CInput>::OnMouseLButtonDown(Point);
	}

	//Обработчик нажатия на правую кнопку мыши
	BOOL CInput::OnMouseRButtonDown(sVector Point)
	{
		return CBaseObject<CInput>::OnMouseRButtonDown(Point);
	}

    //Обработчик отпускния левой кнопки мыши
	BOOL CInput::OnMouseLButtonUp(sVector Point)
	{
		return CBaseObject<CInput>::OnMouseLButtonUp(Point);
	}

	//Обработчик перемещения мыши
	BOOL CInput::OnMouseMove(sVector Point)
	{
		return CBaseObject<CInput>::OnMouseMove(Point);
	}

	//Обработчик нажатия на клавишу клавиатуры
	BOOL CInput::OnKeyDown(WPARAM wParam, LPARAM lParam)
	{	
		return CBaseObject<CInput>::OnKeyDown(wParam, lParam);
	}

	//Определить попадает ли point на объект
	bool CInput::HitTest(sVector point)
	{
		return CBaseObject<CInput>::HitTest(point);
	}
}
