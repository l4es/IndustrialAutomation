#pragma once

/********************************************************************
Создан:	        2005/12/31
Создан:	        31.12.2005   9:44
Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\ArchiveMenager.h
Директория:	    d:\Work\Projects\М-Немо\Source\Library
Имя файла:	    ArchiveMenager
Расширение:	    h
Автор(ы):	    Алексей, Михаил
Класс(ы):       CArchiveMenager

Описание:	Класс реализует функции для работы с Zip архивом
*********************************************************************/

#include "../3rdParty/ZipArchive/ZipArchive.h"
#include "../3rdParty/ZipArchive/ZipMemFile.h"
#include "ObjRectangle.h"
#include "ObjPolyline.h"
#include "ObjEllipse.h"
#include "Image.h"

#include "ObjectMenager.h"


#ifdef LoadImage
	#undef LoadImage
#endif
#ifdef DeleteFile
	#undef DeleteFile
#endif

class CImage;

struct sSchemaSettings
{
	bool FixedSize;
	bool HasBackgrd;
	long cx, cy;
	wstring BackGrdName;
	bool StretchBackGrd;
	sRGB BackGrdColor;
	sSchemaSettings()
		: FixedSize(false)
		, HasBackgrd(false)
		, cx(0), cy(0)
		, BackGrdName(L"")
		, StretchBackGrd(false)
		{
			BackGrdColor = GetSysColor(COLOR_APPWORKSPACE);			
		}	
};

struct sProjectSettings 
{
	wstring	InitialSchema;
	DWORD	IntervalOPC; 
	sProjectSettings()
		: InitialSchema(L"")
		, IntervalOPC(5000){}
};

class CArchiveManager
{
public:
	struct sFindName :
		public unary_function<wstring, bool>
	{
		wstring name;
		sFindName(wstring _name): name(_name){}
		bool operator()(wstring _name)
		{
			return (_name == name);
		}
	};		

public:
	//Папки 
	enum enSpecialFolder
	{
		SF_SCHEMAS,
		SF_IMAGES,
		SF_SOUNDS, 
		SF_VARIABLES,
	};

	CArchiveManager(wstring FileName);
	CArchiveManager();
	~CArchiveManager(void);

	
	//Создание нового архива
	bool CreateNewArchive();

	//Установить имя файла архива
	void SetArchiveFile(wstring FileName){ _FileName = FileName; }
	//Получить имя файла архива
	wstring GetArchiveFile(){return _FileName;};
	
	//Загрузить объекты типа T(имеющие тип TypeID) схемы SchemaName в ObjectMenager
	template <class T> bool LoadSchemaObj(wstring SchemaName, enTypeID TypeID);
	//Сохранить объекты, имеющие ID типа равный TypeID
	bool SaveSchemaObj(wstring SchemaName, UINT TypeID);
	
	//Сохранить действия для объектов схемы SchemaName 
	bool SaveActions(wstring SchemaName);
	//Загрузить действия для объектов схемы SchemaName 
	bool LoadActions(wstring SchemaName);

	//Сохранить переменные проекта
	bool SaveVariables();
	//Загрузить переменные проекта
	bool LoadVariables();

	//Сохранить все объекты схемы SchemaName в ObjectMenager
	bool SaveAllObjects(wstring SchemaName);

	//Удалить все объекты из ObjectMenager и ActionMenager
	bool RemoveAllObjects();

	//Получить картинку 
	bool GetImage(wstring ImageName, ::CImage& Image);
	//Сохранить настройки и все объекты схемы
	bool SaveSchema(wstring SchemaName, sSchemaSettings Settings);

	//Загрузтиь настройки схемы
	bool LoadSchemaSettings(wstring SchemaName, sSchemaSettings& Settings);
	//Сохранить настройки схемы
	bool SaveSchemaSettings(wstring SchemaName, sSchemaSettings Settings);

	//Загрузить настройки проекта
	bool LoadProjectSettings(sProjectSettings& Settings);
	//Сохранить настройки проекта
	bool SaveProjectSettings(sProjectSettings Settings);

	//Получить список всех файлов из папки Fldr
	bool GetDirectoryFileList(TStrings& Names, enSpecialFolder Fldr);
	//Удалить файл из архива
	bool DeleteFile(wstring FileName);
	//Создать файл с имененм FileName
	bool CreateMemFile(wstring FileName, CMemFile** file);
	//Открыть файл с имененм FileName
	bool OpenMemFile(wstring FileName, CMemFile** file);
	//Проверка на существование в архиве файла с именем FileName
	bool IsFileExists(wstring FileName);
	//Записываем данные из CMemFile в архив и удаляем объект CMemFile
	bool WriteAndFreeMemFile(CMemFile* file);
	//Записываем данные из CMemFile в архив и удаляем объект CMemFile
	bool WriteAsAndFreeMemFile(wstring FileName, CMemFile* file);
	//Удалить объект CMemFile
	bool FreeMemFile(CMemFile* file);
	//Получить строковое название папки по ее ID
	wstring GetSpecialFolder(enSpecialFolder Fldr);
	//Удалить папку с именем FolderName находящуюся в папке Fldr
	bool DeleteFolder(enSpecialFolder Fldr, wstring FolderName);
protected:
	//Получить строку из enTypeID
	wstring StringFromID(UINT id);

	TStrings			_Schemas;
	wstring				_FileName;
	CObjectMenagerPtr	_Menager;
};

//Загрузить объекты типа T(имеющие тип TypeID) схемы SchemaName в ObjectMenager
template<class T>
bool CArchiveManager::LoadSchemaObj(wstring SchemaName, enTypeID TypeID)
{
	try
	{
		//Получаем полный путь файла 
		wstring path = GetSpecialFolder(SF_SCHEMAS) + L"\\" + SchemaName + L"\\" + StringFromID(TypeID);

		//Создаем объект CMemfile и счтываем в него данные из архива
		CMemFile *file=NULL;
		if(!OpenMemFile(path, &file))
			return false;

		//Считываем количество объектов
		DWORD count;
		(*file)>>count;

		//Создаем объекты, заполняем их данными и добавляем в СObjectMenager
		for(DWORD i = 0; i < count; i++)
		{
			T* obj = new T;
			obj->Serialize(*file);
			_Menager->AddObject(obj);
		}
		//Освобождаем CMemfile созданный в функции OpenMemFile()
		if(!FreeMemFile(file))
			return false;
	}
	catch(...){return false;}

	return true;
}


