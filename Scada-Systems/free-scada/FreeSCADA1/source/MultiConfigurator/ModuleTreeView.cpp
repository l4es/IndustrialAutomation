#include "StdAfx.h"
#include ".\moduletreeview.h"
#include "ModuleManager.h"

inline int GetDigitalPos(wstring &str)
{
	for(wstring::iterator i=str.begin();i!=str.end();i++)
		if(isdigit(static_cast<unsigned char>(*i)))
			return static_cast<int>(i-str.begin());
	return -1;
}

inline int GetDigit(int start, wstring &str)
{
	CString tmp;
	size_t i=start;
	while(isdigit(static_cast<unsigned char>(str[i])) && i<str.size())
		tmp += str[i++];
	return _wtoi(tmp);
};
//Сравнение пунктов (для сортировки)
int CALLBACK CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM /*lParamSort*/)
{
	sNode* Node1 = (sNode*)lParam1;
	sNode* Node2 = (sNode*)lParam2;


	if((Node1->Type == T_BRANCH) && (Node2->Type == T_ITEM))
		return -1;

	if((Node1->Type == T_ITEM) && (Node2->Type == T_BRANCH)) 
		return 1;

	//Если пункты содержат цифры, сортируем с учетом их
	const int	pos1 = GetDigitalPos(Node1->Name),
				pos2 = GetDigitalPos(Node2->Name);
	if(pos1>=0 && pos2 >=0 && pos1 == pos2)
	{
		const int	digit1 = GetDigit(pos1, Node1->Name),
					digit2 = GetDigit(pos2, Node2->Name);
		if(digit1 > digit2)
			return 1;
		else
		{
			if(digit1 == digit2)
				return 0;
			else
				return -1;
		}
	}
	else
		return  lstrcmpi(Node1->Name.c_str(), Node2->Name.c_str());
}
//Очистка дерева и заполнение его
void CModuleTreeView::InitialUpdate(void)
{
	CImageList ImageList;
	ImageList.CreateFromImage(IDB_ICONS, 16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
	SetImageList(ImageList, LVSIL_NORMAL);
	ImageList.Detach();
	Update(NULL);
	_DragItem = NULL;
}


void CModuleTreeView::Update(sNode* Selected)
{
	DeleteAllItems();
//Заполнение дерева
	for(TNodes::iterator it = _ModuleNodes.begin(); it != _ModuleNodes.end(); it++)
	{
		if(*it != NULL)
			FillTree(NULL, *it);
	}
//Если какой-то узел был выбранным - разкрываем его и делаем выбранным	
	if(Selected != NULL)
	{
		sNode* Current = Selected;
	
		while(Current != NULL)
		{
			Expand(Current->hItem);
			Current = Current->ParentNode;
		}
		Select(Selected->hItem, TVGN_CARET);
	}
}

//Заполнение дерева
void CModuleTreeView::FillTree(HTREEITEM Parent, sNode* Node)
{
	ATLASSERT(Node != NULL);

	HTREEITEM Item;
	if(Node->Type == T_ITEM)
		Item = InsertItem(Node->Name.c_str(), Node->IconInactive, Node->IconActive, Parent, TVI_LAST);
	else
		Item = InsertItem(Node->Name.c_str(), Node->IconClose, Node->IconClose, Parent, TVI_LAST);
	
	Node->hItem = Item;
	SetItemData(Item, (DWORD_PTR)Node);

	if( Node->Type == T_BRANCH )
	{
		for(TNodes::iterator it = Node->ChildNodes.begin();
			it != Node->ChildNodes.end(); it++)
		{
			FillTree(Item, *it);
		}
	}

//Сортировка пунктов
	TVSORTCB SortCB;
	SortCB.hParent = Parent;
	SortCB.lParam = 0;
	SortCB.lpfnCompare = &CompareItems;
//	SortChildren(Parent);
	SortChildrenCB(&SortCB, 0);
}

//Раскрываем меню для выбранного узла (если оно есть) при нажатии правой кнопки мыши
LRESULT CModuleTreeView::OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	//Определяем попадаем ли на пункт иделаем его выбранным
	CPoint point;
	point.x = GET_X_LPARAM(lParam); 
	point.y = GET_Y_LPARAM(lParam); 

	UINT flag;
	HTREEITEM item = HitTest(point, &flag);
	if(item == NULL)
		return 0;
	Select(item, TVGN_CARET);

	sNode* Node = (sNode*)GetItemData(item);
	if(Node == NULL)
		return 0;

	CBaseModule* Module = Node->Module;
	if(Module == NULL)
		return 0;

	ClientToScreen(&point);
	//Раскрытие меню
	Module->ShowMenu(point);

	return 0;
}


