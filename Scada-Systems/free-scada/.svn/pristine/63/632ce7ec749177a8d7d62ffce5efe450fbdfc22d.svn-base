#include "StdAfx.h"
#include ".\graph.h"

namespace Primitives
{
	CGraph::CGraph(void)
	{
		//Дополнение карты категория - список sCtrlParam(название параметра - контрол)
		TCtrlParamList& Visual = _Categories[CAT_APPEARANCE];

		Visual.push_back(sCtrlParam(CT_FLAG, L"Border"));
		Visual.push_back(sCtrlParam(CT_COLOR, L"Border color"));
		Visual.push_back(sCtrlParam(CT_INT, L"Graph Time"));
		Visual.sort(sCompareName());

		//Remove the angle attribute
		RemoveParam(CAT_GEOMETRY, L"Angle");
	}

	CGraph::~CGraph(void)
	{
	}

	//Получить параметр с названием Name категории Category
	bool CGraph::GetParamValue(enCatID Category, wstring Name, sParam& Param)
	{
		BEGIN_GET_PROP_MAP(Category, Name, Param)
/*			GET_PROP_BOOL(CAT_APPEARANCE, L"Border",IsDrawBorder())
			GET_PROP_RGB(CAT_APPEARANCE, L"Border color",GetBorderColor())*/
			GET_PROP_INT(CAT_APPEARANCE, L"Graph Time",GetGraphTime())
		END_GET_PROP_MAP()
		
		return CBaseObject<CGraph>::GetParamValue(Category, Name, Param);	
	}

	//Установить параметр с названием Name категории Category
	bool CGraph::SetParamValue(enCatID Category, wstring Name, sParam Param)
	{
		BEGIN_SET_PROP_MAP(Category, Name, Param)
			/*SET_PROP_BOOL(CAT_APPEARANCE, L"Border", DrawBorder(val))
			SET_PROP_RGB(CAT_APPEARANCE, L"Border color",SetBorderColor(val))*/
			SET_PROP_INT(CAT_APPEARANCE, L"Graph Time",SetGraphTime(val))
		END_SET_PROP_MAP()
		

		bool res = CBaseObject<CGraph>::SetParamValue(Category, Name, Param);
		HelpersLib::CCfgReg reg;
		CString RegPath;
		RegPath.Format(L"%s\\LastObjectSettings\\Graph", cProduct);
		reg.Open((LPCTSTR)RegPath, false, true);
		reg.WriteFloat(L"Width", GetSize().x);
		reg.WriteFloat(L"Height", GetSize().y);
		
		return res;
	}

	//Set focus handler
	void CGraph::OnSetFocus()
	{
		CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
		//Fill the property window
		prop->SetObject(this);
		prop->FreeInst();
		CObjGraph::OnSetFocus();
		
		CObject* obj = GetObject();

		//Manipulators creation
		CManipulatorMover		*move	= new CManipulatorMover(obj);
		CManipulatorSizeLeft	*left	= new CManipulatorSizeLeft(obj);
		CManipulatorSizeRight	*right	= new CManipulatorSizeRight(obj);
		CManipulatorSizeTop		*top	= new CManipulatorSizeTop(obj);
		CManipulatorSizeBottom	*bottom	= new CManipulatorSizeBottom(obj);
		//Set Min/Max sizes
		left->SetMinSize(sVector(200, 100));
		right->SetMinSize(sVector(200, 100));
		top->SetMinSize(sVector(200, 100));
		bottom->SetMinSize(sVector(200, 100));

		_Manipulators.push_front(move);
		_Manipulators.push_front(left);
		_Manipulators.push_front(right);
		_Manipulators.push_front(top);
		_Manipulators.push_front(bottom);
	}

	//Обработчик потери объектом фокуса
	void CGraph::OnLostFocus()
	{
		CBaseObject<CGraph>::OnLostFocus();
	}

	//Обработчик изменения объекта
	void CGraph::OnEdit()
	{
		CObjectPropertyWindow* prop = CObjectPropertyWindow::Instance();
		prop->RefreshProperty();
		prop->FreeInst();
	}

	//Рисование прямоугольнка
	void CGraph::Draw(HDC dc_)
	{
		const bool Visible = IsVisible();
		//В дизайнере объект всегда видимый
		SetVisible(true);

		CObjGraph::Draw(dc_);
		CBaseObject<CGraph>::Draw(dc_);
		//Восстанавливаем исходное значение видимости
		SetVisible(Visible);
	}

	//Обработчик нажатия левой кнопки мыши
	BOOL CGraph::OnMouseLButtonDown(sVector Point)
	{
		return CBaseObject<CGraph>::OnMouseLButtonDown(Point);
	}

	//Обработчик нажатия правой кнопки мыши
	BOOL CGraph::OnMouseRButtonDown(sVector Point)
	{
		return CBaseObject<CGraph>::OnMouseRButtonDown(Point);
	}

	//Обработчик двойного нажатия левой кнопки мыши
	BOOL CGraph::OnMouseLButtonDblClk(sVector Point)
	{
		return CBaseObject<CGraph>::OnMouseLButtonDblClk(Point);
	}

    //Обработчик отпускания левой кнопки мыши
	BOOL CGraph::OnMouseLButtonUp(sVector Point)
	{
		return CBaseObject<CGraph>::OnMouseLButtonUp(Point);
	}

	//Обработчик перемещения мыши
	BOOL CGraph::OnMouseMove(sVector Point)
	{
		return CBaseObject<CGraph>::OnMouseMove(Point);
	}

	//Обработчик нажатия клавиши клавиатуры
	BOOL CGraph::OnKeyDown(WPARAM wParam, LPARAM lParam)
	{	
		return CBaseObject<CGraph>::OnKeyDown(wParam, lParam);
	}

	//Проверка попадает ли вектор point на объект
	bool CGraph::HitTest(sVector point)
	{
		return CBaseObject<CGraph>::HitTest(point);
	}

	
}