#include "StdAfx.h"
#include "resource.h"
#include "MainFrm.h"
#include "PropertyWindow\Prop.h"
#include ".\actionsview.h"

CActionsView::CActionsView()
{
	//Заполнение карты тип действия-список структур sCtrlParam
	sCtrlParam Min(CT_FLOAT, L"Min. value");
	sCtrlParam Max(CT_FLOAT, L"Max. value");
	sCtrlParam Expr(CT_TEXT, L"Expression");
	sCtrlParam Variable(CT_VARIABLE, L"Variable");
	sCtrlParam Name(CT_TEXT, L"Name");


	TCtrlParamList Action;
	Action.push_back(Name);
	Action.push_back(Min);
	Action.push_back(Max);
	Action.push_back(Expr);
	Action.push_back(Variable);

	TCtrlParamList Mover;
	Mover=Action;
	Mover.push_back(sCtrlParam(CT_POLYLINE, L"Trajectory"));
	m_ActionsMap[AT_MOVER] = Mover;

	TCtrlParamList Sizer;
	Sizer=Action;
	Sizer.push_back(sCtrlParam(CT_SIDE, L"Direction"));
	m_ActionsMap[AT_SIZER] = Sizer;

	TCtrlParamList Rotate;
	Rotate=Action;
	Rotate.push_back(sCtrlParam(CT_FLOAT, L"Min. angle"));
	Rotate.push_back(sCtrlParam(CT_FLOAT, L"Max. angle"));
	m_ActionsMap[AT_ROTATE] = Rotate;

	TCtrlParamList Visual;
	Visual=Action;
	m_ActionsMap[AT_VISUAL] = Visual;

	TCtrlParamList Color;
	Color=Action;
	Color.push_back(sCtrlParam(CT_COLOR, L"Start color"));
	Color.push_back(sCtrlParam(CT_COLOR, L"End color"));
	m_ActionsMap[AT_COLOR] = Color;

	TCtrlParamList PlaySound;
	PlaySound=Action;
	m_ActionsMap[AT_PLAY_SOUND] = PlaySound;
	
	TCtrlParamList ChangeImage;
	ChangeImage=Action;
	ChangeImage.push_back(sCtrlParam(CT_IMAGES, L"Image 1"));
	ChangeImage.push_back(sCtrlParam(CT_IMAGES, L"Image 2"));
	m_ActionsMap[AT_CHANGE_IMAGE] = ChangeImage;

	TCtrlParamList Curve;
	Curve=Action;
	Curve.push_back(sCtrlParam(CT_COLOR, L"Color"));
	m_ActionsMap[AT_CURVE] = Curve;



}


LRESULT CActionsView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	m_Toolbar.SubclassWindow( CFrameWindowImplBase<>::CreateSimpleToolBarCtrl(m_hWnd, IDR_ACTIONS, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE | CCS_NODIVIDER | CCS_NOPARENTALIGN | TBSTYLE_FLAT) );
	m_Prop.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	m_Prop.SetExtendedListStyle(PLS_EX_CATEGORIZED);
	SendMessage(WM_POSTCREATE);
	bHandled = FALSE;
	return 0;
}

LRESULT CActionsView::OnPostCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{

	m_Toolbar.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
	

	TBBUTTONINFO tbi = { 0 };
	tbi.cbSize  = sizeof(TBBUTTONINFO);
	tbi.dwMask  = TBIF_STYLE | TBIF_COMMAND;
	m_Toolbar.GetButtonInfo(ID_ADD_ACTION, &tbi);
	tbi.fsStyle |= TBSTYLE_DROPDOWN ;
	
	m_Toolbar.SetButtonInfo(ID_ADD_ACTION, &tbi);
	m_Prop.SetColumnWidth(120);
	return 0;
}


LRESULT CActionsView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	CSize sz(lParam);
	const int ToolBarSize = 22;
	m_Toolbar.SetWindowPos(m_hWnd, 0,0,sz.cx,ToolBarSize, SWP_NOZORDER);
	m_Prop.SetWindowPos(m_hWnd,0, ToolBarSize, sz.cx, sz.cy - ToolBarSize, SWP_NOZORDER);
	bHandled = FALSE;
	return 0;
}

