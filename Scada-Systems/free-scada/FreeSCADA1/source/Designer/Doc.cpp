#include "StdAfx.h"
#include ".\doc.h"
#include "Primitives/Primitives.h"
#include "MainFrm.h"


CDoc::CDoc(void)
	: m_CurrentCreatingPrimitive(P_EMPTY)
	, _CurrentSchema(L"")
	, _IsModified(false)
	, m_ShowGrid(false)
	
{
	HelpersLib::CCfgReg reg;
	reg.Open(cProduct, true, true);
	m_ShowGrid = reg.ReadDWORD(L"ShowGrid", FALSE)?true:false;
}

CDoc::~CDoc(void)
{
	HelpersLib::CCfgReg reg;
	reg.Open(cProduct, false, true);
	reg.WriteDWORD(L"ShowGrid", m_ShowGrid?TRUE:FALSE);

	//Удаляем временный файл
	if(!_TempFile.empty())
	{
		DeleteFileW(_TempFile.c_str());
		_TempFile.clear();
	};
}

//Создание нового примитива
void CDoc::CreateNewPrimitive(enCreatePrimitive ID, CPoint pt, BOOL UseLastSettings)
{
	HelpersLib::CCfgReg reg;
	if(UseLastSettings)
	{	
		CString RegPath;
		RegPath.Format(L"%s\\LastObjectSettings\\", cProduct);
		switch(ID) 
		{
		case P_EMPTY:		RegPath += L"Empty"; break;
		case P_POLYLINE:	RegPath += L"Polyline"; break;
		case P_RECTANGLE:	RegPath += L"Rectangle"; break;
		case P_ELLIPSE:		RegPath += L"Ellipse"; break;
		case P_IMAGE:		RegPath += L"Image"; break;
		case P_TEXT:		RegPath += L"Text"; break;
		case P_BUTTON:		RegPath += L"Button"; break;
		case P_INPUT:		RegPath += L"Input"; break;
		case P_SOUND:		RegPath += L"Sound"; break;
		case P_GRAPH:		RegPath += L"Graph"; break;
		}
		reg.Open((LPCTSTR)RegPath, false, true);
	}

	Primitives::CRectangle*	rect;
	Primitives::CPolyline*	poly;
	Primitives::CEllipse*	elli;
	Primitives::CImage*		image;
	Primitives::CText*		text;
	Primitives::CButton*	button;
	Primitives::CInput*		input;
	Primitives::CSound*		sound;
	Primitives::CGraph*		graph;
	CObjectMenagerPtr mng;
	//Генерация новой порядковой позиции (используется для сортировки в списке объектов)
	DWORD OrderPos = mng->GenerateOrderPos();
	sVector pos = mng->ScreenToGlobal(pt);
	
	switch(ID)
	{
	case P_RECTANGLE: 
		{
			rect = new Primitives::CRectangle();
			rect->MoveTo(pos);
			if(UseLastSettings)
			{
				rect->SetSize(	reg.ReadFloat(L"Width", 100),
								reg.ReadFloat(L"Height", 100));
			}
			else
				rect->SetSize(100, 100);
			rect->SetColor(cDefaultRectangleColor);
			rect->SetOrderPos(OrderPos);
			mng->AddObject(rect);
		}
		break;
	case P_POLYLINE:
		poly = new Primitives::CPolyline();
		poly->AddVertex(pos);
		poly->AddVertex(pos.x + 100, pos.y+100);
		poly->SetColor(cDefaultPolylineColor);
		poly->SetOrderPos(OrderPos);
		mng->AddObject(poly);
		break;
	case P_ELLIPSE:
		elli = new Primitives::CEllipse();
		mng->AddObject(elli);
		elli->MoveTo(pos);
		elli->SetSize(120, 70);
		elli->SetColor(cDefaultEllipseColor);
		elli->SetOrderPos(OrderPos);
		break;
	case P_IMAGE:
		{
		image = new Primitives::CImage();
		mng->AddObject(image);
		image->MoveTo(pos);
		if(UseLastSettings)
		{
			image->SetSize(	reg.ReadFloat(L"Width", 100),
							reg.ReadFloat(L"Height", 100));
			if(!reg.ReadString(L"Image").empty())
				image->SetImage(reg.ReadString(L"Image"));
		}
		else
			image->SetSize(100,100);
		image->SetColor(cDefaultImageColor);
		image->SetOrderPos(OrderPos);
		}
		break;
	case P_TEXT:
		text = new Primitives::CText();
		mng->AddObject(text);
		text->MoveTo(pos);
		text->SetSize(100, 20);
		text->SetText(L"Text");
		text->SetColor(cDefaultTextColor);
		text->SetBGColor(sRGB(255,255,255));
		text->SetOrderPos(OrderPos);
		break;
	case P_BUTTON:
		button = new Primitives::CButton();
		mng->AddObject(button);
		button->MoveTo(pos);
		button->SetSize(90, 30);
		button->SetCaption(L"Button");
		button->SetColor(::GetSysColor(COLOR_BTNFACE));
		button->SetOrderPos(OrderPos);
		break;
	case P_INPUT:
		input = new Primitives::CInput();
		mng->AddObject(input);
		input->MoveTo(pos);
		input->SetSize(90, 20);
		input->SetColor(RGB(0,0,0));
		input->SetOrderPos(OrderPos);
		break;
	case P_SOUND:
		sound = new Primitives::CSound();
		mng->AddObject(sound);
		sound->MoveTo(pos);
		sound->SetOrderPos(OrderPos);
		break;
	case P_GRAPH:
		graph = new Primitives::CGraph();
		mng->AddObject(graph);
		graph->MoveTo(pos);
		graph->SetSize(300, 100);
		graph->SetOrderPos(OrderPos);
		break;
	}
	//Сортируем объекты 
	mng->SortObjects();
	//Присваиваем новые значения объектам в списке в соответствии с их
	//порядком в списке
	mng->SetObjOrderPos();

	UpdateAllViews(NULL);
}

