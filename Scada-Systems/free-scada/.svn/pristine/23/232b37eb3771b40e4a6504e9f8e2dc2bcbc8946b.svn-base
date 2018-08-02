#include "StdAfx.h"

#include "../Primitives/Ellipse.h"
#include ".\propertylistview.h"
#include "../MainFrm.h"
#include "Prop.h"

//Класс для автоматического сокрытия и отображения окна
class CAutoHiderForWindow
{
	HWND _hWnd;
	BOOL Active;
public:
	CAutoHiderForWindow(HWND hwnd)
		:_hWnd(hwnd)
	{
		Active = ::IsWindowVisible(_hWnd);
		if(Active)
			::ShowWindow(_hWnd, SW_HIDE);
	}
	~CAutoHiderForWindow()
	{
		if(Active)
			::ShowWindow(_hWnd, SW_SHOW);
	}
};

CPropertyListView::CPropertyListView(void)
{
	m_Object = NULL;
}

CPropertyListView::~CPropertyListView(void)
{

}

LRESULT CPropertyListView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	SetExtendedListStyle(PLS_EX_CATEGORIZED);
	PostMessage(WM_POSTCREATE);

	return 0;
}

LRESULT CPropertyListView::OnPostCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetColumnWidth(90);

	return 0;
}

LRESULT CPropertyListView::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RedrawWindow();
	bHandled = FALSE;
	return 0;
}

//Запись значения выбранного аттрибута в объекте примитива
void CPropertyListView::SaveObjectValue(BOOL UpdateProp)
{
	if(m_Object == NULL)
		return;

	int idx = GetCurSel();
	if(idx == -1)
		return;

	if(m_hwndInplace != NULL && UpdateProp)
		SendMessage(WM_USER_PROP_UPDATEPROPERTY, 0, (LPARAM)m_hwndInplace);

	HPROPERTY prop = GetProperty(idx);
	//Получение имени аттрибута
	wchar_t buf[0xff];
	GetItemName(prop, buf, 0xff);
	wstring name(buf);

	//Получение значения аттрибута
	_variant_t varVal;
	VariantInit(&varVal);
	GetItemValue(prop, &varVal);

	//Запись значения аттрибута в структуру sParam
	enCatID CatID = (enCatID)GetItemData(prop);
	sParam prm;
	TCtrlParamList List = m_Object->GetParameters(CatID);
	TCtrlParamListIter it = find_if(List.begin(), List.end(), sFindCtrl(name));
	if(it == List.end())
		return ;
	
	switch(it->Ctrl)
	{
	case CT_BOOL:
	case CT_FLAG:
		prm.SetBool(varVal.iVal);
		break;

	case CT_INT:
		{
			CComVariant v(varVal);
			v.ChangeType(VT_I2);
			prm.SetInt(v.iVal);
		}
		break;
	case CT_FLOAT:
		{
			CComVariant v(varVal);
			v.ChangeType(VT_R4);
			prm.SetFloat(v.fltVal);
		}
		break;

	case CT_COLOR:
		prm.SetRGB((COLORREF)varVal.intVal);
		break;

	case CT_TEXT:
	case CT_FONT:
	case CT_IMAGES:
	case CT_SOUND:
	case CT_VARIABLE:
	case CT_ALIGN:
		prm.SetString((wstring)(_bstr_t)varVal.bstrVal);
		break;
	}

	//Передача значения аттрибута в объект через структуру sParam
	if(prm.GetType() != T_NOTYPE)
		m_Object->SetParamValue(CatID, name, prm);

	//Перерисовка графического окна
	((CWindow)GetParent()).PostMessage(WM_UPDATEVIEW);
}


LRESULT CPropertyListView::OnItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled)
{
	bHandled = FALSE;
	SaveObjectValue();
	return 0;
}

