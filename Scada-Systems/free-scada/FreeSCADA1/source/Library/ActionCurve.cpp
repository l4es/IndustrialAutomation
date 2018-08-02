#include "common.h"
#include ".\actioncurve.h"
#include "ObjGraph.h"

CActionCurve::CActionCurve(CObject* Owner_)
	: CAction(Owner_)
{
	//Инициализация объекта
	_Color= RGB(255,0,0);
	_Type = AT_CURVE;
	wstring name(L"Action_Curve_");
	if(Owner_)
	{
		name.append(Owner_->GetName());	
		name.append(L"_");
	}
	SetName(GenerateUniqueActionName(name));
	update=0;
}

CActionCurve::~CActionCurve(void)
{
}

void CActionCurve::Update()
{
	if((GetTickCount()-update)>=1000)
	{
		update=GetTickCount();
		double val = CalcControlValue();
		time_t t;
		time(&t);
		pair<time_t,float> point;
		point.first=t;point.second=val;
		_CurveData.push_back(point);
		int size=((CObjGraph*)_Owner)->GetGraphTime();
		if(_CurveData.front().first+size<=t)
			for(size_t i=0;i<_CurveData.size()/2;i++)
				_CurveData.pop_front();

	}
	
}


void CActionCurve::Serialize(CMemFile& file, BOOL IsLoading)
{
	CAction::Serialize(file, IsLoading);
	if(IsLoading)
	{
		//Загрузка
		file>>_Color;
	}
	else
	{
		//Сохранение
		file<<_Color;
	}
}

bool CActionCurve::GetParamValue(wstring Name, CComVariant& Variant)
{
	//Получить значение параметра экшена по его символьному названию
	BEGIN_GET_VALUE_MAP(Name, Variant)
		GET_VALUE_COLOR(L"Color", this->GetColor().AsCOLORREF())
	END_GET_VALUE_MAP()

	return CAction::GetParamValue(Name, Variant);
}


bool CActionCurve::SetParamValue(wstring Name, CComVariant Variant)
{
	//Установить значение параметра экшена по его символьному названию
	BEGIN_SET_VALUE_MAP(Name, Variant)
		SET_VALUE_COLOR(L"Color", this->SetColor(val))
	END_SET_VALUE_MAP()
	return CAction::SetParamValue(Name, Variant);
}
