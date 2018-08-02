#pragma once
/********************************************************************
	Создан:	        2005/02/09
	Создан:	        9.2.2005   17:09
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Doc.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer
	Имя файла:	    Doc
	Расширение:	    h
	Автор(ы):	    Михаил
    Класс(ы):       CDoc
	
Описание:	Класс реализует работу с загрузкой-сохранением, управлением проектом 
			и схемами, настройками проекта
*********************************************************************/



//Текущий примитив для сосздания
enum enCreatePrimitive
{
	P_EMPTY,
	P_POLYLINE,
	P_RECTANGLE,
	P_ELLIPSE,
	P_IMAGE,
	P_TEXT,
	P_BUTTON,
	P_INPUT,
	P_SOUND,
	P_GRAPH
};



class CDoc
	: public DVF::CBaseDocument
{
public:
	enCreatePrimitive	m_CurrentCreatingPrimitive; //Текущий примитив для сосздания
	CArchiveManager		m_Archive; //Архив
	bool				m_ShowGrid;

	//Создание нового примитива
	void CreateNewPrimitive(enCreatePrimitive ID, CPoint pt, BOOL UseLastSettings=FALSE);
	//Загрузка проекта
	bool LoadProject(wstring FileName);
	//Сохранение проекта
	bool SaveProject(wstring FileName);
	//Создание нового проекта
	bool NewProject();
	//Создание новой схемы
	bool NewSchema(wstring SchemaName);
	//Является ли файл временным
	bool IsFileTemp(){ return !_TempFile.empty(); }
	
	//Получить названия всех изображений поекта
	bool GetImageNames(TStrings& Names);
	//Получить названия всех звуков поекта
	bool GetSoundNames(TStrings& Names);
	//Получить названия всех схем проекта
	bool GetSchemaNames(TStrings& Names);
	//Загрузить переменные проекта
	bool LoadVariables();
	//Сохранить переменные проекта
	bool SaveVariables();
	//Загрузить настройки проекта
	bool LoadProjectSettings();
	//Сохранить настройки проекта
	bool SaveProjectSettings();
	//Получить название текущей схемы
	wstring GetCurrentSchemaName(){ return _CurrentSchema; }
	//Установить название текущей схемы
	void SetCurrentSchemaName(wstring Schema){ _CurrentSchema = Schema; }
	//Переименовать текущую схему
	bool RenameCurrentSchema(wstring NewSchemaName);
	//Удалить схему
	void DeleteSchema(wstring SchemaName);
	//Сохранить текущую схему
	void SaveCurrentSchema();
	//Загрузить текущую схему
	void LoadCurrentSchema();
	//Установить флаг изменения проекта
	void SetModifiedFlag(bool Modified = true){ _IsModified = Modified; }
	
	//Получить настойки схемы
	sSchemaSettings GetSchemaSettings(){ return _Settings; }
	//Установить настройки схемы
	void SetSchemaSettings(sSchemaSettings Settings){ _Settings = Settings; }
	//Получить настройки проекта
	sProjectSettings GetProjectSettings(){return _ProjectSettings; }
	//Установить настройки проекта
	void SetProjectSettings(sProjectSettings ProjectSettings){_ProjectSettings = ProjectSettings; }

	CDoc(void);
	~CDoc(void);
protected:
	wstring _TempFile; //Название временного файла
	wstring _CurrentSchema; //Название текущей схемы
	sSchemaSettings _Settings; //Настройки схемы
	sProjectSettings _ProjectSettings; //Настройки проекта

	bool _IsModified; //Флаг модификации документа

};
