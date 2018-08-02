#include "common.h"
#include "archivemanager.h"
#include "MemFile.h"
#include "ObjImage.h"
#include "ActionMenager.h"
#include "VariableMenager.h"


CArchiveManager::CArchiveManager()
{
}

CArchiveManager::CArchiveManager(wstring FileName)
: _FileName(FileName)
{
}

CArchiveManager::~CArchiveManager(void)
{
}

//Получить строку из enTypeID
wstring CArchiveManager::StringFromID(UINT id)
{
	switch(id)
	{
	case _RECTANGLE: return L"Rectangles";
	case _POLYLINE : return L"Polylines";
	case _ELLIPSE  : return L"Ellipses";
	case _IMAGE	   : return L"Images";
	case _TEXT	   : return L"Text";
	case _BUTTON   : return L"Button";
	case _INPUT	   : return L"Input";
	case _ACTION   : return L"Actions";
	case _VARIABLE : return L"Variables";
	case _SOUND	   : return L"Sounds";
	case _GRAPH	   : return L"Graphics";

	}
	return wstring();
}

//Сохранить действия для объектов схемы SchemaName 
bool CArchiveManager::SaveActions(wstring SchemaName)
{
	try
	{
		CMemFile *file=NULL;
		//Получить полный путь файла содержащего действия
		wstring path = GetSpecialFolder(SF_SCHEMAS)+ L"\\" + SchemaName + L"\\" + StringFromID(_ACTION);
		//Создаем объект CMemfile
		if(!CreateMemFile(path, &file))
			return false;
		//Записываем данные из СActionMenager в СMemFile
		CActionMenagerPtr mng;
		mng->Serialize(*file, FALSE);

		//Записываем данные из СMemFile в архив и освобождаем объект СMemFile
		//созданный в функции CreateMemFile()
		if(!WriteAndFreeMemFile(file))
			return false;
	}
	catch(...){return false;}
	return true;
}

//Загрузить переменные проекта
bool CArchiveManager::LoadVariables()
{
	try
	{
		//Получить полный путь файла содержащего переменные проекта
		wstring path = GetSpecialFolder(SF_VARIABLES) + L"\\" + StringFromID(_VARIABLE);
		//Создаем объект CMemfile и счтываем в него данные из архива
		CMemFile *file=NULL;
		if(!OpenMemFile(path, &file))
			return false;
		//Загружаем переменные из CMemFile в CVariableMenager
		CVariableMenagerPtr mng;
		mng->Serialize(*file, TRUE);

		//Освобождаем объект CMemFile созданный в функции OpenMemFile()
		if(!FreeMemFile(file))
			return false;
	}
	catch(...){return false;}
	return true;
}

//сохранить переменые проекта
bool CArchiveManager::SaveVariables()
{
	try
	{
		
		CMemFile *file=NULL;
		//Получить полный путь к файлу, содержащего переменные проекта
		wstring path = GetSpecialFolder(SF_VARIABLES)+ L"\\"  + StringFromID(_VARIABLE);

		//Пытаемся создать объект CMemFile
		if(!CreateMemFile(path, &file))
			return false;
		//Записываем данные из CVariableMenager в CMemFile
		CVariableMenagerPtr mng;
		mng->Serialize(*file, FALSE);
		
		//Записываем данные из CMemFile в архив и освобождаем объект CMemFile
		//созданый в CreateMemFile()
		if(!WriteAndFreeMemFile(file))
			return false;
	}
	catch(...){return false;}
	return true;

}

//Загрузка действий для объектов из схемы SchemaName
bool CArchiveManager::LoadActions(wstring SchemaName)
{
	try
	{
		//получаем полный путь к файлу, содержащего действия
		wstring path = GetSpecialFolder(SF_SCHEMAS) + L"\\" + SchemaName + L"\\" + StringFromID(_ACTION);
		//Создаем объект CMemfile и счтываем в него данные из архива
		CMemFile *file=NULL;
		if(!OpenMemFile(path, &file))
			return false;
		//Загружаем действия из MemFile в ActionMenager
		CActionMenagerPtr mng;
		mng->Serialize(*file);

		//Освобождаем объект CMemFile созданный в функции OpenMemFile()
		if(!FreeMemFile(file))
			return false;
	}
	catch(...){return false;}
	return true;
}


