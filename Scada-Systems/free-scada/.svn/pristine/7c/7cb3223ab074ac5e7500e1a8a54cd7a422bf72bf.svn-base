#include "StdAfx.h"
#include ".\ellipse.h"

namespace Primitives
{
	CEllipse::CEllipse(void)
	{
		//Дополняем карту категория-список sCtrlParam(имя-тип контрола)
		TCtrlParamList Visual;
		TCategoryMap::iterator it = _Categories.find(CAT_APPEARANCE);
		if(it != _Categories.end())
		{			
			Visual = it->second;
		}
		Visual.push_back(sCtrlParam(CT_FLAG, L"Border"));
		Visual.push_back(sCtrlParam(CT_COLOR, L"Border color"));
		Visual.sort(sCompareName());

		_Categories[CAT_APPEARANCE] = Visual;


	}

	CEllipse::~CEllipse(void)
	{
	}
	
	//Обработчик получения объектом фокуса
	void CEllipse::OnSetFocus()
	{
		CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
		//Отображение параметров в окне свойств и окне действий
		prop->SetObject(this);
		prop->FreeInst();
		CObjEllipse::OnSetFocus();
		CBaseObject<CEllipse>::OnSetFocus();
	}

	//Обработчик потери фокуса объектом
	void CEllipse::OnLostFocus()
	{
		CObjEllipse::OnLostFocus();
		CBaseObject<CEllipse>::OnLostFocus();
	}

	//Рисование эллипса
	void CEllipse::Draw(HDC dc)
	{
		//В дизайнере эллипс всегда виден
		const bool Visible = IsVisible();
		SetVisible(true);

		CObjEllipse::Draw(dc);
		CBaseObject<CEllipse>::Draw(dc);
		//Восстанавливаем значение видимости
		SetVisible(Visible);
	}

	
	//Обработчик изменение объекта
	void CEllipse::OnEdit()
	{
		CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
		//Обновление параметров в окне свойств
		prop->RefreshProperty();
		prop->FreeInst();

		return CObjEllipse::OnEdit();
	}

	//Обработчик нажания левой кнопки мыши 
	BOOL CEllipse::OnMouseLButtonDown(sVector Point)
	{
		return CBaseObject<CEllipse>::OnMouseLButtonDown(Point);
	}

	//Обработчик нажания правой кнопки мыши
	BOOL CEllipse::OnMouseRButtonDown(sVector Point)
	{
		return CBaseObject<CEllipse>::OnMouseRButtonDown(Point);
	}

	//Обработчик двойного нажатия левой кнопки мыши 
	BOOL CEllipse::OnMouseLButtonDblClk(sVector Point)
	{
		return CBaseObject<CEllipse>::OnMouseLButtonDblClk(Point);
	}

	//Обработчик отпускания левой кнопки мыши 
	BOOL CEllipse::OnMouseLButtonUp(sVector Point)
	{
		return CBaseObject<CEllipse>::OnMouseLButtonUp(Point);
	}

	//Обработчик перемещения мыши
	BOOL CEllipse::OnMouseMove(sVector Point)
	{
		return CBaseObject<CEllipse>::OnMouseMove(Point);
	}

	//Обработчик нажатия клавиши клавиатуры
	BOOL CEllipse::OnKeyDown(WPARAM wParam, LPARAM lParam)
	{	
		return CBaseObject<CEllipse>::OnKeyDown(wParam, lParam);
	}

	//Проверка попадает ли point на объект
	bool CEllipse::HitTest(sVector point)
	{
		return CBaseObject<CEllipse>::HitTest(point);
	}

	//Получить значение параметра с категорией Category и названием Name
	bool CEllipse::GetParamValue(enCatID Category, wstring Name, sParam& Param)
	{
		BEGIN_GET_PROP_MAP(Category, Name, Param)
			GET_PROP_BOOL(CAT_APPEARANCE, L"Border", IsDrawBorder())
			GET_PROP_RGB(CAT_APPEARANCE, L"Border color", GetBorderColor())
		END_GET_PROP_MAP()
		return CBaseObject<CEllipse>::GetParamValue(Category, Name, Param);	
	}

	//Установить значение параметра с категорией Category и названием Name
	bool CEllipse::SetParamValue(enCatID Category, wstring Name, sParam Param)
	{
		BEGIN_SET_PROP_MAP(Category, Name, Param)
			SET_PROP_BOOL(CAT_APPEARANCE, L"Border",  DrawBorder(val))
			SET_PROP_RGB(CAT_APPEARANCE, L"Border color", SetBorderColor(val))
		END_SET_PROP_MAP()
		return CBaseObject<CEllipse>::SetParamValue(Category, Name, Param);
	}
}