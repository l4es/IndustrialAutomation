#include "MemFile.h"
#include "common.h"
#include "ObjectMenager.h"
#include ".\objimage.h"
#include "Action.h"


CObjImage::CObjImage(void)
{
	SetColor(128,128,128);
	MoveTo(0,0);
	_IsImage = false;
	_DrawBorder = true;
	_BorderWidth = 1;
	_BorderColor = sRGB(0,0,0);
	_MouseDown = false;
	_PredAngle = 0;
	_StretchImage = true;
	SetTypeID(_IMAGE);
	SetName(GenerateUniqueObjectName(L"Image_"));
	_ImageName = L"";
	SetActionFlags(AT_ALL_VISUAL|AT_CHANGE_IMAGE);
}

CObjImage::~CObjImage(void)
{
}


void CObjImage::Serialize(CMemFile& file, BOOL IsLoading)
{
	if(IsLoading)
	{
		file>>_ImageName;
		file>>_BorderColor;
		file>>_BorderWidth;
		file>>_DrawBorder;

		CObject::Serialize(file, IsLoading);
		
		//Загружаем изображение
		CArchiveManager* arch = file.GetArchive();
		wstring FileName = arch->GetSpecialFolder(CArchiveManager::SF_IMAGES) + L"\\" + GetImageName();
		if(arch->IsFileExists(FileName))
		{
			CMemFile* memfile = NULL;
			arch->OpenMemFile(FileName, &memfile);
			::CImage image;
			image.Load(*memfile);
			LoadImage(image);
			arch->FreeMemFile(memfile);
		}
		return;
	}
	else
	{
		file<<_ImageName;
		file<<_BorderColor;
		file<<_BorderWidth;
		file<<_DrawBorder;
	}
	CObject::Serialize(file, IsLoading);
}

//Рисование картинки
void CObjImage::Draw(HDC dc_)
{
	if(!IsVisible())
		return;

	CDCHandle dc(dc_);

	sVector pos = sVector((int)GetPosition().x, (int)GetPosition().y);
	sVector size = sVector((int)GetSize().x, (int)GetSize().y);
	sVector center = pos + size/2;
	float objAngle = GetAngle();

	//Угловые точки овъекта
	sVector points[4];
	points[0] = pos - center;
	points[1] = sVector((pos+size).x, pos.y) - center;
	points[2] = pos + size - center;
	points[3] = sVector(pos.x, (pos+size).y) - center;

	CObjectMenagerPtr mng;
	for(int i = 0; i <= 3; i++)
	{
		points[i].Rotate(objAngle);
		points[i] += center;
		points[i] = mng->GlobalToScreen(points[i]);
	}

	//Карандаш для пунктирной рамки
	CPen TrackPen;
	TrackPen.CreatePen(PS_DOT, 0, RGB(0,0,0));
	CPenHandle pold = dc.SelectPen(TrackPen);

	//Если угол не близок к кратному 90 грд рисуем пунктирную рамку
	if(int(10*(objAngle - RoundAngle(objAngle))) != 0)
	{
 		dc.MoveTo(points[3].x, points[3].y);
		for(int i = 0; i <= 3; i++)
		{
			dc.LineTo(points[i].x, points[i].y);
		}
	}

	int _BitX=0, _BitY=0, _ImgX=0, _ImgY=0; 
	//определяем ближайший угол кратный 90 градусов
	float angle = RoundAngle(GetAngle());

	//оределяем размеры изображения и рамки куда это изображение нужно вписать
	if((angle == 0) || (angle == PI) || (angle == 2*PI) || (angle == -PI))
	{
		_BitX = size.x;
		_BitY = size.y;
		_ImgX = _Image._Width;
		_ImgY = _Image._Height;
	}

	if((angle == PI/2) || (angle == -PI/2) || (angle == 3*PI/2) || (angle == -3*PI/2))
	{
		_BitX = size.y;
		_BitY = size.x;
		_ImgX = _Image._Height;
		_ImgY = _Image._Width;
		pos.x = center.x - size.y/2;
		pos.y = center.y - size.x/2;
		float tmp = size.x;
		size.x = size.y;
		size.y = tmp;
	}

	CDC bitDC;
	bitDC.CreateCompatibleDC(dc);
	CBitmapHandle bold = bitDC.SelectBitmap(_Bitmap);
	CObjectMenagerPtr om;
	pos = om->GlobalToScreen(pos);

	//Если растягивать/сжимать изображение
	if(_StretchImage)
	{
		SetStretchBltMode(dc, HALFTONE);
		dc.StretchBlt(pos.x, pos.y, _BitX, _BitY, bitDC,
			0, 0, _ImgX, _ImgY, SRCCOPY );
	}
	//Рисовать изображение без масштабирования
	else
	{
		dc.BitBlt(pos.x, pos.y, _BitX, _BitY, bitDC, 0, 0, SRCCOPY );
	}

	bitDC.SelectBitmap(bold);

	//Если рисовать рамку
	if(_DrawBorder)
	{
		CPen _Pen;
		_Pen.CreatePen(PS_SOLID, _BorderWidth, _BorderColor.AsCOLORREF());
		dc.SelectPen(_Pen);

	
		dc.MoveTo(points[3].x, points[3].y);
		for(int i = 0; i <= 3; i++)
		{
			dc.LineTo(points[i].x, points[i].y);
		}
	}

	dc.SelectPen(pold);
	
}

