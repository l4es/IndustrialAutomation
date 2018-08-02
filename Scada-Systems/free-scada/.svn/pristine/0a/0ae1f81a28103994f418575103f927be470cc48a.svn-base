#include "common.h"
#include ".\actionsizer.h"

CActionSizer::CActionSizer(CObject* Owner_)
: CAction(Owner_)
, _CurrentPos(0)
{
	_Type		= AT_SIZER;
	_SizerType	= ST_NONE;
	wstring name(L"Action_Sizer_");
	if(Owner_)
	{
		name.append(Owner_->GetName());	
		name.append(L"_");
	}
	SetName(GenerateUniqueActionName(name));

}


CActionSizer::~CActionSizer(void)
{
}

//Обновление размеров и позиции объекта-владельца
void CActionSizer::Update()
{
	if(	_Owner == NULL)
		return;
	
	//Расчет значения выражения
	double val = CalcControlValue();
	//Определения текущего положения объекта в относительнных координатах
	_CurrentPos = (float)((val - GetValueMin())/(GetValueMax()-GetValueMin()));
	
	float Angle = _Owner->GetAngle();
	
	sVector Pos = _BasePos;
	sVector NewSize = _BaseSize;
	sVector Delta;
	float DeltaSize;

	//Пересчет размеров и позиции объекта в соответствии с типом ActionSizer
	switch(_SizerType) 
	{
	case ST_RIGHT:
		DeltaSize = (_CurrentPos - 1)*_BaseSize.x;
		Delta = 	sVector(DeltaSize*cos(Angle), DeltaSize*sin(Angle));
		Pos.x += Delta.x/2;
		if(cos(Angle) != 0)
			Pos.y += Delta.x/2*sin(Angle)/cos(Angle);
		_Owner->MoveTo(Pos);
		NewSize.x += DeltaSize;
		break;

	case ST_LEFT:
		DeltaSize = (_CurrentPos - 1)*_BaseSize.x;
		Delta = sVector(-DeltaSize*cos(Angle), DeltaSize*sin(Angle));
		Pos.x += Delta.x/2;
		if(cos(Angle) != 0)
			Pos.y += Delta.x/2*sin(Angle)/cos(Angle);
		_Owner->MoveTo(Pos);
		NewSize.x += DeltaSize;
		break;

	case ST_TOP:
		DeltaSize = (_CurrentPos-1)*_BaseSize.y;
		Delta = sVector(DeltaSize*sin(Angle), DeltaSize*cos(Angle));
		Pos.y -= Delta.y/2;
		if(cos(Angle) != 0)
			Pos.x += Delta.y/2*sin(Angle)/cos(Angle);
		_Owner->MoveTo(Pos);
		NewSize.y += DeltaSize;
		break;

	case ST_BOTTOM:
		DeltaSize = (_CurrentPos-1)*_BaseSize.y;
		Delta = 	sVector(DeltaSize*sin(Angle), DeltaSize*cos(Angle));
		Pos.y += Delta.y/2;
		if(cos(Angle) != 0)
			Pos.x -= Delta.y/2*sin(Angle)/cos(Angle);
		_Owner->MoveTo(Pos);
		NewSize.y += DeltaSize;
		break;
	}

	_Owner->SetSize(NewSize);
	_Owner->MoveBy((_BaseSize - NewSize)/2);
}

//сохранение-загрузка ActionSizer
void CActionSizer::Serialize(CMemFile& file, BOOL IsLoading)
{
	if(IsLoading)
	{
		file>>_CurrentPos;
		DWORD tmp;
		file>>tmp;
		_SizerType = (enSizerType)tmp;
	}
	else
	{
		file<<_CurrentPos;
		file<<(DWORD)_SizerType;
	}

	CAction::Serialize(file, IsLoading);
}

//Установка типа действия изменения размеров
void CActionSizer::SetSizerType(enSizerType Type)
{
	_SizerType = Type;
}

//Установка объекта-владельца
void CActionSizer::SetOwner(CObject* Owner)
{
	CAction::SetOwner(Owner);

	if(_Owner)
	{
		_BaseSize	= _Owner->GetSize();
		_BasePos	= _Owner->GetPosition();
	}
}

//Получение строковое название типа ActionSizer
wstring CActionSizer::GetSizerTypeAsText()
{
	wstring Sizer;
	switch(_SizerType) {
	case ST_TOP:
		Sizer = L"Up";
		break;
	case ST_BOTTOM:
		Sizer = L"Down";
		break;
	case ST_LEFT:
		Sizer = L"Left";
		break;
	case ST_RIGHT:
		Sizer = L"Right";
		break;
	case ST_ALL:
		Sizer = L"All sides";
		break;
	}
	return Sizer;
}

//Установить тип ActionSizer через строковую переменную
void CActionSizer::SetSizerTypeAsText(wstring Sizer)
{
	if(Sizer == L"Up") _SizerType = ST_TOP;
	if(Sizer == L"Down") _SizerType = ST_BOTTOM;
	if(Sizer == L"Left") _SizerType = ST_LEFT;
	if(Sizer == L"Right") _SizerType = ST_RIGHT;
	if(Sizer == L"All sides") _SizerType = ST_ALL;
}

//Получение значения объекта 
bool CActionSizer::GetParamValue(wstring Name, CComVariant& Variant)
{
	BEGIN_GET_VALUE_MAP(Name, Variant)
		GET_VALUE_STRING(L"Direction", this->GetSizerTypeAsText().c_str())
	END_GET_VALUE_MAP()

	return CAction::GetParamValue(Name, Variant);
}

//Установка значения объекта
bool CActionSizer::SetParamValue(wstring Name, CComVariant Variant)
{
	BEGIN_SET_VALUE_MAP(Name, Variant)
		SET_VALUE_STRING(L"Direction", this->SetSizerTypeAsText(val))
	END_SET_VALUE_MAP()
	return CAction::SetParamValue(Name, Variant);
}