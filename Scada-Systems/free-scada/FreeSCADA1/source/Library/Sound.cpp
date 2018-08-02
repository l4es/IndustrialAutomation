#include "common.h"
#include ".\sound.h"
#include <Mmsystem.h>

CSound::CSound(void)
{
	_Data = NULL;
	_Size = NULL;
	_IsPlay = false;
}

CSound::CSound(const CSound& src)
{
	_Data = new BYTE[src._Size];
	memcpy(_Data, src._Data, src._Size);
	_Size = src._Size;
}

CSound::~CSound(void)
{
	Clear();
}

void CSound::Clear()
{
	if(_Data)
	{
		delete _Data;
		_Data = NULL;
	}
	_Size = NULL;
	_IsPlay = false;
}

//Загрузить
bool CSound::Load(CMemFile& file)
{
	_Size = static_cast<ULONG>(file.GetSize());
	_Data = new BYTE[_Size];
	if(_Data == NULL)
		return false;
	file.ReadBlock(_Data, _Size);
	return true;
}

//Сохранить
bool CSound::Save(CMemFile& file)
{
	file.WriteBlock(_Data, _Size);
	return true;
}

void CSound::Play(bool Loop)
{
	if(_Data == NULL)
		return;

	DWORD flags = SND_SYNC|SND_MEMORY;
	if(Loop)
		flags |= SND_LOOP;

	PlaySoundA((LPCSTR)_Data, NULL, flags);
	_IsPlay = false;
}

void CSound::PlayAsync(bool Loop)
{
	if(_Data == NULL)
		return;

	DWORD flags = SND_ASYNC|SND_MEMORY;
	if(Loop)
		flags |= SND_LOOP;

	_IsPlay = PlaySoundA((LPCSTR)_Data, NULL, flags)?true:false;
}

void CSound::StopAsync()
{
	PlaySoundA(NULL, NULL, SND_ASYNC|SND_MEMORY);
	_IsPlay = false;
}

bool CSound::LoadFromStdFile(wstring FileName)
{
	CAtlFile file;
	if(FAILED(file.Create(FileName.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING)))
		return false;
	
	ULONGLONG Size;
	file.GetSize(Size);
	
	Clear();
	_Size = static_cast<ULONG>(Size);
	_Data = new BYTE[_Size];
	if(_Data)
		file.Read(_Data, _Size);
	else
		return false;
	file.Close();
	return true;
}