//Сохранение объектов из схемы SchemaName имеющих тип TypeID
bool CArchiveManager::SaveSchemaObj(wstring SchemaName, UINT TypeID)
{
	try
	{
		typedef list<CObject*> TObjList;
		typedef TObjList::iterator TObjListIt;
		TObjList ObjList;

		//Создаем список из объектов содержащихся в CObjectMenager
		//имеющих тип TypeID
		for(TObjListIt it = _Menager->_Objects.begin(); 
			it != _Menager->_Objects.end(); it++)
		{
			if((*it)->GetTypeID()==TypeID)
				ObjList.push_back(*it);
		}

		DWORD count = (DWORD)ObjList.size();
		
		wstring path = GetSpecialFolder(SF_SCHEMAS)+ L"\\" + SchemaName + L"\\" + StringFromID(TypeID);
		
		//Пытаемся создать CMemFile
		CMemFile *file=NULL;
		if(!CreateMemFile(path, &file))
			return false;
		//Записываем количество объектов в CMemfile
		(*file)<<count;
		//сохраняем объекты в CMemFile
		for(TObjListIt it=ObjList.begin(); it!=ObjList.end(); it++)
		{
			(*it)->Serialize(*file, FALSE);
		}
		//Записываем CMemFile в архив и освобождаем CMemFile
		if(!WriteAndFreeMemFile(file))
			return false;
	}
	catch(...){return false;}

	return true;
}

//Сохранение настроек и объектов схемы 
bool CArchiveManager::SaveSchema(wstring SchemaName, sSchemaSettings Settings)
{
	SaveSchemaSettings(SchemaName,Settings);
	SaveAllObjects(SchemaName);
	return true;
}

//Загрузка настроек схемы
bool CArchiveManager::LoadSchemaSettings(wstring SchemaName, sSchemaSettings& Settings)
{
	try
	{
		wstring path = GetSpecialFolder(SF_SCHEMAS) + L"\\" + SchemaName + L"\\" + L"Settings";
		CMemFile *file=NULL;
		if(!OpenMemFile(path, &file))
		{
			sSchemaSettings stgs;
			Settings = stgs;
			return false;
		}

		(*file)>>Settings.FixedSize;
		(*file)>>Settings.HasBackgrd;
		(*file)>>Settings.cx;
		(*file)>>Settings.cy;
		(*file)>>Settings.BackGrdName;
		(*file)>>Settings.StretchBackGrd;
		(*file)>>Settings.BackGrdColor;


		FreeMemFile(file);
	}
	catch(...){return false;}
	return true;
}

//Сохранение настроек схемы
bool CArchiveManager::SaveSchemaSettings(wstring SchemaName, sSchemaSettings Settings)
{
	try
	{
		wstring path = GetSpecialFolder(SF_SCHEMAS) + L"\\" + SchemaName + L"\\" + L"Settings";
		CMemFile *file=NULL;

		if(!CreateMemFile(path, &file))
			return false;

		(*file)<<Settings.FixedSize;
		(*file)<<Settings.HasBackgrd;
		(*file)<<Settings.cx;
		(*file)<<Settings.cy;
		(*file)<<Settings.BackGrdName;
		(*file)<<Settings.StretchBackGrd;
		(*file)<<Settings.BackGrdColor;

		if(!WriteAndFreeMemFile(file))
			return false;
	}
	catch(...){return false;}
	return true;
}

//Загрузка настроек проекта
bool CArchiveManager::LoadProjectSettings(sProjectSettings& Settings)
{
	try
	{
		wstring path = L"Settings";
		CMemFile *file=NULL;
		if(!OpenMemFile(path, &file))
			return false;

		(*file)>>Settings.InitialSchema;
		(*file)>>Settings.IntervalOPC;

	}
	catch(...){return false;}
	return true;
}

//Сохранение настроек проекта
bool CArchiveManager::SaveProjectSettings(sProjectSettings Settings)
{
	try
	{
		wstring path = L"Settings";
		CMemFile *file=NULL;

		if(!CreateMemFile(path, &file))
			return false;

		(*file)<<Settings.InitialSchema;
		(*file)<<Settings.IntervalOPC;

		if(!WriteAndFreeMemFile(file))
			return false;
	}
	catch(...){return false;}
	return true;
}

//Сохранение всех объектов схемы
bool CArchiveManager::SaveAllObjects(wstring SchemaName)
{
	SaveSchemaObj(SchemaName, _RECTANGLE);
	SaveSchemaObj(SchemaName, _POLYLINE);
	SaveSchemaObj(SchemaName, _ELLIPSE);
	SaveSchemaObj(SchemaName, _IMAGE);
	SaveSchemaObj(SchemaName, _TEXT);
	SaveSchemaObj(SchemaName, _BUTTON);
	SaveSchemaObj(SchemaName, _INPUT);
	SaveSchemaObj(SchemaName, _SOUND);
	SaveSchemaObj(SchemaName, _GRAPH);

	SaveActions(SchemaName);
	
	return true;
}
 
