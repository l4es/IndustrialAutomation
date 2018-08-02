#include "common.h"
#include ".\actioncolor.h"
#include "ObjImage.h"

CActionColor::CActionColor(CObject* Owner_)
	: CAction(Owner_)
{
	//Инициализация объекта
	_StartColor = RGB(0,0,0);
	_EndColor = RGB(255,255,255);
	_Type = AT_COLOR;
	wstring name(L"Action_Color_");
	if(Owner_)
	{
		name.append(Owner_->GetName());	
		name.append(L"_");
	}
	SetName(GenerateUniqueActionName(name));

}

CActionColor::~CActionColor(void)
{
}

void CActionColor::Update()
{
	if(	_Owner == NULL)
		return;

	//Вычисляем значение выражения
	double val = CalcControlValue();

	//Текущее значение изменяется от 0 до 1
	double CurrentPos = (double)((val - GetValueMin())/(GetValueMax()-GetValueMin()));

	sRGB NewColor;

	//Вычисляем новое значение цвета по каждой составляющей
/*	if(_StartColor.r < _EndColor.r)
		NewColor.r = _StartColor.r + (_EndColor.r-_StartColor.r)*CurrentPos;
	else
		NewColor.r = _EndColor.r + (_StartColor.r-_EndColor.r)*CurrentPos;

	if(_StartColor.g < _EndColor.g)
		NewColor.g = _StartColor.g + (_EndColor.g-_StartColor.g)*CurrentPos;
	else
		NewColor.g = _EndColor.g + (_StartColor.g-_EndColor.g)*CurrentPos;

	if(_StartColor.b < _EndColor.b)
		NewColor.b = _StartColor.b + (_EndColor.b-_StartColor.b)*CurrentPos;
	else
		NewColor.b = _EndColor.b + (_StartColor.b-_EndColor.b)*CurrentPos;
*/	
	NewColor.r = abs(_StartColor.r - abs(_EndColor.r-_StartColor.r)*CurrentPos);
	NewColor.g = abs(_StartColor.g - abs(_EndColor.g-_StartColor.g)*CurrentPos);
	NewColor.b = abs(_StartColor.b - abs(_EndColor.b-_StartColor.b)*CurrentPos);
	if(_Owner->GetColor() != NewColor)
	{
		//Если текущий цвет отличается от расчитанного, изменяем цвет объекта
		_Owner->SetColor(NewColor);

		//Для объекта типа "Image" необходимо принудительно обновить изображение
		if(_Owner->GetTypeID() == _IMAGE)
			((CObjImage*)_Owner)->RotateImage(_Owner->GetAngle());
	}
}

void CActionColor::Serialize(CMemFile& file, BOOL IsLoading)
{
	CAction::Serialize(file, IsLoading);
	if(IsLoading)
	{
		//Загрузка
		file>>_StartColor;
		file>>_EndColor;
	}
	else
	{
		//Сохранение
		file<<_StartColor;
		file<<_EndColor;
	}
}

bool CActionColor::GetParamValue(wstring Name, CComVariant& Variant)
{
	//Получить значение параметра экшена по его символьному названию
	BEGIN_GET_VALUE_MAP(Name, Variant)
		GET_VALUE_COLOR(L"Start color", this->GetStartColor().AsCOLORREF())
		GET_VALUE_COLOR(L"End color", this->GetEndColor().AsCOLORREF())
	END_GET_VALUE_MAP()

	return CAction::GetParamValue(Name, Variant);
}


bool CActionColor::SetParamValue(wstring Name, CComVariant Variant)
{
	//Установить значение параметра экшена по его символьному названию
	BEGIN_SET_VALUE_MAP(Name, Variant)
		SET_VALUE_COLOR(L"Start color", this->SetStartColor(val))
		SET_VALUE_COLOR(L"End color", this->SetEndColor(val))
	END_SET_VALUE_MAP()
	return CAction::SetParamValue(Name, Variant);
}
