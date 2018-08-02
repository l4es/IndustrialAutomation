#include "Common.h"
#include "image.h"

CImage::CImage(void)
{
	//Инициализация переменных
	_Width = _Height = 0;
}
CImage::CImage(const CImage& src)
{
	_Width	= src._Width;
	_Height	= src._Height;
	_Data	= src._Data;
}

CImage::~CImage(void)
{
}

CImage& CImage::operator=(CImage& src)
{
	_Width	= src._Width;
	_Height	= src._Height;
	_Data	= src._Data;
	return *this;
}

//Загрузить изображение
bool CImage::Load(CMemFile& file)
{
	//Считать ID файла
	file.Seek(0,SEEK_SET);
	DWORD ID;
	file>>ID;
	if(DECODE_IMAGE_VER(ID) != IMAGE_FILE_VER)
		return false; //Если неизвестное ID - выходим
	
	//Считываем изображение
	file>>_Width;
	file>>_Height;
	_Data.resize(_Width*_Height);
	file.ReadBlock(&_Data[0], _Width*_Height*sizeof(TImgData::value_type));
	return true;
}

//Сохранить изображение
bool CImage::Save(CMemFile& file)
{
	file.Seek(0,SEEK_SET);
	DWORD ID = IMAGE_ID;
	file<<ID;
	file<<_Width;
	file<<_Height;
	file.WriteBlock(&_Data[0], _Width*_Height*sizeof(TImgData::value_type));
	return true;
}

//Нарисовать изображение на DC
void CImage::DrawOnDC(HDC dc)
{
	TImgData::iterator it = _Data.begin();
	for(WORD j=0;j<_Height;j++)
	for(WORD i=0;i<_Width;i++)
	{
		SetPixel(dc,i, j, it->AsCOLORREF());
		it++;
	}
}

//Скопировать изображение из DC в локальный формат
void CImage::CopyFromDC(HDC dc, CRect& rc)
{
	_Width = rc.Width();
	_Height= rc.Height();
	_Data.resize(_Width*_Height);
	TImgData::iterator it = _Data.begin();
	for(int j=rc.top;j<rc.bottom;j++)
	for(int i=rc.left;i<rc.right;i++)
	{
		*it = GetPixel(dc,i,j);
		it++;
	}
}

//Загрузить изображение стандартного формата
bool CImage::LoadFromStdFile(wstring FileName)
{
	//Инициализация FreeImage
	FIBITMAP *dib = NULL;
	FreeImage_Initialise();

	//Загрузка файла
	FREE_IMAGE_FORMAT fmt = FreeImage_GetFileType(hl::conv_wstr_to_str(FileName).c_str());
	if(fmt != FIF_UNKNOWN)
		dib = FreeImage_Load(fmt, hl::conv_wstr_to_str(FileName).c_str());
	if(dib == NULL)
	{
		TRACE(L"Ошибка загрузки файла\n");
		return false;
	}

	//Конвертация в 32BPP
	FIBITMAP* ptr = FreeImage_ConvertTo32Bits(dib);
	if(ptr != NULL)
		FreeImage_Unload(dib); //выгружаем исходное изображение
	else
		ptr = dib; //Исходное изображение уже в 32BPP

	//Получить ширину/высоту изображение
	_Width		=		FreeImage_GetWidth(ptr);
	_Height		=		FreeImage_GetHeight(ptr);
	
	//Расчет с какого бита начинается цветовая состовляющая
	byte	RShift = 0, 
			GShift = 0, 
			BShift = 0;
	DWORD tmp;
	
	tmp = FreeImage_GetRedMask(ptr);
	while(!(tmp & 1)){ tmp >>= 1; RShift++;	}

	tmp = FreeImage_GetGreenMask(ptr);
	while(!(tmp & 1)){ tmp >>= 1; GShift++; }

	tmp = FreeImage_GetBlueMask(ptr);
	while(!(tmp & 1)){ tmp >>= 1; BShift++; }

	//Резервируем память в массиве под изображение
	_Data.clear();
	_Data.reserve(_Height*_Width);

	//Заполняем массив
	sRGB rgb;
	for(int i = _Height-1; i >= 0; i--) //Сканлинии начинаются с конца
	{		
		DWORD* line =  (DWORD*)FreeImage_GetScanLine(ptr,i);
		for(int j = 0; j < _Width; j++)
		{
			//Преобразование цвета
			rgb.r = (*line >> RShift) & 0xff;
			rgb.g = (*line >> GShift) & 0xff;
			rgb.b = (*line >> BShift) & 0xff;

			_Data.push_back(rgb);	//Добавляем пиксель
			line++;	//Следующий пиксеть в сканлинии
		}
	}

	//Освобождаем память
	FreeImage_Unload(ptr);
	FreeImage_DeInitialise();

	return true;
}