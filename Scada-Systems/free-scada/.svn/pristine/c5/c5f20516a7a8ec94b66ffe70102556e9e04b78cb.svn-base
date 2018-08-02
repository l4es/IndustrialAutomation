#include "common.h"
#include ".\actionchangeimage.h"
#include "ObjImage.h"

CActionChangeImage::CActionChangeImage(CObject* Owner_, CArchiveManager* am)
	:CAction(Owner_)
{
	//Инициализация объекта
	_Image1.clear();
	_Image2.clear();
	_Type = AT_CHANGE_IMAGE;
	_ArchiveMenager = am;
	wstring name(L"Action_ChangeImage_");
	if(Owner_)
	{
		name.append(Owner_->GetName());	
		name.append(L"_");
		
	}
	SetName(GenerateUniqueActionName(name));

}

CActionChangeImage::~CActionChangeImage(void)
{
}

void CActionChangeImage::Update()
{
	if(	_Owner == NULL)
		return;
	if(_ArchiveMenager == NULL)
		return;

	//Вычисляем значение выражения
	double val = CalcControlValue();

	CObjImage *ImageObj = static_cast<CObjImage*>(_Owner);
	
	wstring NewImage;
	if(val >= GetValueMax())
		NewImage = _Image2;
	else
		NewImage = _Image1;

	if(ImageObj->GetImageName() != NewImage)
	{
		ImageObj->SetImageName(NewImage);

		::CImage image;
		if(_ArchiveMenager->GetImage(NewImage, image))
			ImageObj->LoadImage(image);
	}
}

void CActionChangeImage::Serialize(CMemFile& file, BOOL IsLoading)
{
	CAction::Serialize(file, IsLoading);
	if(IsLoading)
	{
		//Загрузка
		file>>_Image1;
		file>>_Image2;
	}
	else
	{
		//Сохранение
		file<<_Image1;
		file<<_Image2;
	}
}

bool CActionChangeImage::GetParamValue(wstring Name, CComVariant& Variant)
{
	//Получить значение параметра экшена по его символьному названию
	BEGIN_GET_VALUE_MAP(Name, Variant)
		GET_VALUE_STRING(L"Image 1", this->GetImage1().c_str())
		GET_VALUE_STRING(L"Image 2", this->GetImage2().c_str())
	END_GET_VALUE_MAP()

	return CAction::GetParamValue(Name, Variant);
}


bool CActionChangeImage::SetParamValue(wstring Name, CComVariant Variant)
{
	//Установить значение параметра экшена по его символьному названию
	BEGIN_SET_VALUE_MAP(Name, Variant)
		SET_VALUE_STRING(L"Image 1", this->SetImage1(val))
		SET_VALUE_STRING(L"Image 2", this->SetImage2(val))
	END_SET_VALUE_MAP()
	return CAction::SetParamValue(Name, Variant);
}