LRESULT CModuleTreeView::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	CPoint point;
	point.x = GET_X_LPARAM(lParam); 
	point.y = GET_Y_LPARAM(lParam); 

	UINT flag;
	HTREEITEM item = HitTest(point, &flag);
	if(item == NULL)
		return 0;

	Select(item, TVGN_CARET);
	
	return 0;
}

LRESULT CModuleTreeView::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
// Освобождаем ImageList, отображающий перетаскиваемый пункт
	m_DragImageList.DragLeave(m_hWnd);
	m_DragImageList.EndDrag();
	m_DragImageList.Destroy();

	ReleaseCapture();
	CPoint point;
	point.x = GET_X_LPARAM(lParam); 
	point.y = GET_Y_LPARAM(lParam); 

	UINT flag;
	HTREEITEM item = HitTest(point, &flag);
	//Делаем item родителем для _DragItem
	MoveItem(_DragItem, item);
	_DragItem = NULL;

	//Сортировка 
	TVSORTCB SortCB;
	SortCB.hParent = NULL;
	SortCB.lParam = 0;
	SortCB.lpfnCompare = &CompareItems;


	SortChildrenCB(&SortCB);

    return 0;
}


LRESULT CModuleTreeView::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	CPoint point;
	point.x = GET_X_LPARAM(lParam); 
	point.y = GET_Y_LPARAM(lParam); 

	UINT flag;
	
	HTREEITEM item = HitTest(point, &flag);
//Если пункт над которым находися перетаскиваемый пункт может быть родителем
//для перетаскиваемого пункта - выделяем этот пункт
	if((_DragItem != NULL) && IsValidToMove(_DragItem, item))
	{
		m_DragImageList.DragShowNolock(FALSE);		
		SelectDropTarget(item);
		m_DragImageList.DragShowNolock(TRUE);
	}
	m_DragImageList.DragMove(point);
	return 0;
}
 
//Создаем ImageList для отображения перетаскиваемого пункта
LRESULT CModuleTreeView::OnBeginDrag(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	LPNMTREEVIEW tv = (LPNMTREEVIEW)pnmh;
	HTREEITEM item = tv->itemNew.hItem;
	CPoint Point = tv->ptDrag;

	SetCapture();
	m_DragImageList = CreateDragImage(item);
	m_DragImageList.BeginDrag(0,CPoint(-15,-15));
	m_DragImageList.DragEnter(m_hWnd, Point);

	_DragItem = item;

	return 0;
}

//Делаем пункт То родительским для пункта From
void CModuleTreeView::MoveItem(HTREEITEM From, HTREEITEM To)
{
	//Проверка, может ли То быть родителем для From
	if(!IsValidToMove(From, To))
		return;
	//определям sNode для То и From
	sNode* FromNode = (sNode*)GetItemData(From);
	sNode* ToNode = (sNode*)GetItemData(To);

	//Удаляем From из списка списка "детей" его родителя
	TNodes::iterator it = 
		find_if(FromNode->ParentNode->ChildNodes.begin(), 
				FromNode->ParentNode->ChildNodes.end(), 
				sFindNode(FromNode->ItemID));
	if(it != FromNode->ParentNode->ChildNodes.end())
		FromNode->ParentNode->ChildNodes.erase(it);
	
	//Помещаем From в список детей То
	ToNode->ChildNodes.push_back(FromNode);
	//Устанавливаем для From родителя То
	FromNode->ParentNode = ToNode;

	FromNode->Module->Update();
//Перерисовываем дерево
	Update(FromNode);
}

//Проверка может ли узел То быть родителем узла From
bool CModuleTreeView::IsValidToMove(HTREEITEM From, HTREEITEM To)
{
	if(From == NULL)
		return false;

	if(To == NULL)
		return false;

	if(From == To)
		return false;

	sNode* FromNode = (sNode*)GetItemData(From);
	if(FromNode == NULL)
		return false;

	sNode* ToNode = (sNode*)GetItemData(To);
	if(ToNode == NULL)
		return false;

	if(FromNode->Module != ToNode->Module)
		return false;

	if(ToNode->Type != T_BRANCH)
		return false;
	
	return true;
}
