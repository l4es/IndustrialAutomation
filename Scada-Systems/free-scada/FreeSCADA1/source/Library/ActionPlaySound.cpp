#include "common.h"
#include ".\actionplaysound.h"
#include "ObjSound.h"

CActionPlaySound::CActionPlaySound(CObject* Owner_)
	: CAction(Owner_)
{
	//Инициализация объекта
	_Type = AT_PLAY_SOUND;
	wstring name(L"Action_PlaySound_");
	if(Owner_)
	{
		name.append(Owner_->GetName());	
		name.append(L"_");
	}
	SetName(GenerateUniqueActionName(name));

}

CActionPlaySound::~CActionPlaySound(void)
{
}

void CActionPlaySound::Update()
{
	if(	_Owner == NULL)
		return;

	//Вычисляем значение
	double val = CalcControlValue();

	CObjSound* Owner = static_cast<CObjSound*>(_Owner);

	//Надо ли проиграть звук?
	if(val >= GetValueMax() || val <= GetValueMin()) 
	{
		if(!Owner->IsPlay())
			Owner->Play();
	}
	else
	{
		if(Owner->IsPlay())
			Owner->Stop();
	}

}