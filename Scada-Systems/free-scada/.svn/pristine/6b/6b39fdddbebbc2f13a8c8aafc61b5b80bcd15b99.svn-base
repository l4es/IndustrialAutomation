#pragma once
#include "MemFile.h"

class CSound
{
public:
	CSound(void);
	CSound(const CSound& src);
	virtual ~CSound(void);

	//Загрузить
	bool Load(CMemFile& file);
	//Сохранить
	bool Save(CMemFile& file);

	void Play(bool Loop);
	void PlayAsync(bool Loop);
	void StopAsync();

	void Clear();
	bool LoadFromStdFile(wstring FileName);
	inline bool IsPlay(){return _IsPlay;};

protected:
	BYTE*	_Data;		//Данные
	ULONG	_Size;		//Размер
	bool	_IsPlay;
};
