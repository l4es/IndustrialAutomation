#pragma once
#include "BaseModule.h"
#include "resource.h"

//Класс, отображающий дерево модулей
typedef CWinTraitsOR<TVS_HASBUTTONS  | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVIF_STATE/*| TVS_DISABLEDRAGDROP*/ > CTreeTraits;
class CModuleTreeView
	: public CWindowImpl<CModuleTreeView, CTreeViewCtrl, CTreeTraits>
{
protected:
	HTREEITEM _RootItem;
	sNode* _ParentNode;
	TNodes _ModuleNodes;//вектор с "ветвями" модулей

	HTREEITEM _DragItem; //Перетаскиваемый итем

	CImageList m_DragImageList; // 
	void FillTree(HTREEITEM Parent, sNode* Node); //Заполнение дерева
	

public:


	BEGIN_MSG_MAP(CModuleTreeView)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_BEGINDRAG, OnBeginDrag)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	
	CModuleTreeView() : _ParentNode(NULL){}

	void InitialUpdate(void);//Создание CImageList для дерева и заполнение дерева
	void Update(sNode* Selected);//Очистка и заполнение дерева
	void AddModuleNode(sNode* Node){
		_ModuleNodes.push_back(Node); 
	}
	void MoveItem(HTREEITEM From, HTREEITEM To);//Установка для From в качестве родителя То
	bool IsValidToMove(HTREEITEM From, HTREEITEM To);//Проверка может ли То быть родителем From

	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnBeginDrag(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/); //DragAndDrop
	
};