//Округление угла с точность до 90 градусов
float CObjImage::RoundAngle(float angle)
{
	float ANGLE = 0;
	if((angle >= -PI/4) && (angle <= PI/4)) ANGLE = 0;
	if((angle > PI/4) && (angle <= 3*PI/4)) ANGLE = PI/2;
	if((angle > 3*PI/4) && (angle <= 5*PI/4)) ANGLE = PI;
	if((angle > 5*PI/4) && (angle <= 7*PI/4)) ANGLE = 3*PI/2;
	if((angle > 7*PI/4) && (angle > -PI/4)) ANGLE = 2*PI;
	if((angle < -PI/4) && (angle >= -3*PI/4)) ANGLE = -PI/2;
	if((angle < -3*PI/4) && (angle >= -5*PI/4)) ANGLE = -PI;
	if((angle < -5*PI/4) && (angle >= -7*PI/4)) ANGLE = -3*PI/2;
	return ANGLE;
}

//Событие окончания редактирования
void CObjImage::OnEndEdit()
{
	Rotate(RoundAngle(GetAngle()));
}

//Событие изменения объекта
void CObjImage::OnEdit()
{
	float angle = RoundAngle(GetAngle());
	
	if(angle == _PredAngle)
	{
		return;
	}

	_PredAngle = angle;

	RotateImage(angle);
}

//Загрузка картинки 
bool CObjImage::LoadImage(CImage& Image)
{
	_Image = Image;

	
	float koef = (float)_Image._Width/_Image._Height;
	if(koef >= 1)
		SetSize(koef*GetSize().y, GetSize().y);
	else
		SetSize(GetSize().x, GetSize().x/koef);
    
	RotateImage(RoundAngle(GetAngle()));
	return true;
}

