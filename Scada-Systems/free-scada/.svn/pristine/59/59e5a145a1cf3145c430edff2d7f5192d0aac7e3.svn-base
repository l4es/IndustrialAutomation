#include "stdafx.h"
#include "CrashReporter.h"
#include "CrashHandler.h"

namespace crash_reporter
{

#ifdef _DEBUG
#define CRASH_ASSERT(pObj)          \
	if (!pObj || sizeof(*pObj) != sizeof(CCrashHandler))  \
	DebugBreak()                                       
#else
#define CRASH_ASSERT(pObj)
#endif // _DEBUG

	CRASHREPORTER_API LPVOID Install(LPGETLOGFILE pfn)
	{
		CCrashHandler *pImpl = new CCrashHandler(pfn);
		CRASH_ASSERT(pImpl);

		return pImpl;
	}

	CRASHREPORTER_API void Uninstall(LPVOID lpState)
	{
		CCrashHandler *pImpl = (CCrashHandler*)lpState;
		CRASH_ASSERT(pImpl);

		delete pImpl;
	}

	CRASHREPORTER_API void AddFile(LPVOID lpState, LPCTSTR lpFile, LPCTSTR lpDesc)
	{
		CCrashHandler *pImpl = (CCrashHandler*)lpState;
		CRASH_ASSERT(pImpl);

		pImpl->AddFile(lpFile, lpDesc);
	}

	CRASHREPORTER_API void GenerateErrorReport(LPVOID lpState, PEXCEPTION_POINTERS pExInfo)
	{
		CCrashHandler *pImpl = (CCrashHandler*)lpState;
		CRASH_ASSERT(pImpl);

		pImpl->GenerateErrorReport(pExInfo);
	}
}
