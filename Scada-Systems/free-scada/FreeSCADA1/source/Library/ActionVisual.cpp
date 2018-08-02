#include "common.h"
#include ".\actionvisual.h"

CActionVisual::CActionVisual(CObject* Owner_)
	: CAction(Owner_)
{
	//Инициализация объекта
	_Type = AT_VISUAL;
	wstring name(L"Action_Visual_");
	if(Owner_)
	{
		name.append(Owner_->GetName());	
		name.append(L"_");
	}
	SetName(GenerateUniqueActionName(name));

}

CActionVisual::~CActionVisual(void)
{
}


void CActionVisual::Update()
{
	if(	_Owner == NULL)
		return;

	//Вычисляем значение
	double val = CalcControlValue();

	//Показываем или прячем объект
	if(val >= GetValueMax()) _Owner->Show();
	if(val <= GetValueMin()) _Owner->Hide();
		
}