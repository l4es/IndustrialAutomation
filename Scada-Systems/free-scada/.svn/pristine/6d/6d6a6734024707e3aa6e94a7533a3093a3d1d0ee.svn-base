#include "StdAfx.h"
#include ".\sound.h"

namespace Primitives
{
	CSound::CSound(void)
	{
		RemoveParam(CAT_GEOMETRY, L"Angle");
		RemoveParam(CAT_GEOMETRY, L"Height");
		RemoveParam(CAT_GEOMETRY, L"Width");
		_Categories[CAT_GEOMETRY].sort(sCompareName());

		RemoveParam(CAT_APPEARANCE, L"Color");
		_Categories[CAT_APPEARANCE].sort(sCompareName());
		
		_Categories[CAT_DATA].push_back(sCtrlParam(CT_SOUND, L"Sound"));
		_Categories[CAT_DATA].push_back(sCtrlParam(CT_BOOL, L"Loop"));
		_Categories[CAT_DATA].sort(sCompareName());
	}

	CSound::~CSound(void)
	{
	}

	//Получить параметр с названием Name категории Category
	bool CSound::GetParamValue(enCatID Category, wstring Name, sParam& Param)
	{
		BEGIN_GET_PROP_MAP(Category, Name, Param)
			GET_PROP_STRING(CAT_DATA, L"Sound", GetSoundName().c_str())
			GET_PROP_BOOL(CAT_DATA, L"Loop", GetLoop())
		END_GET_PROP_MAP()
		return CBaseObject<CSound>::GetParamValue(Category, Name, Param);	
	}

	//Установить параметр с названием Name категории Category
	bool CSound::SetParamValue(enCatID Category, wstring Name, sParam Param)
	{
		BEGIN_SET_PROP_MAP(Category, Name, Param)
			SET_PROP_STRING(CAT_DATA, L"Sound", SetSoundName(val))
			SET_PROP_BOOL(CAT_DATA, L"Loop", SetLoop(val))
		END_SET_PROP_MAP()
		return CBaseObject<CSound>::SetParamValue(Category, Name, Param);
	}

	//Обработчик получения объектом фокуса
	void CSound::OnSetFocus()
	{
		CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
		//Заполняем окно свойств параметрами объекта
		prop->SetObject(this);
		prop->FreeInst();
		
		CObject* obj = GetObject();
		CManipulatorMover		*move	= new CManipulatorMover(obj);
		_Manipulators.push_front(move);
	}

	//Обработчик потери объектом фокуса
	void CSound::OnLostFocus()
	{
		CBaseObject<CSound>::OnLostFocus();
	}

	//Обработчик изменения объекта
	void CSound::OnEdit()
	{
		CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
		prop->RefreshProperty();
		prop->FreeInst();
	}

	//Рисование
	void CSound::Draw(HDC dc_)
	{
		const bool Visible = IsVisible();
		//В дизайнере объект всегда видимый
		SetVisible(true);

		CObjSound::Draw(dc_);
		CBaseObject<CSound>::Draw(dc_);
		//Восстанавливаем исходное значение видимости
		SetVisible(Visible);
	}

	//Обработчик нажатия левой кнопки мыши
	BOOL CSound::OnMouseLButtonDown(sVector Point)
	{
		return CBaseObject<CSound>::OnMouseLButtonDown(Point);
	}

	//Обработчик нажатия правой кнопки мыши
	BOOL CSound::OnMouseRButtonDown(sVector Point)
	{
		return CBaseObject<CSound>::OnMouseRButtonDown(Point);
	}

	//Обработчик двойного нажатия левой кнопки мыши
	BOOL CSound::OnMouseLButtonDblClk(sVector Point)
	{
		return CBaseObject<CSound>::OnMouseLButtonDblClk(Point);
	}

    //Обработчик отпускания левой кнопки мыши
	BOOL CSound::OnMouseLButtonUp(sVector Point)
	{
		return CBaseObject<CSound>::OnMouseLButtonUp(Point);
	}

	//Обработчик перемещения мыши
	BOOL CSound::OnMouseMove(sVector Point)
	{
		return CBaseObject<CSound>::OnMouseMove(Point);
	}

	//Обработчик нажатия клавиши клавиатуры
	BOOL CSound::OnKeyDown(WPARAM wParam, LPARAM lParam)
	{	
		return CBaseObject<CSound>::OnKeyDown(wParam, lParam);
	}

	//Проверка попадает ли вектор point на объект
	bool CSound::HitTest(sVector point)
	{
		return CBaseObject<CSound>::HitTest(point);
	}

	
}