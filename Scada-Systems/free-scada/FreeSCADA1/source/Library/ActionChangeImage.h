/********************************************************************
	Создан:			2005/05/19
	Создан:			19:5:2005   10:42
	Путь к файлу:	d:\Work\Projects\М-Немо\trunk\Source\Library\ActionChangeImage.h
	Директория:		d:\Work\Projects\М-Немо\trunk\Source\Library
	Имя файла:		ActionChangeImage
	Расширение:		h
	Автор(ы):		Михаил
	Класс(ы):       CActionColor
	
	Описание: Данный action изменяет изображение объекта "Image" в зависимости 
	от значения выражения.
*********************************************************************/
#pragma once

#include "action.h"
#include "Object.h"

class CActionChangeImage
	: public CAction
{
public:
	//Конструктор/деструктор
	CActionChangeImage(CObject* Owner_, CArchiveManager* am);
	~CActionChangeImage(void);

	//Функция "обновления"
	virtual void Update();

	//Установить первое изображение
	inline void SetImage1(wstring Image){_Image1 = Image;};
	//Получить первое изображение
	inline wstring GetImage1(){return _Image1;};

	//Установить второе изображение
	inline void SetImage2(wstring Image){_Image2 = Image;};
	//Получить второе изображение
	inline wstring GetImage2(){return _Image2;};

	//Функция загрузки/сохранения
	virtual void Serialize(CMemFile& file, BOOL IsLoading = TRUE);

	//Получить значение параметра экшена по его символьному названию
	virtual bool GetParamValue(wstring Name, CComVariant& Variant);
	//Установить значение параметра экшена по его символьному названию
	virtual bool SetParamValue(wstring Name, CComVariant Variant);

	inline CArchiveManager* GetArchiveMenager(){return _ArchiveMenager;};
protected:
	wstring _Image1;
	wstring _Image2;
	CArchiveManager *_ArchiveMenager;
};
