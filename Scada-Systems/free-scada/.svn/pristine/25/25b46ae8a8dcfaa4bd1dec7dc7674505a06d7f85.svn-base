#pragma once
#include "Connections.h"

using namespace Helpers;

class CEngine
	: public CSingleton<CEngine>
{
public:
	void Start(void);		//«апустить архивацию
	void Stop(void);		//ќстановить архивацию
	void Pause(void);		//приостановить архивацию (данные принимаютс€, но в базу не помещаютс€)
	void Continue(void);	//продолжить архивацию

protected:
	CEngine(void);
	~CEngine(void);
	DEF_SINGLETON(CEngine);

	//‘ункци€ таймера, который периодически провер€ет состо€ни€ след€щих таймеров каналов
	static void CALLBACK TimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);

protected:
	CConnections _Connections;
	UINT_PTR _TimerID;
};