//Удаление всех объектов из CObjectMenager и всех действий из CActionMenager
bool CArchiveManager::RemoveAllObjects()
{
	_Menager->RemoveAll();
	CActionMenagerPtr mng;
	mng->RemoveAll();
	return true;
}

//Создание нового архива
bool CArchiveManager::CreateNewArchive()
{
	if(_FileName.empty())
		return false;

	CZipArchive arch;
	arch.Open((LPCTSTR)_FileName.c_str(),  CZipArchive::zipCreate);
	arch.Close();
	return true;
}

//Получить список файлов из папки Fldr
bool CArchiveManager::GetDirectoryFileList(TStrings& Names, enSpecialFolder Fldr)
{
	if(Fldr != SF_IMAGES && Fldr != SF_SCHEMAS && Fldr != SF_SOUNDS)
		return false;

	CZipArchive arch;
	CZipMemFile mf;
	try
	{
		if(PathFileExists(_FileName.c_str()) == false)
			return false;
		//Открываем архив
		arch.Open(_FileName.c_str(), CZipArchive::zipOpenReadOnly);

		//Получаем список всех файлов в архиве
		CZipWordArray  array;
		arch.FindMatches(L"*", array, true);

		int size = array.GetSize();

		//Выделяем перечень названий файлов из папки Fldr
		for(int i = 0; i< size; i++)
		{
			CZipFileHeader header;
			arch.GetFileInfo(header, array[i]);
			wstring str = header.GetFileName();	
			//Просматриваем только содержимое папки Fldr
			if(str.find(GetSpecialFolder(Fldr)+L"\\") != 0)
				continue; //Это другая папка
			wstring::iterator it = str.begin();
			while(it!=str.end()&&*it != L'\\'  ) it++;
			if(it!=str.end())
				it++;
			wstring name;
			while(it!=str.end()&&*it != L'\\' )
			{
				name += *it;
				it++;
			}
			//Исключаем дублирование
			TStrings::iterator find = 
				find_if(Names.begin(), Names.end(), sFindName(name));
			if(find == Names.end() && !name.empty()) 
			{
				Names.push_back(name);
			}
		}
		sort(Names.begin(), Names.end());
	}
	catch (...){};

	arch.Close();
	return true;
}


//Создание CMemFile
bool CArchiveManager::CreateMemFile(wstring FileName, CMemFile** file)
{
	CZipArchive arch;
	CZipMemFile mf;

	try
	{
		if(!PathFileExists(_FileName.c_str()))
			return false;
		//
		arch.Open((LPCTSTR)_FileName.c_str(), CZipArchive::zipOpen);
		int index = arch.FindFile((LPCTSTR)FileName.c_str());
		//Файл уже существует в архиве, удаляем его
		if(index != -1) 
			arch.DeleteFileW(index);
		
		//Создаем новый CMemFile
		*file = new CMemFile(FileName, this);
		if(*file == NULL)
			return false;
	}
	catch(...){return false;};

	mf.Close();
	arch.Close();

	return true;
}

//Проверка существует ли файл  в архиве
bool CArchiveManager::IsFileExists(wstring FileName)
{
	CZipArchive arch;
	CZipMemFile mf;

	try
	{
		if(!PathFileExists(_FileName.c_str()))
			return false;

		arch.Open((LPCTSTR)_FileName.c_str(), CZipArchive::zipOpen);
		int index = arch.FindFile((LPCTSTR)FileName.c_str());
		mf.Close();
		arch.Close();
		return index != -1;
	}
	catch(...){return false;};
	return false;
}

//Создаем CMemFile и считываем в него данные из архива
bool CArchiveManager::OpenMemFile(wstring FileName, CMemFile** file)
{
	CZipArchive arch;
	CZipMemFile mf;

	try
	{
		if(!PathFileExists(_FileName.c_str()))
			return false;

		arch.Open((LPCTSTR)_FileName.c_str(), CZipArchive::zipOpen);
		int index = arch.FindFile((LPCTSTR)FileName.c_str());
		if(index == -1) return false;

		arch.ExtractFile(index, mf);

		*file = new CMemFile(FileName, this);
		if(*file == NULL)
			return false;

		vector<BYTE> tmp;
		tmp.resize(mf.GetLength());
		mf.Read(&tmp[0], mf.GetLength());
		(*file)->WriteBlock(&tmp[0], (UINT)tmp.size());
		(*file)->Seek(0, SEEK_SET);
	}
	catch(...){return false;};

	mf.Close();
	arch.Close();

	return true;
}

