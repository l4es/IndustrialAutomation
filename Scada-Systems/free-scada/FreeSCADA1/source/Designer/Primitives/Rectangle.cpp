#include "StdAfx.h"
#include ".\rectangle.h"

namespace Primitives
{
	CRectangle::CRectangle(void)
	{
		//Дополнение карты категория - список sCtrlParam(название параметра - контрол)
		TCtrlParamList& Visual = _Categories[CAT_APPEARANCE];

		Visual.push_back(sCtrlParam(CT_FLAG, L"Border"));
		Visual.push_back(sCtrlParam(CT_COLOR, L"Border color"));
		Visual.sort(sCompareName());
	}

	CRectangle::~CRectangle(void)
	{
	}

	//Получить параметр с названием Name категории Category
	bool CRectangle::GetParamValue(enCatID Category, wstring Name, sParam& Param)
	{
		BEGIN_GET_PROP_MAP(Category, Name, Param)
			GET_PROP_BOOL(CAT_APPEARANCE, L"Border",IsDrawBorder())
			GET_PROP_RGB(CAT_APPEARANCE, L"Border color",GetBorderColor())
		END_GET_PROP_MAP()
		return CBaseObject<CRectangle>::GetParamValue(Category, Name, Param);	
	}

	//Установить параметр с названием Name категории Category
	bool CRectangle::SetParamValue(enCatID Category, wstring Name, sParam Param)
	{
		BEGIN_SET_PROP_MAP(Category, Name, Param)
			SET_PROP_BOOL(CAT_APPEARANCE, L"Border", DrawBorder(val))
			SET_PROP_RGB(CAT_APPEARANCE, L"Border color",SetBorderColor(val))
		END_SET_PROP_MAP()
		bool res = CBaseObject<CRectangle>::SetParamValue(Category, Name, Param);
		
		HelpersLib::CCfgReg reg;
		CString RegPath;
		RegPath.Format(L"%s\\LastObjectSettings\\Rectangle", cProduct);
		reg.Open((LPCTSTR)RegPath, false, true);
		reg.WriteFloat(L"Width", GetSize().x);
		reg.WriteFloat(L"Height", GetSize().y);
		return res;
	}

	//Обработчик получения объектом фокуса
	void CRectangle::OnSetFocus()
	{
		CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
		//Заполняем окно свойств параметрами объекта
		prop->SetObject(this);
		prop->FreeInst();
		CBaseObject<CRectangle>::OnSetFocus();
	}

	//Обработчик потери объектом фокуса
	void CRectangle::OnLostFocus()
	{
		CBaseObject<CRectangle>::OnLostFocus();
	}

	//Обработчик изменения объекта
	void CRectangle::OnEdit()
	{
		CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
		prop->RefreshProperty();
		prop->FreeInst();
	}

	//Рисование прямоугольнка
	void CRectangle::Draw(HDC dc_)
	{
		const bool Visible = IsVisible();
		//В дизайнере объект всегда видимый
		SetVisible(true);

		CObjRectangle::Draw(dc_);
		CBaseObject<CRectangle>::Draw(dc_);
		//Восстанавливаем исходное значение видимости
		SetVisible(Visible);
	}

	//Обработчик нажатия левой кнопки мыши
	BOOL CRectangle::OnMouseLButtonDown(sVector Point)
	{
		return CBaseObject<CRectangle>::OnMouseLButtonDown(Point);
	}

	//Обработчик нажатия правой кнопки мыши
	BOOL CRectangle::OnMouseRButtonDown(sVector Point)
	{
		return CBaseObject<CRectangle>::OnMouseRButtonDown(Point);
	}

	//Обработчик двойного нажатия левой кнопки мыши
	BOOL CRectangle::OnMouseLButtonDblClk(sVector Point)
	{
		return CBaseObject<CRectangle>::OnMouseLButtonDblClk(Point);
	}

    //Обработчик отпускания левой кнопки мыши
	BOOL CRectangle::OnMouseLButtonUp(sVector Point)
	{
		return CBaseObject<CRectangle>::OnMouseLButtonUp(Point);
	}

	//Обработчик перемещения мыши
	BOOL CRectangle::OnMouseMove(sVector Point)
	{
		return CBaseObject<CRectangle>::OnMouseMove(Point);
	}

	//Обработчик нажатия клавиши клавиатуры
	BOOL CRectangle::OnKeyDown(WPARAM wParam, LPARAM lParam)
	{	
		return CBaseObject<CRectangle>::OnKeyDown(wParam, lParam);
	}

	//Проверка попадает ли вектор point на объект
	bool CRectangle::HitTest(sVector point)
	{
		return CBaseObject<CRectangle>::HitTest(point);
	}

	
}