//Загрузка проекта
bool CDoc::LoadProject(wstring FileName)
{
	CWaitCursor wait;
	//Если временный файл не пустой, очистить содержимое временного файла
	if(!_TempFile.empty())
	{
		DeleteFileW(_TempFile.c_str());
		_TempFile.clear();
	};
	//Установить новое имя файла архива
	m_Archive.SetArchiveFile(FileName);
	LoadVariables();
	LoadProjectSettings();
	TStrings Names;
	//Получить названия схем в проекте
	GetSchemaNames(Names);  

	//Установка текущей схемы в соответствии с настройками проекта 
	if(!_ProjectSettings.InitialSchema.empty() && 
		(find(Names.begin(), Names.end(), _ProjectSettings.InitialSchema) !=  Names.end()))
	{
		SetCurrentSchemaName(_ProjectSettings.InitialSchema);
		LoadCurrentSchema();
	}
	else
	{
		if(!Names.empty())
			SetCurrentSchemaName(*Names.begin());
		LoadCurrentSchema();
	}
	
	return true;
}

//Сохранение проекта
bool CDoc::SaveProject(wstring FileName)
{
	CWaitCursor wait;

	SaveCurrentSchema();
	SaveProjectSettings();
	SaveVariables();
	
	if(_TempFile.empty() && m_Archive.GetArchiveFile().empty())
		return false;
	//Если данные во временном файле
	if(!_TempFile.empty())
	{
		CopyFile(m_Archive.GetArchiveFile().c_str(), FileName.c_str(), FALSE);
		DeleteFileW(_TempFile.c_str());
		m_Archive.SetArchiveFile(FileName);
        _TempFile.clear();
	}
	
	if(m_Archive.GetArchiveFile() != FileName)
	{
		//Сохранить как
		CopyFile(m_Archive.GetArchiveFile().c_str(), FileName.c_str(), FALSE);
	}

	return true;
}

//Создание нового проекта
bool CDoc::NewProject()
{
	CWaitCursor wait;
	//Удаление временного файла
	if(!_TempFile.empty())
	{
		DeleteFileW(_TempFile.c_str());
		_TempFile.clear();
	};

	TCHAR TempPath[_MAX_PATH+1];
	TCHAR TempFile[_MAX_PATH+1];
	GetTempPathW(_MAX_PATH, TempPath);

	//Получаем новое имя для архивного файла
	if(GetTempFileName(TempPath, _T("MN_"), 0, TempFile) == NULL)
		return false;
	//Устанавливаем название для архивного файла 
	m_Archive.SetArchiveFile(TempFile);
	//Создаем новый архивный файл 
	m_Archive.CreateNewArchive();
	_TempFile = TempFile;

	//Устанавливаем настройки проекта по умолчанию
	SetProjectSettings(sProjectSettings());
	//Сохраняем настройки проекта
	SaveProjectSettings();
	//создаем новую схему
	NewSchema(L"New circuit #1");
	return true;
}

//Создание новой схемы
bool CDoc::NewSchema(wstring SchemaName)
{
	CWaitCursor wait;

	sSchemaSettings Settings;
	//Установка настроек по умолчанию для текущей схемы
	SetSchemaSettings(Settings);
	//Установка названия текущей схемы
	SetCurrentSchemaName(SchemaName);
	//Удаление всех объектов из архива
	m_Archive.RemoveAllObjects();
	//Сохранение новой схемы в файле
	SaveCurrentSchema();
//	LoadCurrentSchema();
	return true;
}

