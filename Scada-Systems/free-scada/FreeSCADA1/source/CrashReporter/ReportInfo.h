#pragma once

#include <dbghelp.h>
#include <fstream>

namespace crash_reporter
{
	struct module_info_t
	{
		std::wstring full_path;
		ULONG image_size;
		VS_FIXEDFILEINFO version;
	};
	typedef std::vector<module_info_t> modules_info_t;

	class CReportInfo  
	{
	public:
		CReportInfo(PEXCEPTION_POINTERS ExceptionInfo);

		std::wstring getCrashLog();
		std::wstring getCrashFile();
		std::wstring getModuleName() { return m_sModule; };
		std::wstring getExceptionCode() { return m_sException; };
		std::wstring getExceptionAddr() { return m_sAddress; };
		size_t get_additional_files_count(){return m_additional_files.size();}
		std::wstring get_additional_file(size_t index){return m_additional_files[index];}

	private:
		static std::wstring m_sModule;
		static std::wstring m_sException;
		static std::wstring m_sAddress;
		std::vector<std::wstring> m_additional_files;
		modules_info_t m_modules_info;

		PEXCEPTION_POINTERS m_excpInfo;

		void write_exception_info(std::wofstream& file, EXCEPTION_RECORD* pExceptionRecord);
		void write_processor_info(std::wofstream& file);
		void write_os_info(std::wofstream& file);
		void write_modules_info(std::wofstream& file);
	};
}