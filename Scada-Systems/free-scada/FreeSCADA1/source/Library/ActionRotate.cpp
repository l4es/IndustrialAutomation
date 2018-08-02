#include "common.h"
#include ".\actionrotate.h"

CActionRotate::CActionRotate(CObject* Owner_)
	: CAction(Owner_)
{
	//Инициализация объекта
	_Type = AT_ROTATE;
	_StartAngle = 0;
	_FinishAngle = 0;
	wstring name(L"Action_Rotate_");
	if(Owner_)
	{
		name.append(Owner_->GetName());	
		name.append(L"_");
	}
	SetName(GenerateUniqueActionName(name));

}

CActionRotate::~CActionRotate(void)
{
}

void CActionRotate::Update()
{
	if(	_Owner == NULL)
		return;

	//Вычисляем значение выражения
	double val = CalcControlValue();

	//Вычисляем текущую позицию в диапазоне min=0 max=1
	float CurrentPos = (float)((val - GetValueMin())/(GetValueMax()-GetValueMin()));
	
	//Расчитываем угол объекта
	float CurrentAngle = _BaseAngle + GetStartAngle() + 
		CurrentPos*(GetFinishAngle() - GetStartAngle());
	_Owner->Rotate(PI*CurrentAngle/180);
}


void CActionRotate::Serialize(CMemFile& file, BOOL IsLoading)
{
	CAction::Serialize(file, IsLoading);
	if(IsLoading)
	{
		//Загрузка
		file>>_StartAngle;
		file>>_FinishAngle;
	}
	else
	{
		//Сохранение
		file<<_StartAngle;
		file<<_FinishAngle;
	}
}

bool CActionRotate::GetParamValue(wstring Name, CComVariant& Variant)
{
	//Получить значение параметра экшена по его символьному названию
	BEGIN_GET_VALUE_MAP(Name, Variant)
		GET_VALUE_FLOAT(L"Min. angle", this->GetStartAngle())
		GET_VALUE_FLOAT(L"Max. angle", this->GetFinishAngle())
	END_GET_VALUE_MAP()

	return CAction::GetParamValue(Name, Variant);
}


bool CActionRotate::SetParamValue(wstring Name, CComVariant Variant)
{
	//Установить значение параметра экшена по его символьному названию
	BEGIN_SET_VALUE_MAP(Name, Variant)
		SET_VALUE_FLOAT(L"Min. angle", this->SetStartAngle(val))
		SET_VALUE_FLOAT(L"Max. angle", this->SetFinishAngle(val))
	END_SET_VALUE_MAP()
	return CAction::SetParamValue(Name, Variant);
}


void CActionRotate::SetStartAngle(float Start)
{
	_StartAngle = Start;
}


void CActionRotate::SetFinishAngle(float Finish)
{
	_FinishAngle = Finish;
}

float CActionRotate::GetStartAngle()
{
	return _StartAngle;
}


float CActionRotate::GetFinishAngle()
{
	return _FinishAngle;
}


void CActionRotate::SetOwner(CObject* Owner)
{
	CAction::SetOwner(Owner);

	if(_Owner)
		_BaseAngle	= _Owner->GetAngle();
}