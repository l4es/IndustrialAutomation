#pragma once

//Типы узлов
enum enNodeType { T_BRANCH, T_ITEM };
enum enFileOtions {FO_SAVE = 1, FO_LOAD=2};

class CBaseModule;
class CModuleManager;

//Узел
struct sNode
{
	wstring Name; //имя узла
	enNodeType Type; //тип узла (папка или простой пункт)
	DWORD ItemID; //id узла
	CBaseModule* Module; //Модуль, к которому относится узел
	
	sNode* ParentNode; //Родительский узел
	vector<sNode*> ChildNodes; //Узлы - дети (для папки)

	HTREEITEM hItem; //HTREEITEM для узла

	UINT IconInactive;	//id иконки для отображения в TreeView	(Элемент не выделен)
	UINT IconActive;	//id иконки для отображения в TreeView	(Элемент выделен)
	UINT IconOpen;		//id иконки для отображения в TreeView	(Папка открыта)
	UINT IconClose;		//id иконки для отображения в TreeView	(Папка закрыта)
	sNode() 
		:Module(NULL)
		,hItem(NULL)
		,IconOpen(NULL)
		,IconClose(NULL)
		,IconInactive(NULL)
		,IconActive(NULL){}
};

//функтор поиска узла по ID
struct sFindNode
	: public unary_function<sNode*, bool>
{
	DWORD id;
	sFindNode(DWORD id_) : id(id_){	}
	bool operator()(sNode* Node)
	{
		return Node->ItemID == id;
	}
};

typedef vector<sNode*> TNodes;

//Класс базового модуля
class CBaseModule
{
public:
	HWND m_hWindow; //HWND текущего окна
	HWND m_hToolBar; //HWND toolbar-a  модуля
	sNode* m_Node; //
	sNode* m_CurrentNode; //Текущий узел

	CModuleManager* m_ModuleManager;

	CBaseModule(void) 
		: m_hWindow(NULL)
		, m_Node(NULL)
		, m_CurrentNode(NULL)
		, m_hToolBar(NULL)
		, _SupportedOptions(NULL)
	{}
		

	virtual ~CBaseModule(void){};

	
	virtual BOOL PreTranslateMessage(MSG* /*pMsg*/){ return FALSE; }
	virtual bool Detect(){return false;} //Проверка, опеделен ли модуль
	virtual bool Load(HWND, HWND, CRect){return false;} //Загрузка модуля
	virtual bool SetCurrent(sNode* Node){ m_CurrentNode = Node; return true; } //Установка текущего узла
	virtual bool ExpandNode(sNode* /*Node*/){ return true; } //Раскрытие узла
	virtual bool SaveNode(sNode* /*Node*/){ return false; } //Сохранение данных узла
	virtual HWND GetWndHandle(){ return m_hWindow; } //Получить HWND вида (который отображается в правом окне программы)
	virtual void ShowMenu(CPoint){} //Показать меню (Появляется при нажатии правой кнопки мыши в TreeView
									//на узле, относящемся к этому модулю)	
	virtual void Update(){} 
	virtual void OnFileSave(){}//Обработчик команды Файл->Сохранить
	virtual void OnFileOpen(){}//Обработчик команды Файл->Загрузить

	inline BOOL ModuleCanSave(){return (_SupportedOptions&FO_SAVE) ? TRUE:FALSE;}
	inline BOOL ModuleCanLoad(){return (_SupportedOptions&FO_LOAD) ? TRUE:FALSE;}
	
	virtual BOOL OnCommand(WORD /*CtrlType*/, WORD /*CtrlID*/, HWND /*hWnd*/){return FALSE;}

protected:
	DWORD _SupportedOptions;
};
