#pragma once
#include <WTypes.h>
#include "Global.h"

namespace HelpersLib
{
	class HELPERS_LIB_API CTimer
	{
	public:
		//При создании инстанции класса таймер сбрасывается в 0
		CTimer(void);
		~CTimer(void);

		//Переинициализировать таймер
		void Reset();

		//Получить значение таймера
		// время в секундах с момента инициализации
		inline double GetTimer()
		{
			__int64 endclock;
			QueryPerformanceCounter((LARGE_INTEGER*)&endclock);
			return (endclock-startclock)*rate_inv;
		};

		//Получить знчение таймера как float (вместо double)
		inline operator float(){return static_cast<float>(GetTimer());}

	private:
		__int64 startclock;
		double rate_inv;
	};
}
