#include "stdafx.h"
#include "Param.h"
#include "../PropertyWindow/ObjectPropertyWindow.h"

#include ".\text.h"

namespace Primitives
{
	CText::CText(void)
	{
		//Дополняем карту категория-список структур sCtrlParam
		TCtrlParamList& Visual = _Categories[CAT_APPEARANCE];
		Visual.push_back(sCtrlParam(CT_FLAG, L"Transparent background"));
		Visual.push_back(sCtrlParam(CT_COLOR, L"Background color"));
		Visual.sort(sCompareName());

		TCtrlParamList Font;
		Font.push_back(sCtrlParam(CT_FONT,	L"Font"));
		Font.push_back(sCtrlParam(CT_INT,	L"Size"));
		Font.push_back(sCtrlParam(CT_ALIGN,	L"Alignment"));
		Font.push_back(sCtrlParam(CT_FLAG,	L"Bold"));
		Font.push_back(sCtrlParam(CT_FLAG,	L"Italic"));
		Font.push_back(sCtrlParam(CT_FLAG,	L"Strikethrough"));
		Font.push_back(sCtrlParam(CT_FLAG,	L"Underline"));
		Font.sort(sCompareName());
		_Categories[CAT_FONT] = Font;

		TCtrlParamList Data;
		Data.push_back(sCtrlParam(CT_TEXT, L"Text"));
		Data.push_back(sCtrlParam(CT_FLAG, L"Computed"));
		Data.push_back(sCtrlParam(CT_TEXT, L"Expression"));
		Data.push_back(sCtrlParam(CT_TEXT, L"Format"));
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

	CText::~CText(void)
	{
	}

	//Обработчик получения объектом фокуса
	void CText::OnSetFocus()
	{
		CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
		//Заполнение окна свойств параметрами объекта
		prop->SetObject(this);
		prop->FreeInst();
		CObjText::OnSetFocus();

		CObject* obj = GetObject();

		//Создаем манипуляторы
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

	//Обработчик потери объектом фокуса
	void CText::OnLostFocus()
	{
		CObjText::OnLostFocus();
		//Освобождаем манипуляторы
		CBaseObject<CText>::OnLostFocus();
	}

	//Рисование текста
	void CText::Draw(HDC dc)
	{
		const bool Visible = IsVisible();
		//В дизайнере объект всегда видимый
		SetVisible(true);

		CObjText::Draw(dc);
		CBaseObject<CText>::Draw(dc);
		//Восстанавливаем исходное значение видимости
		SetVisible(Visible);
	}

	//Обработчик нажатия левой кнопки мыши
	BOOL CText::OnMouseLButtonDown(sVector Point)
	{
		return CBaseObject<CText>::OnMouseLButtonDown(Point);
	}

	//Обработчик нажатия правой кнопки мыши
	BOOL CText::OnMouseRButtonDown(sVector Point)
	{
		return CBaseObject<CText>::OnMouseRButtonDown(Point);
	}

	//Обработчик двойного нажатия левой кнопки мыши
	BOOL CText::OnMouseLButtonDblClk(sVector Point)
	{
		return CBaseObject<CText>::OnMouseLButtonDblClk(Point);
	}

	//Обработчик отпускания левой кнопки мыши
	BOOL CText::OnMouseLButtonUp(sVector Point)
	{
		return CBaseObject<CText>::OnMouseLButtonUp(Point);
	}

	//Обработчик перемещения мыши
	BOOL CText::OnMouseMove(sVector Point)
	{
		return CBaseObject<CText>::OnMouseMove(Point);
	}

	//Обработчик нажатия клавиши клавиатуры
	BOOL CText::OnKeyDown(WPARAM wParam, LPARAM lParam)
	{	
		return CBaseObject<CText>::OnKeyDown(wParam, lParam);
	}

	//Проверка попадает ли вектор point на объект
	bool CText::HitTest(sVector point)
	{
		return CBaseObject<CText>::HitTest(point);
	}

	//Обработчик изменения объекта
	void CText::OnEdit()
	{
		CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
		//Обновление параметров в окне свойств
		prop->RefreshProperty();
		prop->FreeInst();

		return CObjText::OnEdit();
	}


	//Получить значение параметра
	bool CText::GetParamValue(enCatID Category, wstring Name, sParam& Param)
	{
		BEGIN_GET_PROP_MAP(Category, Name, Param)
			GET_PROP_BOOL(CAT_APPEARANCE,	L"Transparent background",	IsBGTransparent())
			GET_PROP_RGB(CAT_APPEARANCE,	L"Background color",		GetBGColor())

			GET_PROP_STRING(CAT_FONT,	L"Font",		GetFontName().c_str())
			GET_PROP_INT(CAT_FONT,		L"Size",		GetHeight())
			GET_PROP_BOOL(CAT_FONT,		L"Bold",		GetBold())
			GET_PROP_BOOL(CAT_FONT,		L"Italic",	GetItalic())
			GET_PROP_BOOL(CAT_FONT,		L"Strikethrough",	GetStrikeOut())
			GET_PROP_BOOL(CAT_FONT,		L"Underline",	GetUnderline())
			GET_PROP_STRING(CAT_FONT,	L"Alignment",	GetAlignAsText().c_str())
		
			GET_PROP_STRING(CAT_DATA,	L"Text",		GetText().c_str())
			GET_PROP_BOOL(CAT_DATA,		L"Computed",	IsOPC())
			GET_PROP_STRING(CAT_DATA,	L"Expression",		GetExpression().c_str())
			GET_PROP_STRING(CAT_DATA,	L"Format",		GetFormat().c_str())
			
		END_GET_PROP_MAP()
		return CBaseObject<CText>::GetParamValue(Category, Name, Param);	
	}

	//Установить значение параметра
	bool CText::SetParamValue(enCatID Category, wstring Name, sParam Param)
	{
		BEGIN_SET_PROP_MAP(Category, Name, Param)
			SET_PROP_BOOL(CAT_APPEARANCE,	L"Transparent background",	SetBGTransparent(val))
			SET_PROP_RGB(CAT_APPEARANCE,	L"Background color",		SetBGColor(val))

			SET_PROP_STRING(CAT_FONT,	L"Font",		SetFontName(val))
			SET_PROP_INT(CAT_FONT,		L"Size",		SetHeight(val))
			SET_PROP_BOOL(CAT_FONT,		L"Bold",		SetBold(val))
			SET_PROP_BOOL(CAT_FONT,		L"Italic",	SetItalic(val))
			SET_PROP_BOOL(CAT_FONT,		L"Strikethrough",	SetStrikeOut(val))
			SET_PROP_BOOL(CAT_FONT,		L"Underline",	SetUnderline(val))
			SET_PROP_STRING(CAT_FONT,	L"Alignment",	SetAlignAsText(val))

			SET_PROP_STRING(CAT_DATA,	L"Text",		SetText(val))
			SET_PROP_BOOL(CAT_DATA,		L"Computed",	SetOPC(val))
			SET_PROP_STRING(CAT_DATA,	L"Expression",		SetExpression(val))
			SET_PROP_STRING(CAT_DATA,	L"Format",		SetFormat(val))
			
		END_SET_PROP_MAP()
		return CBaseObject<CText>::SetParamValue(Category, Name, Param);
	}

	//Получить значение категории в виде строки
	wstring CText::StrFromCatID(enCatID id)
	{
		wstring str = CBaseObject<CText>::StrFromCatID(id);
		switch(id) 
		{
		case CAT_FONT:			str = L"Font";  break;
		case CAT_DATA:		    str = L"Data"; break;
		}
		return str;
	}

	//Получить выравнивание текста в виде строки
	wstring CText::GetAlignAsText()
	{
		wstring str = L"By left";
		switch(GetTextAlign()) 
		{
		case TA_LEFT:
			str = L"By left";
			break;
		case TA_RIGHT:
			str = L"By right";
			break;
		case TA_CENTER:
			str = L"By center";
			break;
		}
		return str;
	}

	//Установить выравнивание текста через строковое значение
	void CText::SetAlignAsText(wstring Align)
	{
		if(Align == L"By left")
		{
			SetTextAlign(TA_LEFT);
			return;
		}

		if(Align == L"By right")
		{
			SetTextAlign(TA_RIGHT);
			return;
		}

		if(Align == L"By center")
		{
			SetTextAlign(TA_CENTER);
			return;
		}

		SetTextAlign(TA_LEFT);

	}
}
