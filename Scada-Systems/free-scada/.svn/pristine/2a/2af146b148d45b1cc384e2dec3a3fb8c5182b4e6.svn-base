#pragma once
#include "Messages.h"
#include <fstream>

namespace Helpers
{
	class CEventLogger
		:public CSingleton<CEventLogger>
	{
	public:
		void GenMsg(HRESULT msg);
		void DecodeIError(IErrorPtr err);
		void DecodeHRESULT(HRESULT err);
		void LogMessage(CString msg);

	protected:
		CEventLogger(void);
		~CEventLogger(void);
		DEF_SINGLETON(CEventLogger)

	protected:
		wofstream _out;
		CString GetTime();
	};

	typedef CSingletonPtr<CEventLogger> CEventLoggerPtr;
};
