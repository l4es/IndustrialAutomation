#pragma once
#include "object.h"
#include "Sound.h"

class CObjSound :
	public CObject
{
public:
	CObjSound(void);
	virtual ~CObjSound(void);

	//Прорисовка объекта
	virtual void Draw(HDC dc);

	void Serialize(CMemFile& file, BOOL IsLoading = TRUE);
	inline wstring GetSoundName(){return _FileName;};
	inline void SetSoundName(wstring FileName){_FileName = FileName;};

	inline bool GetLoop(){return _Loop;};
	inline void SetLoop(bool Loop){_Loop = Loop;};
	inline bool IsPlay(){return _Sound.IsPlay();};

	void Play(){_Sound.PlayAsync(_Loop);};
	void Stop(){_Sound.StopAsync();};

protected:
	wstring	_FileName;
	CSound	_Sound;
	bool	_Loop;
};