//Установка примитива, атрибута которого будут отображаться
void CPropertyListView::SetObject(Primitives::CBase* obj)
{
	m_Object = obj;
	if(!IsWindow())
		return;

	ResetContent();
	if(obj == NULL)
		return;
	TCategoryList pCategories = obj->GetCategories();
	if(pCategories.empty()) return;

	//Скрываем окно аттрибутов (окно аттрибутов снова появляется при выходе из функции)
	CAutoHiderForWindow AutoHide(m_hWnd);

	//Заполнение значений аттрибутов
	int i=0;
	for(TCategoryListIter itc =  pCategories.begin(); itc != pCategories.end(); itc++)
	{
		HPROPERTY cat=NULL;
		if(*itc != CAT_EMPTY)
		{
			i++;
			cat = AddItem( PropCreateCategory( m_Object->StrFromCatID(*itc).c_str()) );
		}
		TCtrlParamList pParameters = obj->GetParameters(*itc);
		if(pParameters.empty()) continue;

		for(TCtrlParamListIter itp =  pParameters.begin(); itp != pParameters.end(); itp++)
		{
			sParam prm;
			if(obj->GetParamValue(*itc, itp->Param, prm))
			{
				int ival;
				float fval;
				CComVariant variant;

				HPROPERTY item;
				switch(itp->Ctrl)
				{
				case CT_COLOR:
					{
						sRGB rgb;
						if(prm.GetRGB(rgb))
						{
							item = AddItem( PropCreateColor(itp->Param.c_str(), rgb.AsCOLORREF()) );
							SetItemData(item, (LPARAM)(*itc));
						}
					}
					break;
				case CT_IMAGES:
					item = AddItem( PropCreateImageList(itp->Param.c_str(), prm.GetAsText()) );
					SetItemData(item, (LPARAM)(*itc));
					break;
				case CT_SOUND:
					item = AddItem( PropCreateSoundList(itp->Param.c_str(), prm.GetAsText()) );
					SetItemData(item, (LPARAM)(*itc));
					break;
				case CT_BOOL:
					{
						bool val;
						if(prm.GetBool(val))
						{
							item = AddItem( PropCreateSimple(itp->Param.c_str(), val) );
							SetItemData(item, (LPARAM)(*itc));
						}
					}
					break;
				case CT_FLAG:
					{
						bool val;
						if(prm.GetBool(val))
						{
							item = AddItem( PropCreateCheckButton(itp->Param.c_str(), val) );
							SetItemData(item, (LPARAM)(*itc));
						}
					}
					break;
				
				case CT_TEXT:
					item = AddItem( PropCreateSimple(itp->Param.c_str(),  prm.GetAsText().c_str()) );
					SetItemData(item, (LPARAM)(*itc));
					break;

				case CT_ALIGN:
					item = AddItem( PropCreateAlignList(itp->Param.c_str(),  prm.GetAsText().c_str()) );
					SetItemData(item, (LPARAM)(*itc));
					break;
			
				case CT_INT:
					if(prm.GetInt(ival))
					{
						variant = ival;
						item = AddItem( PropCreateVariantEx(itp->Param.c_str(),  variant) );
						SetItemData(item, (LPARAM)(*itc));
					}
					break;
				
				case CT_FLOAT:
					if(prm.GetFloat(fval))
					{
						variant = fval;
						item = AddItem( PropCreateVariantEx(itp->Param.c_str(),  variant) );
						SetItemData(item, (LPARAM)(*itc));
					}
					break;
			
				case CT_FONT:
					item = AddItem( PropCreateFontList(itp->Param.c_str(), prm.GetAsText()) );
					SetItemData(item, (LPARAM)(*itc));
					break;
			
				case CT_VARIABLE:
					item = AddItem( PropCreateVariableList(itp->Param.c_str(), prm.GetAsText()) );
					SetItemData(item, (LPARAM)(*itc));
					break;
				}
			}
		}
		
		if(*itc != CAT_EMPTY && cat!=NULL)
			ExpandItem(cat);
	}
}

//Обновление значений аттрибутов в окне свойств
void CPropertyListView::RefreshProrerty()
{
 	if(m_Object == NULL)
		return;

	TCategoryList pCategories = m_Object->GetCategories();
	if(pCategories.empty()) return;

	for(TCategoryListIter itc =  pCategories.begin(); itc != pCategories.end(); itc++)
	{
		TCtrlParamList pParameters = m_Object->GetParameters(*itc);
		if(pParameters.empty()) continue;

		for(TCtrlParamListIter itp =  pParameters.begin(); itp != pParameters.end(); itp++)
		{
			HPROPERTY prop = FindProperty(itp->Param.c_str());
			if(prop == NULL)
				continue;

			if(prop->GetItemData() != (LPARAM)(*itc))
				break;
			
			int idx = FindProperty(prop);

			sParam prm;
			if(m_Object->GetParamValue(*itc, itp->Param.c_str(), prm))
			{
				CComVariant v;
				sRGB rgb;
				wstring str;

				switch(itp->Ctrl)
				{
				case CT_COLOR:
					if(prm.GetRGB(rgb))
					{
						v.ChangeType(VT_I4);
						v = (int)rgb.AsCOLORREF();
					}
					break;

				case CT_IMAGES:
					if(prm.GetString(str))
					{
						v.ChangeType(VT_BSTR);
						v = str.c_str();
					}
					break;

				case CT_TEXT:
					str = prm.GetAsText();
					v.ChangeType(VT_BSTR);
					v = str.c_str();
					break;

				case CT_ALIGN:
					if(prm.GetString(str))
					{
						v.ChangeType(VT_BSTR);
						v = str.c_str();
					}
					break;

				case CT_INT:
					str = prm.GetAsText();
					v.ChangeType(VT_BSTR);
					v = str.c_str();
					break;

				case CT_FLOAT:
					str = prm.GetAsText();
					v.ChangeType(VT_BSTR);
					v = str.c_str();
					break;
				}


				if(v.vt != VT_EMPTY)
				{
					prop->SetValue(v);
					InvalidateItem(idx);
				}
			}
		}
	}
	
}