//Разворачивание меню выбора действий
LRESULT CActionsView::OnToolbarDropDown(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	LPNMTOOLBAR lptb = (LPNMTOOLBAR) pnmh;
	CToolBarCtrl tb = lptb->hdr.hwndFrom;
	RECT rcItem;
	tb.GetItemRect(tb.CommandToIndex(lptb->iItem), &rcItem);
	POINT pt = { rcItem.left, rcItem.bottom };
	tb.ClientToScreen(&pt);

	if(m_ActionsMenu.IsMenu())
	{
		m_ActionsMenu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, pt.x, pt.y,m_hWnd);
	}
	return TBDDRET_DEFAULT;
}

//Добавление действия изменения размеров
LRESULT CActionsView::OnAddActionSizer(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_Object == NULL)
		return FALSE;

	CActionMenagerPtr ActMng;
	//Создаем действие изменения размеров и добавляем его в ActionMenager
	//CActionSizer* Sizer = new CActionSizer(m_Object->GetObject()); //Alex: было везде вот так....
	CAction* Action= ActMng->AddAction(m_Object->GetObject()->GetName(), AT_SIZER);
	if(!Action) return FALSE;
	//Делаем недоступным в меню выбора действия пункт,
	//соответствующий действию изменения размеров
	SetMenuItemEnabled(AT_SIZER, false);
	
	//Добавляем действие изменения размеров в окно параметров
	AddToList(Action);
	
	return FALSE;
}

//Добавление действия перемещения
LRESULT CActionsView::OnAddActionMover(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_Object == NULL)
		return FALSE;

	CActionMenagerPtr ActMng;
	//Создаем действие перемещения и добавляем его в ActionMenager
	CAction* Action = ActMng->AddAction(m_Object->GetObject()->GetName(), AT_MOVER);
	if(!Action) return FALSE;
	//Делаем недоступным в меню выбора действия пункт,
	//соответствующий действию перемещения
	SetMenuItemEnabled(AT_MOVER, false);

	//Добавляем действие изменения видимости в окно параметров
	AddToList(Action);

	return FALSE;
}

//Добавление действия вращения
LRESULT CActionsView::OnAddActionRotate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_Object == NULL)
		return FALSE;

	CActionMenagerPtr ActMng;
	//Создаем действие вращения и добавляем его в ActionMenager
	CAction* Action =ActMng->AddAction(m_Object->GetObject()->GetName(), AT_ROTATE);
	if(!Action) return FALSE;
	//Делаем недоступным в меню выбора действия пункт,
	//соответствующий действию вращения
	SetMenuItemEnabled(AT_ROTATE, false);

	//Добавляем действие изменения видимости в окно параметров
	AddToList(Action);

	return FALSE;
}

//Добавление действия изменения видимости
LRESULT CActionsView::OnAddActionVisual(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_Object == NULL)
		return FALSE;

	CActionMenagerPtr ActMng;
	//Создаем действие изменения видимости и добавляем его в ActionMenager
	CAction* Action = ActMng->AddAction(m_Object->GetObject()->GetName(),AT_VISUAL);
	if(!Action) return FALSE;
	//Делаем недоступным в меню выбора действия пункт,
	//соответствующий действию изменения видимости
	SetMenuItemEnabled(AT_VISUAL, false);
	//Добавляем действие изменения видимости в окно параметров
	AddToList(Action);
	
	return FALSE;
}

//Добавление действия изменения цвета
LRESULT CActionsView::OnAddActionColor(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_Object == NULL)
		return FALSE;

	CActionMenagerPtr ActMng;
	//Создаем действие изменения цвета и добавляем его в ActionMenager
	CAction* Action = ActMng->AddAction(m_Object->GetObject()->GetName(), AT_COLOR);
	if(!Action) return FALSE;
	//Делаем недоступным в меню выбора действия пункт,
	//соответствующий действию изменения цвета
	SetMenuItemEnabled(AT_COLOR, false);
	//Добавляем действие изменения цвета в окно параметров
	AddToList(Action);

	return FALSE;
}

LRESULT CActionsView::OnAddActionPlaySound(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_Object == NULL)
		return FALSE;

	CActionMenagerPtr ActMng;
	//Создаем действие изменения цвета и добавляем его в ActionMenager
	CAction* Action = ActMng->AddAction(m_Object->GetObject()->GetName(), AT_PLAY_SOUND);
	if(!Action) return FALSE;
	//Делаем недоступным в меню выбора действия пункт,
	//соответствующий действию изменения цвета
	SetMenuItemEnabled(AT_PLAY_SOUND, false);
	//Добавляем действие изменения цвета в окно параметров
	AddToList(Action);

	return FALSE;
}

