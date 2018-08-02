#include "common.h"
#include ".\objsound.h"
#include "Action.h"

const COLORREF g_SoundIconColors[]={0,8421504,12632256,14933984,16777215, 0x848284};
const COLORREF g_SoundIconActiveColors[]={0,0x0000C8,0x0000FF,14933984,16777215,0x848284};

BYTE g_SoundIcon[23][22] =
{
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,0,0,3,3,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,2,2,0,3,3,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,2,2,2,0,3,3,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,2,2,4,4,0,3,3,
	3,3,3,3,3,3,3,3,3,3,3,3,0,0,2,2,4,4,4,0,3,3,
	3,3,3,3,3,3,3,0,0,0,0,0,2,2,2,4,4,4,4,0,3,3,
	3,3,3,3,3,0,0,2,2,2,2,2,2,2,4,4,4,4,4,0,3,3,
	3,3,3,3,0,2,2,2,2,2,2,2,4,4,4,4,4,2,2,0,3,3,
	3,0,0,0,2,4,4,4,4,4,4,4,4,4,4,2,2,2,2,0,3,3,
	0,0,0,0,4,4,4,4,4,4,4,4,4,4,2,2,2,2,2,0,3,3,
	0,3,0,4,4,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,3,3,
	0,3,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,3,3,
	0,3,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,3,3,
	0,0,0,0,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,0,3,3,
	3,0,0,0,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,0,0,3,
	3,3,3,3,0,1,1,1,1,1,1,1,1,1,2,2,2,2,2,0,0,0,
	3,3,3,3,3,0,0,1,1,1,1,1,1,1,1,1,2,2,2,0,0,0,
	3,3,3,3,3,3,3,0,0,0,0,0,1,1,1,1,2,2,2,0,0,0,
	3,3,3,3,3,3,3,3,3,3,3,3,0,0,1,1,1,1,1,0,0,3,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,1,1,1,1,0,3,3,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,1,1,1,0,3,3,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,1,1,0,3,3,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,0,0,3,3,
};

CObjSound::CObjSound(void)
	:_FileName(L"")
{
	//Генерируем уникальное имя для объекта
	SetName(GenerateUniqueObjectName(L"Sound_"));
	SetTypeID(_SOUND);
	SetActionFlags(AT_PLAY_SOUND);
	SetSize(23,22);
	SetVisible(false);
}

CObjSound::~CObjSound(void)
{
}

//Расование объекта
void CObjSound::Draw(HDC dc_)
{
	if(!IsVisible())
		return;

	CDCHandle dc(dc_);

	//Получить инстанцию менеджера объектов
	CObjectMenagerPtr mng;
	CPoint pt;
	//Перевод из глобальных координат в экранные
	pt = mng->GlobalToScreen(GetPosition());

	for(int i=0;i<23;i++)
	for(int j=0;j<22;j++)
		if(g_SoundIcon[i][j] != 3)
			if(_Sound.IsPlay())
				dc.SetPixel(pt.x+i,pt.y+j, g_SoundIconActiveColors[g_SoundIcon[i][j]]);
			else
				dc.SetPixel(pt.x+i,pt.y+j, g_SoundIconColors[g_SoundIcon[i][j]]);
}


void CObjSound::Serialize(CMemFile& file, BOOL IsLoading)
{
	CObject::Serialize(file, IsLoading);

	if(IsLoading)
	{
		file>>_FileName;
		file>>_Loop;

		//Загружаем изображение
		CArchiveManager* arch = file.GetArchive();
		wstring FileName = arch->GetSpecialFolder(CArchiveManager::SF_SOUNDS) + L"\\" + GetSoundName();
		if(arch->IsFileExists(FileName))
		{
			CMemFile* memfile;
			arch->OpenMemFile(FileName, &memfile);
			_Sound.Load(*memfile);
		}
	}
	else
	{
		file<<_FileName;
		file<<_Loop;
	}
}