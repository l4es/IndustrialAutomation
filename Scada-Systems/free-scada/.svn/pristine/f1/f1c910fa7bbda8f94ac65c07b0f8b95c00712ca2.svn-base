#pragma once
/********************************************************************
	Создан:	        2004/12/08
	Создан:	        8.12.2004   11:38
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\Image.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Library
	Имя файла:	    Image
	Расширение:	    h
	Автор(ы):	    Михаил
    Класс(ы):       CImage
	
Описание:	Класс релизует функции для загрузки и сохранения изображений из/в CMemFile
*********************************************************************/



#include "MemFile.h"

#define IMAGE_FILE_VER			1
#define IMAGE_ID				(DWORD)(((0x30+IMAGE_FILE_VER)<<24)|(0x47<<16)|(0x4D<<8)|0x49)
#define DECODE_IMAGE_VER(val)	(BYTE)(((val>>24) & 0xff)-0x30)


class CImage
{
	friend class CObjImage;
public:
	//Constructor/Destructor
	CImage(void);
	CImage(const CImage& src);
	virtual ~CImage(void);
	CImage& operator=(CImage& src);

	//Загрузить изображение
	bool Load(CMemFile& file);
	//Сохранить изображение
	bool Save(CMemFile& file);
	//Загрузить изображение стандартного формата
	bool LoadFromStdFile(wstring FileName);

	//Нарисовать изображение на DC
	void DrawOnDC(HDC dc);
	//Скопировать изображение из DC в собственный формат
	void CopyFromDC(HDC dc, CRect& rc);

	inline WORD GetWidth(){return _Width;};
	inline WORD GetHeight(){return _Height;};

protected:
	typedef vector<sRGB> TImgData;
	typedef vector<COLORREF> TColorVector;

	WORD		_Width;		//Ширина
	WORD		_Height;	//Высота
	TImgData	_Data;		//Данные
};