LRESULT CActionsView::OnAddActionChangeImage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_Object == NULL)
		return FALSE;
	if(g_MainFrame == NULL)
		return FALSE;

	CActionMenagerPtr ActMng;
	
	//Создаем действие изменения цвета и добавляем его в ActionMenager
	CAction* Action= ActMng->AddAction(m_Object->GetObject()->GetName(), AT_CHANGE_IMAGE);
	if(!Action) return FALSE;

	//Делаем недоступным в меню выбора действия пункт,
	//соответствующий действию изменения цвета
	SetMenuItemEnabled(AT_CHANGE_IMAGE, false);
	//Добавляем действие изменения цвета в окно параметров
	AddToList(Action);

	return FALSE;
}
LRESULT CActionsView::OnAddActionCurve(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

	
	if(m_Object == NULL)
		return FALSE;
	if(g_MainFrame == NULL)
		return FALSE;

	CActionMenagerPtr ActMng;
	
	//Создаем действие изменения цвета и добавляем его в ActionMenager
	CAction* Action = ActMng->AddAction(m_Object->GetObject()->GetName(), AT_CURVE);
	if(!Action) return FALSE;

	//Делаем недоступным в меню выбора действия пункт,
	//соответствующий действию изменения цвета
	//SetMenuItemEnabled(AT_CURVE, false);
	//Добавляем действие изменения цвета в окно параметров
	AddToList(Action);

	return FALSE;
}

//Удаление действия
LRESULT CActionsView::OnDeleteAction(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_Object== NULL)
		return 0;
	//Индех выбранного пункта
	int i = m_Prop.GetCurSel();
	if(i == -1) return 0;

	//Выбранный пункт
	HPROPERTY prop = m_Prop.GetProperty(i);
	
	//Тип действия к которому относится выбранный пункт
	CAction *Action = (CAction*)m_Prop.GetItemData(prop);
	CActionMenagerPtr mng;
	//Удаляем действие из ActionMenager
	SetMenuItemEnabled(Action->GetType(), true);
	mng->RemoveAction(m_Object->GetObject()->GetName(), Action->GetName());

	//Очищаем и заполняем окно параметров действиями объекта
	AddActionsToList();
	//Установка доступным пункт соответствующий удаленному действию в меню выбора действия


	return 0;
}

//Заполнение окна свойств параметрами объекта obj
void CActionsView::SetObject(Primitives::CBase* obj)
{
	//Установка текущего объекта равным obj
	m_Object = obj;
	if(!IsWindow())
		return;

	//Очистка окна свойств
	m_Prop.ResetContent();

	
	if(obj == NULL)
		return;

	//Создание меню содержащего список возможных действий
	if(m_ActionsMenu.IsMenu())
		m_ActionsMenu.DestroyMenu();
	m_ActionsMenu.CreatePopupMenu();


	BYTE flags = obj->GetObject()->GetActionFlags();
	//Заполнение меню всеми действиями которые могут бытъ у данного объекта
	if(flags & AT_MOVER)
 		m_ActionsMenu.AppendMenu(MF_BYPOSITION, ID_ACTION_MOVER, L"Move");

	if(flags & AT_SIZER)
		m_ActionsMenu.AppendMenu(MF_BYPOSITION, ID_ACTION_SIZER, L"Size");

	if(flags & AT_ROTATE)
		m_ActionsMenu.AppendMenu(MF_BYPOSITION, ID_ACTION_ROTATE, L"Rotate");
	
	if(flags & AT_VISUAL)
		m_ActionsMenu.AppendMenu(MF_BYPOSITION, ID_ACTION_VISUAL, L"Visibility");

	if(flags & AT_COLOR)
		m_ActionsMenu.AppendMenu(MF_BYPOSITION, ID_ACTION_COLOR, L"Color");
	
	if(flags & AT_PLAY_SOUND)
		m_ActionsMenu.AppendMenu(MF_BYPOSITION, ID_ACTION_PLAY_SOUND, L"Play sound");

	if(flags & AT_CHANGE_IMAGE)
		m_ActionsMenu.AppendMenu(MF_BYPOSITION, ID_ACTION_CHANGE_IMAGE, L"Switch image");
	if(flags & AT_CURVE)
		m_ActionsMenu.AppendMenu(MF_BYPOSITION, ID_ACTION_CURVE, L"Curve");

	//Добавление в окно параметров действий, которые есть у объекта
	AddActionsToList();
}

