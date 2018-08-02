#include "stdafx.h"
#include "ReportInfo.h"
#include "utility.h"

namespace crash_reporter
{
	std::wstring CReportInfo::m_sModule = _T("");
	std::wstring CReportInfo::m_sException = _T("");
	std::wstring CReportInfo::m_sAddress = _T("");

	BOOL CALLBACK MiniDumpCallbackFunc(PVOID CallbackParam, const PMINIDUMP_CALLBACK_INPUT CallbackInput, PMINIDUMP_CALLBACK_OUTPUT CallbackOutput)
	{
		if(CallbackInput && CallbackInput->CallbackType == ModuleCallback)
		{
			modules_info_t *modules = reinterpret_cast<modules_info_t*>(CallbackParam);
			module_info_t info;
			info.full_path = CallbackInput->Module.FullPath;
			info.image_size	= CallbackInput->Module.SizeOfImage;
			info.version = CallbackInput->Module.VersionInfo;
			modules->push_back(info);
		}

		switch(CallbackInput->CallbackType)
		{
		case ModuleCallback:

			CallbackOutput->ModuleWriteFlags =	0xFFFF;
			break;
		case ThreadCallback:
		case ThreadExCallback:
			CallbackOutput->ThreadWriteFlags =	0xFFFF;
			break;
		case IncludeThreadCallback:
		case IncludeModuleCallback:
		case MemoryCallback:
			break;
		default:
			return FALSE;
		}

		return TRUE;
	}



	CReportInfo::CReportInfo(PEXCEPTION_POINTERS ExceptionInfo)
	{
		m_excpInfo = ExceptionInfo;
	}

