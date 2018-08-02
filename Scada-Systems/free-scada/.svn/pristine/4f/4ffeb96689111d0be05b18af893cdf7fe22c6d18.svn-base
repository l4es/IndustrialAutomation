#pragma once

/********************************************************************
Создан:	        2005/02/07
Создан:	        7.02.2005   13:40
Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\ObjImage.h
Директория:	    d:\Work\Projects\М-Немо\Source\Library
Имя файла:	    ObjImage
Расширение:	    h
Автор(ы):	    Алексей, Михаил
Класс(ы):       CObjImage

Описание:	Класс реализует рисование объекта изображения
*********************************************************************/

#include "object.h"
#include "Image.h"

class CObjImage
	: public CObject
{
public:
	CObjImage(void);
	~CObjImage(void);

	//Рисование картинки
	virtual void Draw(HDC dc_); 
	//Сохранение-загрузка объекта
	virtual void Serialize(CMemFile& file, BOOL IsLoading = 1);
	//Событие окончания редактирования
	virtual void OnEndEdit();
	//Событие изменения объекта
	virtual void OnEdit();
	//Загрузка картинки 
	bool LoadImage(CImage& Image);
	//Поворот картинки, заполнение _Bitmap
	bool RotateImage(float Angle);
	//Округление угла с точность до 90 градусов
	float RoundAngle(float angle);
	//Получить имя изображения
	wstring GetImageName();
	//Установить имя изображения
	bool SetImageName(wstring Name);
	//Получить цвет рамки
	inline sRGB GetBorderColor(){return _BorderColor;};
	//Установить цвет рамки
	inline void SetBorderColor(sRGB Color){_BorderColor = Color;};
	//Установить рисовать рамку или нет
	inline void DrawBorder(bool Draw=true){_DrawBorder = Draw;};
	//Определить рисуется ли рамка
	inline bool IsDrawBorder(){return _DrawBorder;};
	//Установить толщину линий рамки
	inline void SetBorderWidth(long Width){_BorderWidth = Width;};
	//Получить толщину линий рамки
	inline long GetBorderWidth(){return _BorderWidth;};
	//Определить растягивается/сжимается ли изображение до размеров рамки
	bool GetStretchImage(){ return _StretchImage; }
	//Установить растягиваеть/сжимаеть изображение до размеров рамки
	void SetStretchImage(bool Stretch){ _StretchImage = Stretch; }

protected:

	CImage	_Image;
	
	CBitmap	_Bitmap;
	bool	_IsImage;
	bool	_MouseDown;
	float	_PredAngle;
	wstring	_ImageName;

	sRGB	_BorderColor;
	bool	_DrawBorder;
	long	_BorderWidth;

	bool   _StretchImage;
};