//Добавление в окно параметров действий, которые есть у объекта
void CActionsView::AddActionsToList()
{
	if(m_Object == NULL)
		return; 

	m_Prop.ResetContent();

	CActionMenagerPtr mng;
	CActionMenager::TActions Actions;
	//Получить из ActionMenager список всех действий объекта m_Object
	mng->GetAllActions(m_Object->GetObject()->GetName(), Actions);

	if(!Actions.empty())
	{
		for(CActionMenager::TActions::iterator it = Actions.begin();
			it != Actions.end(); it++)
		{
			//Делаем неактивным пункт меню соответствующий действию
			//которое уже есть у объекта m_Object
			//SetMenuItemEnabled((*it)->GetType(), false); // был запрет меню....
			//Добавляем действие в окно параметров
			AddToList(*it);
		}
	}
}

//Добавление действия в окно параметров
void CActionsView::AddToList(CAction* Action)
{
	enActionType Type = Action->GetType();
	//Добавление категории (название действия)
	HPROPERTY cat = m_Prop.AddItem( PropCreateCategory(GetNameFromType(Type).c_str()) );
	m_Prop.SetItemData(cat, (LPARAM)Action/*(LPARAM)Type*/);

	
	TActionsMapIter iter = m_ActionsMap.find(Type);
	if(iter == m_ActionsMap.end()) return;
	
	//Получаем список параметров (sCtrlParam) для действия
	TCtrlParamList List = iter->second;
	if(List.empty()) return;

	HPROPERTY Item;
	CComVariant Var;
	
	//Добавляем пункты в окно параметров
	//Контрол определяется полем Ctrl структуры sCtrlParam
	for(TCtrlParamListIter it = List.begin(); it != List.end(); it++)
	{
		Action->GetParamValue(it->Param, Var);
		switch(it->Ctrl) 
		{
		case CT_TEXT:
			Item = m_Prop.AddItem( PropCreateVariantEx(it->Param.c_str(), Var) );
			m_Prop.SetItemData(Item, (LPARAM)Action);
			break;
		case CT_FLOAT:
			Item = m_Prop.AddItem( PropCreateVariantEx(it->Param.c_str(), Var) );
			m_Prop.SetItemData(Item, (LPARAM)Action);
			break;
		case CT_INT:
			Item = m_Prop.AddItem( PropCreateVariantEx(it->Param.c_str(), Var) );
			m_Prop.SetItemData(Item, (LPARAM)Action);
			break;
		case CT_SIDE:
			{
				wstring str(_bstr_t(Var.bstrVal));
				Item = m_Prop.AddItem( PropCreateSideList(it->Param.c_str(), str) );
				m_Prop.SetItemData(Item, (LPARAM)Action);
			}
			break;
		case CT_POLYLINE:
			{
				wstring str(_bstr_t(Var.bstrVal));
				Item = m_Prop.AddItem( PropCreatePolylineList(it->Param.c_str(),str) );
				m_Prop.SetItemData(Item, (LPARAM)Action);
			}
			break;
		case CT_COLOR:
			{
				Item = m_Prop.AddItem( PropCreateColor(it->Param.c_str(), Var.lVal) );
				m_Prop.SetItemData(Item, (LPARAM)Action);
			}
			break;
		case CT_IMAGES:
			{
				wstring str(_bstr_t(Var.bstrVal));
				Item = m_Prop.AddItem( PropCreateImageList(it->Param.c_str(), str) );
				m_Prop.SetItemData(Item, (LPARAM)Action);
			}
			break;
		case CT_VARIABLE:
			{
				wstring str(_bstr_t(Var.bstrVal));
				Item = m_Prop.AddItem( PropCreateVariableList(it->Param.c_str(), str) );
				m_Prop.SetItemData(Item, (LPARAM)Action);
			}
			break;
		default:
			ATLASSERT("Неизвестный тип" == NULL);
		}
	}
	//Разворачиваем категорию (пункт с названием действия)
	m_Prop.ExpandItem(cat);
}

