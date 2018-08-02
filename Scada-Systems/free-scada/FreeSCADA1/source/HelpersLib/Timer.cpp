#include "StdAfx.h"
#include ".\timer.h"

namespace HelpersLib
{
	CTimer::CTimer(void)
	{
		//Сброс и инициализация таймера
		Reset();
	}

	CTimer::~CTimer(void)
	{
	}

	//сброс таймера
	void CTimer::Reset()
	{
		__int64 rate;
		QueryPerformanceFrequency((LARGE_INTEGER*)&rate);
		if (rate) rate_inv=1.0/(double)rate;
		QueryPerformanceCounter((LARGE_INTEGER*)&startclock);
	}
}