//Поворот картинки, заполнение _Bitmap
bool CObjImage::RotateImage(float Angle)
{
	CImage::TColorVector RGBVector;
	//Заполнение вектора vector<COLORREF>
	for(CImage::TImgData::iterator it = _Image._Data.begin();
		it != _Image._Data.end(); it++)
	{
		COLORREF clr = it->AsCOLORREF();
		RGBVector.push_back(clr);
	}

	if(RGBVector.empty())
		return false;

	int ImgX = _Image._Width;
	int ImgY = _Image._Height;

	if(!_Bitmap.IsNull())
		_Bitmap.DeleteObject();

	HWND hwnd = ::GetDesktopWindow();
	CDCHandle dc = ::GetDC(hwnd);

	CDC dc_;
	dc_.CreateCompatibleDC(dc);
	
	CBitmapHandle old_bmp;

	//Определяем цветовую маску
	COLORREF pixel;
	float fr = ((float)GetColor().r-128.0f)/255.0f;
	float fg = ((float)GetColor().g-128.0f)/255.0f;
	float fb = ((float)GetColor().b-128.0f)/255.0f;

	//Заполняем _Bitmap в соответствии с углом
	if((Angle == 0) || (Angle == 2*PI))
	{
		_Bitmap.CreateCompatibleBitmap(dc, ImgX, ImgY);
		old_bmp = dc_.SelectBitmap(_Bitmap);
		
		
		for(int i = 0; i < _Image._Height; i++)
			for(int j = 0; j < _Image._Width; j++)
			{
				pixel = RGBVector[i*_Image._Width + j];

				float rv = (float)GetRValue(pixel)/255.0f+fr;
				float gv = (float)GetGValue(pixel)/255.0f+fg;
				float bv = (float)GetBValue(pixel)/255.0f+fb;
				//Наложение цветовой маски
				rv = max(0.0f,rv);
				rv = min(1.0f,rv);
				gv = max(0.0f,gv);
				gv = min(1.0f,gv);
				bv = max(0.0f,bv);
				bv = min(1.0f,bv);

				dc_.SetPixel(j, i, RGB(	rv*255, gv*255, bv*255));
			}
	}
	else 
		if((Angle == PI) || (Angle == -PI))
		{
			_Bitmap.CreateCompatibleBitmap(dc, ImgX, ImgY);
			old_bmp = dc_.SelectBitmap(_Bitmap);

			reverse(RGBVector.begin(), RGBVector.end());
			for(int i = 0; i < _Image._Height; i++)
				for(int j = 0; j < _Image._Width; j++)
				{
					pixel = RGBVector[i*_Image._Width + j];
					float rv = (float)GetRValue(pixel)/255.0f+fr;
					float gv = (float)GetGValue(pixel)/255.0f+fg;
					float bv = (float)GetBValue(pixel)/255.0f+fb;

					rv = max(0.0f,rv);
					rv = min(1.0f,rv);
					gv = max(0.0f,gv);
					gv = min(1.0f,gv);
					bv = max(0.0f,bv);
					bv = min(1.0f,bv);

					dc_.SetPixel(j, i, RGB(	rv*255, gv*255, bv*255));
				}
		}
		else
			if((Angle == PI/2) || (Angle == -3*PI/2))
			{
				_Bitmap.CreateCompatibleBitmap(dc, ImgY, ImgX);
				old_bmp = dc_.SelectBitmap(_Bitmap);
				for(int i = 0; i < _Image._Width; i++)
					for(int j = 0; j < _Image._Height; j++)
					{
						pixel = RGBVector[(_Image._Height-j-1)*_Image._Width + i];
						
						float rv = (float)GetRValue(pixel)/255.0f+fr;
						float gv = (float)GetGValue(pixel)/255.0f+fg;
						float bv = (float)GetBValue(pixel)/255.0f+fb;

						rv = max(0.0f,rv);
						rv = min(1.0f,rv);
						gv = max(0.0f,gv);
						gv = min(1.0f,gv);
						bv = max(0.0f,bv);
						bv = min(1.0f,bv);

						dc_.SetPixel(j, i, RGB(	rv*255, gv*255, bv*255));
					}
				
			}
			else
				if((Angle == 3*PI/2) || (Angle == -PI/2))
				{
					_Bitmap.CreateCompatibleBitmap(dc, ImgY, ImgX);
					old_bmp = dc_.SelectBitmap(_Bitmap);
					for(int i = 0; i < _Image._Width; i++)
						for(int j = 0; j < _Image._Height; j++)
						{
							pixel = RGBVector[j*_Image._Width + (_Image._Width -i-1)];

							float rv = (float)GetRValue(pixel)/255.0f+fr;
							float gv = (float)GetGValue(pixel)/255.0f+fg;
							float bv = (float)GetBValue(pixel)/255.0f+fb;

							rv = max(0.0f,rv);
							rv = min(1.0f,rv);
							gv = max(0.0f,gv);
							gv = min(1.0f,gv);
							bv = max(0.0f,bv);
							bv = min(1.0f,bv);

							dc_.SetPixel(j, i, RGB(	rv*255, gv*255, bv*255));
						}
				}
				else
				{
					_Bitmap.CreateCompatibleBitmap(dc, ImgX, ImgY);
					old_bmp = dc_.SelectBitmap(_Bitmap);
					for(int i = 0; i < _Image._Height; i++)
						for(int j = 0; j < _Image._Width; j++)
						{
							pixel = RGBVector[i*_Image._Width + j];

							float rv = (float)GetRValue(pixel)/255.0f+fr;
							float gv = (float)GetGValue(pixel)/255.0f+fg;
							float bv = (float)GetBValue(pixel)/255.0f+fb;

							rv = max(0.0f,rv);
							rv = min(1.0f,rv);
							gv = max(0.0f,gv);
							gv = min(1.0f,gv);
							bv = max(0.0f,bv);
							bv = min(1.0f,bv);

							dc_.SetPixel(j, i, RGB(	rv*255, gv*255, bv*255));
						}
				}
					


	dc_.SelectBitmap(old_bmp);

	_IsImage = true;

	::ReleaseDC(hwnd, dc);
	return true;
}


//Получить имя изображения
wstring CObjImage::GetImageName()
{
	return _ImageName;
}

//Установить имя изображения
bool CObjImage::SetImageName(wstring Name)
{
	_ImageName = Name;
	return true;
}