	//-----------------------------------------------------------------------------
	// CReportInfo::getCrashFile
	//
	// Creates the dump file returning the file name
	//
	std::wstring CReportInfo::getCrashFile()
	{
		std::wstring sFile;

		// Create the dump file name
		wchar_t temp_path[MAX_PATH+1];
		const DWORD path_len = GetTempPathW(MAX_PATH, temp_path);
		for(DWORD i=path_len;i!=0;i++)
		{
			if(temp_path[i] == L'\\')
				temp_path[i] = 0;
			else
				break;
		}

		wostringstream fmt;
		fmt<<temp_path<<L"\\"<<CUtility::getAppName()<<L".dmp";
		sFile = fmt.str();

		typedef BOOL (WINAPI *MINIDUMP_WRITE_DUMP)(
			HANDLE hProcess,
			DWORD ProcessId,
			HANDLE hFile,
			MINIDUMP_TYPE DumpType,
			PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
			PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
			PMINIDUMP_CALLBACK_INFORMATION CallbackParam
			);

		HMODULE hDbgHelp = LoadLibraryA("dbghelp.dll");
		MINIDUMP_WRITE_DUMP MiniDumpWriteDump_ = (MINIDUMP_WRITE_DUMP)GetProcAddress(hDbgHelp, 
			"MiniDumpWriteDump");

		if (MiniDumpWriteDump_ == NULL)
		{
			FreeLibrary(hDbgHelp);
			return L"";
		}

		// Create the file
		HANDLE hFile = CreateFile(sFile.c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		BOOL res = FALSE;
		if (hFile != INVALID_HANDLE_VALUE)
		{
			MINIDUMP_EXCEPTION_INFORMATION eInfo;
			eInfo.ThreadId = GetCurrentThreadId();
			eInfo.ExceptionPointers = m_excpInfo;
			eInfo.ClientPointers = FALSE;
			
			MINIDUMP_CALLBACK_INFORMATION callback_info;
			callback_info.CallbackParam = (PVOID)&m_modules_info;
			callback_info.CallbackRoutine = MiniDumpCallbackFunc;
			res = MiniDumpWriteDump_(
				GetCurrentProcess(),
				GetCurrentProcessId(),
				hFile,
				MiniDumpNormal,
				m_excpInfo ? &eInfo : NULL,
				NULL,
				&callback_info);
		}

		CloseHandle(hFile);
		FreeLibrary(hDbgHelp);

		if(res)
			return sFile;
		else
			return std::wstring();
	}


	//-----------------------------------------------------------------------------
	// CReportInfo::getCrashLog
	//
	// Creates the XML log file returning the name
	//
	std::wstring CReportInfo::getCrashLog()
	{
		std::wstring filename;
		// Create the dump file name
		wchar_t temp_path[MAX_PATH+1];
		const DWORD path_len = GetTempPathW(MAX_PATH, temp_path);
		for(DWORD i=path_len;i!=0;i++)
		{
			if(temp_path[i] == L'\\')
				temp_path[i] = 0;
			else
				break;
		}

		wostringstream fmt;
		fmt<<temp_path<<L"\\"<<CUtility::getAppName()<<L".info";
		filename = fmt.str();

		wofstream file(filename.c_str());
		if(!file.is_open())
			return std::wstring();

		// Add exception record
		if (m_excpInfo)
			write_exception_info(file, m_excpInfo->ExceptionRecord);

		write_processor_info(file);
		write_os_info(file);
		write_modules_info(file);

		return filename;
	}

	void CReportInfo::write_exception_info(std::wofstream& file, EXCEPTION_RECORD* pExceptionRecord)
	{
		file<<std::endl<<L"Exception Info:"<<std::endl;

		// Set module name attribute
		TCHAR szModName[_MAX_FNAME + 1];
		GetModuleFileName(NULL, szModName, _MAX_FNAME);
		file<<L"ModuleName:\t\t"<<szModName<<std::endl;

		// Set exception code
		file<<L"ExceptionCode:\t\t"<<std::hex<<"0x"<<pExceptionRecord->ExceptionCode<<std::dec<<std::endl;

		// Set exception description
		switch (pExceptionRecord->ExceptionCode)
		{
		case EXCEPTION_ACCESS_VIOLATION:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_ACCESS_VIOLATION"<<std::endl;
			break;
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_DATATYPE_MISALIGNMENT"<<std::endl;
			break;
		case EXCEPTION_BREAKPOINT:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_BREAKPOINT"<<std::endl;
			break;
		case EXCEPTION_SINGLE_STEP:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_SINGLE_STEP"<<std::endl;
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_ARRAY_BOUNDS_EXCEEDED"<<std::endl;
			break;
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_FLT_DENORMAL_OPERAND"<<std::endl;
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_FLT_DIVIDE_BY_ZERO"<<std::endl;
			break;
		case EXCEPTION_FLT_INEXACT_RESULT:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_FLT_INEXACT_RESULT"<<std::endl;
			break;
		case EXCEPTION_FLT_INVALID_OPERATION:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_FLT_INVALID_OPERATION"<<std::endl;
			break;
		case EXCEPTION_FLT_OVERFLOW:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_FLT_OVERFLOW"<<std::endl;
			break;
		case EXCEPTION_FLT_STACK_CHECK:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_FLT_STACK_CHECK"<<std::endl;
			break;
		case EXCEPTION_FLT_UNDERFLOW:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_FLT_UNDERFLOW"<<std::endl;
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_INT_DIVIDE_BY_ZERO"<<std::endl;
			break;
		case EXCEPTION_INT_OVERFLOW:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_INT_OVERFLOW"<<std::endl;
			break;
		case EXCEPTION_PRIV_INSTRUCTION:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_PRIV_INSTRUCTION"<<std::endl;
			break;
		case EXCEPTION_IN_PAGE_ERROR:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_IN_PAGE_ERROR"<<std::endl;
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_ILLEGAL_INSTRUCTION"<<std::endl;
			break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_NONCONTINUABLE_EXCEPTION"<<std::endl;
			break;
		case EXCEPTION_STACK_OVERFLOW:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_STACK_OVERFLOW"<<std::endl;
			break;
		case EXCEPTION_INVALID_DISPOSITION:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_INVALID_DISPOSITION"<<std::endl;
			break;
		case EXCEPTION_GUARD_PAGE:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_GUARD_PAGE"<<std::endl;
			break;
		case EXCEPTION_INVALID_HANDLE:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_INVALID_HANDLE"<<std::endl;
			break;
		default:
			file<<L"ExceptionDescription:\t"<<L"EXCEPTION_UNKNOWN"<<std::endl;
		}

		// Set exception address
		file<<L"ExceptionAddress:\t"<<std::hex<<"0x"<<pExceptionRecord->ExceptionAddress<<std::dec<<std::endl;

		file<<std::endl;
	}

	void CReportInfo::write_processor_info(std::wofstream& file)
	{
		file<<std::endl<<L"Processor Info:"<<std::endl;

		SYSTEM_INFO si;
		GetSystemInfo(&si);
		// Set architecture
		switch (si.wProcessorArchitecture)
		{
		case PROCESSOR_ARCHITECTURE_INTEL:
			file<<L"Architecture:\t\t"<<L"PROCESSOR_ARCHITECTURE_INTEL"<<std::endl;
			break;
		case PROCESSOR_ARCHITECTURE_MIPS:
			file<<L"Architecture:\t\t"<<L"PROCESSOR_ARCHITECTURE_MIPS"<<std::endl;
			break;
		case PROCESSOR_ARCHITECTURE_ALPHA:
			file<<L"Architecture:\t\t"<<L"PROCESSOR_ARCHITECTURE_ALPHA"<<std::endl;
			break;
		case PROCESSOR_ARCHITECTURE_PPC:
			file<<L"Architecture:\t\t"<<L"PROCESSOR_ARCHITECTURE_PPC"<<std::endl;
			break;
		case PROCESSOR_ARCHITECTURE_SHX:
			file<<L"Architecture:\t\t"<<L"PROCESSOR_ARCHITECTURE_SHX"<<std::endl;
			break;
		case PROCESSOR_ARCHITECTURE_ARM:
			file<<L"Architecture:\t\t"<<L"PROCESSOR_ARCHITECTURE_ARM"<<std::endl;
			break;
		case PROCESSOR_ARCHITECTURE_IA64:
			file<<L"Architecture:\t\t"<<L"PROCESSOR_ARCHITECTURE_IA64"<<std::endl;
			break;
		case PROCESSOR_ARCHITECTURE_ALPHA64:
			file<<L"Architecture:\t\t"<<L"PROCESSOR_ARCHITECTURE_ALPHA64"<<std::endl;
			break;
		case PROCESSOR_ARCHITECTURE_UNKNOWN:
			file<<L"Architecture:\t\t"<<L"PROCESSOR_ARCHITECTURE_UNKNOWN"<<std::endl;
			break;
		default:
			file<<L"Architecture:\t\t"<<L"Unknown ("<<si.wProcessorArchitecture<<L")"<<std::endl;
		}

		// Set level
		if (PROCESSOR_ARCHITECTURE_INTEL == si.wProcessorArchitecture)
		{
			switch (si.wProcessorLevel)
			{
			case 3:	file<<L"Level:\t\t"<<L"Intel 30386"<<std::endl;break;
			case 4: file<<L"Level:\t\t"<<L"Intel 80486"<<std::endl;break;
			case 5: file<<L"Level:\t\t"<<L"Intel Pentium"<<std::endl;break;
			case 6: file<<L"Level:\t\t"<<L"Intel Pentium Pro or Pentium II"<<std::endl;break;
			default: file<<L"Level:\t\t"<<si.wProcessorLevel<<std::endl;break;
			}
		}

		// Set number of processors
		file<<L"NumberOfProcessors:\t"<<si.dwNumberOfProcessors<<std::endl;

		file<<std::endl;
	}

	void CReportInfo::write_os_info(std::wofstream& file)
	{
		file<<std::endl<<L"OS Info:"<<std::endl;

		BSTR nodeName                    = ::SysAllocString(L"OperatingSystem");
		BSTR majorName                   = ::SysAllocString(L"MajorVersion");
		BSTR minorName                   = ::SysAllocString(L"MinorVersion");
		BSTR buildName                   = ::SysAllocString(L"BuildNumber");
		BSTR csdName                     = ::SysAllocString(L"CSDVersion");

		OSVERSIONINFO oi;
		oi.dwOSVersionInfoSize = sizeof(oi);
		GetVersionEx(&oi);

		file<<L"MajorVersion:\t"<<oi.dwMajorVersion<<std::endl;
		file<<L"MinorVersion:\t"<<oi.dwMinorVersion<<std::endl;
		file<<L"BuildNumber:\t"<<oi.dwBuildNumber<<std::endl;
		file<<L"CSDVersion:\t"<<oi.szCSDVersion<<std::endl;

		file<<std::endl;
	}

	void CReportInfo::write_modules_info(std::wofstream& file)
	{
		file<<std::endl<<L"Modules Info:"<<std::endl;

		// Add module information
		for(modules_info_t::iterator i=m_modules_info.begin();i!=m_modules_info.end();i++)
		{
			file<<L"Module: \""<<i->full_path<<"\""<<std::endl;
			file<<L"\tSize:\t\t"<<i->image_size<<std::endl;

			// Set timestamp
			FILETIME    ft = CUtility::getLastWriteFileTime(i->full_path);
			SYSTEMTIME  st = {0};

			FileTimeToSystemTime(&ft, &st);

			file<<L"\tTimeStamp:\t";
			file<<st.wMonth<<L'/'<<st.wDay<<L'/'<<st.wYear<<L' '<<
				st.wHour<<L':'<<st.wMinute<<L':'<<st.wSecond;
			file<<std::endl;

			// Set file version
			file<<L"\tFileVersion:\t"<<HIWORD(i->version.dwFileVersionMS)<<L'.'<<
				LOWORD(i->version.dwFileVersionMS)<<L'.'<<
				HIWORD(i->version.dwFileVersionLS)<<L'.'<<
				LOWORD(i->version.dwFileVersionLS)<<std::endl;

			// Set product version
			file<<L"\tProductVersion:\t"<<HIWORD(i->version.dwProductVersionMS)<<L'.'<<
				LOWORD(i->version.dwProductVersionMS)<<L'.'<<
				HIWORD(i->version.dwProductVersionLS)<<L'.'<<
				LOWORD(i->version.dwProductVersionLS)<<std::endl;
			file<<std::endl;
		}

		file<<std::endl;
	}
}
