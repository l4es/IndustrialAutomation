#pragma once
/********************************************************************
	Создан:	        2005/02/07
	Создан:	        7.2.2005   16:08
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\MemFile.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Library
	Имя файла:	    MemFile
	Расширение:	    h
	Автор(ы):	    Алексей, Михаил
    Класс(ы):       CMemFile
	
	Виртуальный файл. Главная идея в следующем. Файлы проекта хранятся
в едином архиве в запакованном виде (ZIP). Для работы с ним его необходимо
предварительно распаковать.
	Этот класс представляет собой обертку вокруг буффера данных, полученных из
CArchiveMenager.
*********************************************************************/

#include "ArchiveManager.h"

class CMemFile
{
	//Создать инстанцию этого объекта может только CArchiveManager
	//Удалить может так же только он.
	friend class CArchiveManager;
protected:
	//Закрытые конструктор/деструктор
	CMemFile(wstring FileName, CArchiveManager* Archive)
		:_Size(0)
		,_Pos(0)
		,_FileName(FileName)
		,_Archive(Archive){}
	~CMemFile(){  }

	template<typename T> inline void write_simple_type(const T& value)
	{
		WriteBlock(static_cast<void*>(const_cast<T*>(&value)), sizeof(T));
	}
	template<typename T> inline void read_simple_type(T& value)
	{
		ReadBlock(static_cast<void*>(&value), sizeof(T));
	}

public:
	//Читать буффер из файла. Функция возвращает кол-во прочитаных байт.
	size_t ReadBlock(void* Buffer, size_t Count);
	//Записать буффер в файл
	void WriteBlock(void* Buffer, size_t Count);

	//Перейти в указанную позицию
	//SEEK_SET - от начала
	//SEEK_CUR - от текущей позиции
	//SEEK_END - от конца
	void Seek(UINT Offset, UINT From);

	//Получить размер файла
	inline size_t GetSize(){ return _Size; }

	//Получить имя файла
	inline const wstring GetFileName(){return _FileName;};

	inline CArchiveManager* GetArchive(){return _Archive;};

	CMemFile& operator<<(const std::wstring& str);
	CMemFile& operator<<(const sRGB& value){write_simple_type<sRGB>(value);return *this;};
	CMemFile& operator<<(const CComVariant& value){write_simple_type<CComVariant>(value);return *this;};
	CMemFile& operator<<(const GUID& value){write_simple_type<GUID>(value);return *this;};
	CMemFile& operator<<(const sVector& value){write_simple_type<sVector>(value);return *this;};
	CMemFile& operator<<(long value){write_simple_type<long>(value);return *this;};
	CMemFile& operator<<(bool value){write_simple_type<bool>(value);return *this;};
	CMemFile& operator<<(float value){write_simple_type<float>(value);return *this;};
	CMemFile& operator<<(DWORD value){write_simple_type<DWORD>(value);return *this;};
	CMemFile& operator<<(WORD value){write_simple_type<WORD>(value);return *this;};

	CMemFile& operator>>(std::wstring& str);
	CMemFile& operator>>(sRGB& value){read_simple_type<sRGB>(value);return *this;};
	CMemFile& operator>>(CComVariant& value){read_simple_type<CComVariant>(value);return *this;};
	CMemFile& operator>>(GUID& value){read_simple_type<GUID>(value);return *this;};
	CMemFile& operator>>(sVector& value){read_simple_type<sVector>(value);return *this;};
	CMemFile& operator>>(long& value){read_simple_type<long>(value);return *this;};
	CMemFile& operator>>(bool& value){read_simple_type<bool>(value);return *this;};
	CMemFile& operator>>(float& value){read_simple_type<float>(value);return *this;};
	CMemFile& operator>>(DWORD& value){read_simple_type<DWORD>(value);return *this;};
	CMemFile& operator>>(WORD& value){read_simple_type<WORD>(value);return *this;};

protected:
	vector<BYTE>		_Buffer;
	size_t				_Pos;
	size_t				_Size;
	wstring				_FileName;
	CArchiveManager*	_Archive;
};
