#include "common.h"
#include ".\action.h"
#include "VariableMenager.h"

CAction::CAction(CObject* Owner_)
	: _Type(AT_NONE)
	, _MinVal(0)
	, _MaxVal(0)
	, _Expression(L"0")
{
	//Инициализация объекта
	SetOwner(Owner_);
	_Name.clear();
}

CAction::~CAction(void)
{
}

double CAction::CalcControlValue()
{
	if(!_Variable.empty())
	{
		CComVariant v;
		CVariableMenagerPtr vm;
		v = vm->GetVariable(_Variable);

		if(v.vt == VT_BOOL)
			v = v.boolVal?1:0;

		if(SUCCEEDED(v.ChangeType(VT_R8)))
		{
			v.dblVal = max(v.dblVal, (double)GetValueMin());
			v.dblVal = min(v.dblVal, (double)GetValueMax());
			return v.dblVal;
		}
	}

	//Вычисляем значение выражения
	double val = _Expression.empty() ? NULL : _Equation.CalcDouble(_Expression);
	val = max(val, (double)GetValueMin());
	val = min(val, (double)GetValueMax());
	return val;
}

void CAction::Serialize(CMemFile& file, BOOL IsLoading)
{
	if(IsLoading)
	{
		//Загрузка
		wstring Name;
		file>>Name;
		
		CObjectMenagerPtr mng;
		CObject* obj = mng->FindObject(Name);

		if(obj == NULL) return;
		SetOwner(obj);
		
		long tmp;
		file>>tmp;
		_Type = static_cast<enActionType>(tmp);
		
		file>>_Name;
		file>>_MinVal;
		file>>_MaxVal;
		file>>_Expression;
		file>>_Variable;
	}
	else
	{
		//Запись
		file<<_Owner->GetName();

		file<<(long)_Type;
		
		file<<_Name;
		file<<_MinVal;
		file<<_MaxVal;
		file<<_Expression;
		file<<_Variable;
	}
}

void CAction::SetOwner(CObject* Owner)
{
	_Owner = Owner;

}

bool CAction::GetParamValue(wstring Name, CComVariant& Variant)
{
	//Получить значение параметра экшена по его символьному названию
	BEGIN_GET_VALUE_MAP(Name, Variant)//Alex:
		GET_VALUE_FLOAT(L"Min. value", this->GetValueMin())
		GET_VALUE_FLOAT(L"Max. value", this->GetValueMax())
		GET_VALUE_STRING(L"Expression", this->GetExpression().c_str())
		GET_VALUE_STRING(L"Variable", this->GetVariable().c_str())
		GET_VALUE_STRING(L"Name", this->GetName().c_str())
	END_GET_VALUE_MAP()	
	return false;
}


bool CAction::SetParamValue(wstring Name, CComVariant Variant)
{
	//Установить значение параметра экшена по его символьному названию
	BEGIN_SET_VALUE_MAP(Name, Variant) //Alex:
		SET_VALUE_FLOAT(L"Min. value", this->SetValueLimit(val, this->GetValueMax()))
		SET_VALUE_FLOAT(L"Max. value", this->SetValueLimit(this->GetValueMin(), val))
		SET_VALUE_STRING(L"Expression", this->SetExpression(val))
		SET_VALUE_STRING(L"Variable", this->SetVariable(val))
		SET_VALUE_STRING(L"Name", this->SetName(val))
	END_SET_VALUE_MAP()
	return false;
}