//Обработчик смены выбранного пункта
LRESULT CActionsView::OnItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled)
{
	//Сохранение значения выбранного параметра
	SaveValue();
	bHandled = FALSE;
	return FALSE;
}

//Получить значение пункта в виде вещественного значения
float CActionsView::FloatFromProp(HPROPERTY prop)
{
	wstring s = StringFromProp(prop);
	float f = _wtof(s.c_str());
	return f;
}

//Получить значение пункта в виде строки
wstring CActionsView::StringFromProp(HPROPERTY prop)
{
	CComVariant val;
	m_Prop.GetItemValue(prop, &val);
	_bstr_t bstr = val.bstrVal;
	wstring s(bstr);
	return s;
}

//Сохранение значения выбранного параметра
void CActionsView::SaveValue()
{
	if(m_Object == NULL)
		return;
	
	//Получаем индех выбранного пункта
	int index = m_Prop.GetCurSel();
	if(index == -1)
		return;

	//Получаем свойство для выбранного пункта
	HPROPERTY prop = m_Prop.GetProperty(index);
	
	//Определяем тип действия к которому относится выбранный пункт
	CAction* Action=(CAction *)m_Prop.GetItemData(prop);

	wchar_t buf[0xff];
	//получаем название параметра
	m_Prop.GetItemName(prop, buf, 0xff);
	wstring Name(buf);

	CComVariant Var;
	//Получаем значение параметра действия
	m_Prop.GetItemValue(prop, &Var);
 
	CActionMenagerPtr mng;
	//Находим соответствующее действие в ActionMenager
	Action = mng->FindAction(m_Object->GetObject()->GetName(), Action->GetName());
	if(Name==L"Name")
	{
		if(mng->FindAction(m_Object->GetObject()->GetName(), Var.bstrVal))
		{
			::MessageBox(this->m_hWnd,L"Action with the same name are exists",L"Error",MB_OK);
			
			CComVariant v(Action->GetName().c_str());
			m_Prop.SetItemValue(prop,&v);
			return;
		}

	}

	if(Action == NULL) return;
	//Присваиваем параметру действия новое значение
	Action->SetParamValue(Name, Var);
}


//Устанавливаем пункт меню выбора действий действительным или не действительным
void CActionsView::SetMenuItemEnabled(enActionType Type, bool Enabled)
{
	if(m_ActionsMenu.IsMenu())
	{
			if(Enabled)
				m_ActionsMenu.EnableMenuItem(GetIDFromType(Type), MFS_ENABLED);
			else
				m_ActionsMenu.EnableMenuItem(GetIDFromType(Type), MFS_DISABLED);
	}
}

//Получить ID пункта меню выбора действий по типу действия
UINT CActionsView::GetIDFromType(enActionType Type)
{
	switch(Type)
	{
	case AT_MOVER: return ID_ACTION_MOVER;
	case AT_SIZER: return ID_ACTION_SIZER;
	case AT_ROTATE: return ID_ACTION_ROTATE;
	case AT_VISUAL: return ID_ACTION_VISUAL;
	case AT_COLOR:	return ID_ACTION_COLOR;
	case AT_PLAY_SOUND:	return ID_ACTION_PLAY_SOUND;
	case AT_CHANGE_IMAGE:	return ID_ACTION_CHANGE_IMAGE;
	case AT_CURVE:	return ID_ACTION_CURVE;
	}
	return 0;
}

//Получить строковое название действия по типу действия
wstring CActionsView::GetNameFromType(enActionType Type)
{
	wstring Name;
	switch(Type)
	{
	case  AT_MOVER: Name = L"Move"; break;
	case  AT_SIZER: Name = L"Size"; break;
	case  AT_ROTATE: Name = L"Rotate"; break;
	case  AT_VISUAL: Name = L"Visibility"; break;
	case  AT_COLOR:	Name = L"Color"; break;
	case  AT_PLAY_SOUND:	Name = L"Play sound"; break;
	case  AT_CHANGE_IMAGE:	Name = L"Change image"; break;
	case  AT_CURVE:	Name = L"Curve"; break;
	}
	return Name;
}


