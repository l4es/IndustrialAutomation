#include "stdafx.h"
#include "CrashHandler.h"
#include "zlibcpp.h"
#include "ReportInfo.h"
#include "process.h"
#include "Utility.h"

// global app module
CAppModule _Module;

namespace crash_reporter
{
	// maps crash objects to processes
	CSimpleMap<int, CCrashHandler*> _crashStateMap;

	// unhandled exception callback set with SetUnhandledExceptionFilter()
	LONG WINAPI CustomUnhandledExceptionFilter(PEXCEPTION_POINTERS pExInfo)
	{
		_crashStateMap.Lookup(_getpid())->GenerateErrorReport(pExInfo);
		return EXCEPTION_EXECUTE_HANDLER;
	}

	CCrashHandler::CCrashHandler(LPGETLOGFILE lpfn /*=NULL*/)
	{
		// wtl initialization stuff...
		HRESULT hRes = ::CoInitialize(NULL);
		ATLASSERT(SUCCEEDED(hRes));

		hRes = _Module.Init(NULL, GetModuleHandle(_T("CrashReporter.dll")));
		ATLASSERT(SUCCEEDED(hRes));

		::DefWindowProc(NULL, 0, 0, 0L);

		// initialize member data
		m_lpfnCallback = NULL;
		m_oldFilter    = NULL;

		// save user supplied callback
		if (lpfn)
			m_lpfnCallback = lpfn;

		// add this filter in the exception callback chain
		m_oldFilter = SetUnhandledExceptionFilter(CustomUnhandledExceptionFilter);

		// attach this handler with this process
		m_pid = _getpid();
		_crashStateMap.Add(m_pid, this);
	}

	CCrashHandler::~CCrashHandler()
	{
		// reset exception callback
		if (m_oldFilter)
			SetUnhandledExceptionFilter(m_oldFilter);

		_crashStateMap.Remove(m_pid);

		// uninitialize
		_Module.Term();
		::CoUninitialize();

	}

	void CCrashHandler::AddFile(LPCTSTR lpFile, LPCTSTR lpDesc)
	{
		// make sure the file exist
		HANDLE hFile = ::CreateFile(
			lpFile,
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0);
		if (hFile)
		{
			// add file to report
			m_files[lpFile] = lpDesc;
			::CloseHandle(hFile);
		}
	}

	void CCrashHandler::GenerateErrorReport(PEXCEPTION_POINTERS pExInfo)
	{
		CReportInfo  rpt(pExInfo);
		CZLib             zlib;
		std::wstring      sTempFileName = CUtility::getTempFileName();

		// let client add application specific files to report
		if (m_lpfnCallback && !m_lpfnCallback(this))
			return;

		// add crash files to report
		std::wstring crash_filename = rpt.getCrashFile();
		std::wstring crash_log_filename = rpt.getCrashLog();

		if(!crash_filename.empty())
			m_files[crash_filename] = L"Crash dump";
		if(!crash_log_filename.empty())
			m_files[crash_log_filename] = L"Crash information";

		// add symbol files to report
		for (size_t i = 0; i < rpt.get_additional_files_count(); i++)
			m_files[rpt.get_additional_file(i)] = L"Additional file";

		// zip the report
		if (!zlib.Open(sTempFileName))
			return;

		// add report files to zip
		string_map_t::iterator cur = m_files.begin();
		for (string_map_t::iterator i=m_files.begin();i!=m_files.end();i++)
			zlib.AddFile(i->first);

		zlib.Close();

		// display message
		const wchar_t msg[] = L"Unfortunately the program is crashed. Most likely this happened\nbecause of an error in the program’s logic. Would you like to\nsend the crash report with your own comments to FreeSCADA developers?\n\nHit `Yes’ to save the report on your disk or\nhit ‘No’ to simply exit and forget about this error...";
		if(MessageBox(NULL, msg, L"Critical Error", MB_YESNO|MB_ICONERROR) == IDYES)
			SaveReport(rpt, sTempFileName.c_str());

		DeleteFile(sTempFileName.c_str());
	}

	BOOL CCrashHandler::SaveReport(CReportInfo&, LPCTSTR lpcszFile)
	{
		// let user more zipped report
		return (CopyFile(lpcszFile, CUtility::getSaveFileName().c_str(), TRUE));
	}
}