//Записываем данные из CMemFile в архив и удаляем объект CMemFile
bool CArchiveManager::WriteAndFreeMemFile(CMemFile* file)
{
	if(file == NULL)
		return false;

	CZipArchive arch;
	CZipMemFile mf;

	try
	{
		if(!PathFileExists(_FileName.c_str()))
			return false;

		arch.Open((LPCTSTR)_FileName.c_str(), CZipArchive::zipOpen);
		int index = arch.FindFile(file->GetFileName().c_str());
		//Если файл с таким именем уже есть в архиве, удаляем его
		if(index != -1)
			arch.DeleteFileW(index);
		
		file->Seek(0,SEEK_SET);
		vector<BYTE> tmp(file->GetSize());
        file->ReadBlock(&tmp[0], file->GetSize());
		mf.Write(&tmp[0], (UINT)tmp.size());
		//Записываем данные в архив
		arch.AddNewFile(mf, file->GetFileName().c_str(), Z_BEST_COMPRESSION);
		delete file;
		file = NULL;
	}
	catch(...)
	{
		if(file)
		{
			delete file;
			file = NULL;
		}
		return false;
	};

	mf.Close();
	arch.Close();

	return true;
}

bool CArchiveManager::FreeMemFile(CMemFile* file)
{
	if(file == NULL)
		return false;

	delete file;
	return true;
}

bool CArchiveManager::WriteAsAndFreeMemFile(wstring FileName, CMemFile* file)
{
	if(file == NULL)
		return false;

	CZipArchive arch;
	CZipMemFile mf;

	try
	{
		if(!PathFileExists(_FileName.c_str()))
			return false;

		arch.Open((LPCTSTR)_FileName.c_str(), CZipArchive::zipOpen);
		int index = arch.FindFile(FileName.c_str());
		if(index != -1)
			arch.DeleteFileW(index);

		file->Seek(0,SEEK_SET);
		vector<BYTE> tmp(file->GetSize());
		file->ReadBlock(&tmp[0], file->GetSize());
		mf.Write(&tmp[0], (UINT)tmp.size());

		arch.AddNewFile(mf, FileName.c_str(), Z_BEST_COMPRESSION);
		delete file;
		file = NULL;
	}
	catch(...)
	{
		if(file)
		{
			delete file;
			file = NULL;
		}
		return false;
	};

	mf.Close();
	arch.Close();

	return true;
}

//Удалить файл из архива
bool CArchiveManager::DeleteFile(wstring FileName)
{
	CZipArchive arch;

	try
	{
		if(!PathFileExists(_FileName.c_str()))
			return false;

		arch.Open((LPCTSTR)_FileName.c_str(), CZipArchive::zipOpen);
		int index = arch.FindFile(FileName.c_str());
		if(index != -1)
			arch.DeleteFileW(index);
	}
	catch(...)
	{
		return false;
	};

	arch.Close();
	return true;
}

//Получить строковое название папки по ее ID
wstring CArchiveManager::GetSpecialFolder(enSpecialFolder Fldr)
{
	switch(Fldr) 
	{
	case SF_SCHEMAS: return L"Schemas";
	case SF_IMAGES: return L"Images";
	case SF_SOUNDS: return L"Sounds";
	case SF_VARIABLES: return L"Variables";
	}
	return L"";
}

//Удалить папку с именем FolderName находящуюся в папке Fldr
bool CArchiveManager::DeleteFolder(enSpecialFolder Fldr, wstring FolderName)
{
	CZipArchive arch;

	try
	{
		if(!PathFileExists(_FileName.c_str()))
			return false;
		
		FolderName =  GetSpecialFolder(Fldr) + L"\\" + FolderName;
		arch.Open((LPCTSTR)_FileName.c_str(), CZipArchive::zipOpen);
		CZipWordArray array;
		arch.FindMatches( L"*", array, false);
		int count = array.GetSize();

		TStrings Names;
		for(int i = 0; i< count; i++)
		{
			CZipFileHeader header;
			arch.GetFileInfo(header, array[i]);
			wstring str = header.GetFileName();

			if(str.substr(0, FolderName.length()) == FolderName)
			{
				Names.push_back(str);
			}
		}

		for(TStrings::iterator it = Names.begin(); it != Names.end(); it++)
		{
			int index = arch.FindFile(it->c_str());
			if(index != -1)
				arch.DeleteFileW(index);
		}
	}
	catch(...)
	{
		return false;
	}

	arch.Close();
	return true;
}

//Получить картинку 
bool CArchiveManager::GetImage(wstring ImageName, ::CImage& Image)
{
	try
	{
		wstring path = GetSpecialFolder(SF_IMAGES) + L"\\" + ImageName;

		CMemFile *file=NULL;
		if(!OpenMemFile(path, &file))
			return false;

		::CImage img;
		img.Load(*file);
		FreeMemFile(file);

		Image = img;
	}
	catch(...){return false;}

	return true;
}