//Получить названия изображений в проекте
bool CDoc::GetImageNames(TStrings& Names)
{
	TStrings List;
	if(m_Archive.GetDirectoryFileList(List, CArchiveManager::SF_IMAGES))
	{
		Names = List;
		return true;
	}

	return false;
}

//Получить названия звуков в проекте
bool CDoc::GetSoundNames(TStrings& Names)
{
	TStrings List;
	if(m_Archive.GetDirectoryFileList(List, CArchiveManager::SF_SOUNDS))
	{
		Names = List;
		return true;
	}

	return false;
}

//Загрузка переменных проекта
bool CDoc::LoadVariables()
{
	m_Archive.LoadVariables();
	return true;
}

//Сохранение переменных проекта
bool CDoc::SaveVariables()
{
	m_Archive.SaveVariables();
	return true;
}

//Загрузка настроек проекта
bool CDoc::LoadProjectSettings()
{
	m_Archive.LoadProjectSettings(_ProjectSettings);
	return true;
}

//Сохранение настроек проекта
bool CDoc::SaveProjectSettings()
{
	m_Archive.SaveProjectSettings(_ProjectSettings);
	return true;
}

//Получить список назвний схем
bool CDoc::GetSchemaNames(TStrings& Names)
{
	TStrings List;
	if(m_Archive.GetDirectoryFileList(List, CArchiveManager::SF_SCHEMAS))
	{
		Names = List;
		return true;
	}

	return false;
}

//Сохранить текущую схему
void CDoc::SaveCurrentSchema()
{
	CWaitCursor wait;

	m_Archive.SaveSchema(_CurrentSchema, _Settings);
	SaveVariables();
	SaveProjectSettings();
}

//Переименовать текущую схему
bool CDoc::RenameCurrentSchema(wstring NewSchemaName)
{
	//Удаляем из архива текущую схему
	if(m_Archive.DeleteFolder(CArchiveManager::SF_SCHEMAS, _CurrentSchema))
	{
		if(_ProjectSettings.InitialSchema == _CurrentSchema)
		{
			_ProjectSettings.InitialSchema = NewSchemaName;
		}
		//Присваиваем текущей схеме новое название
		_CurrentSchema = NewSchemaName;
		//Сохраняем текущую схему
		SaveCurrentSchema();
		return true;
	}
	return false;
}

//Удалить схему
void CDoc::DeleteSchema(wstring SchemaName)
{

	if(m_Archive.DeleteFolder(CArchiveManager::SF_SCHEMAS, SchemaName))
	{
		if(_CurrentSchema == SchemaName)
		{
			TStrings Names;
			GetSchemaNames(Names);
			if(!Names.empty())
			{
				_CurrentSchema = *Names.begin();
			}
		}

		if(_ProjectSettings.InitialSchema == SchemaName)
		{
			_ProjectSettings.InitialSchema.clear();
		}
	}
}

//Загрузка текущей схемы
void CDoc::LoadCurrentSchema()
{
	CWaitCursor wait;
	//Удаляем все объекты из ObjectMenager-а
	m_Archive.RemoveAllObjects();
	//Загрузка настроек схемы
	m_Archive.LoadSchemaSettings(_CurrentSchema, _Settings);
	//Загрузка объектов схемы
	m_Archive.LoadSchemaObj<Primitives::CRectangle>(_CurrentSchema, _RECTANGLE);
	m_Archive.LoadSchemaObj<Primitives::CPolyline>(_CurrentSchema, _POLYLINE);
	m_Archive.LoadSchemaObj<Primitives::CEllipse>(_CurrentSchema, _ELLIPSE);
	m_Archive.LoadSchemaObj<Primitives::CImage>(_CurrentSchema, _IMAGE);
	m_Archive.LoadSchemaObj<Primitives::CText>(_CurrentSchema, _TEXT);
	m_Archive.LoadSchemaObj<Primitives::CButton>(_CurrentSchema, _BUTTON);
	m_Archive.LoadSchemaObj<Primitives::CInput>(_CurrentSchema, _INPUT);
	m_Archive.LoadSchemaObj<Primitives::CSound>(_CurrentSchema, _SOUND);
	m_Archive.LoadSchemaObj<Primitives::CGraph>(_CurrentSchema, _GRAPH);
	
	//Загрузка действий
	m_Archive.LoadActions(_CurrentSchema);

	UpdateAllViews();
}
