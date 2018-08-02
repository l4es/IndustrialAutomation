#pragma once

using namespace std;
using namespace ATL;

namespace Helpers
{
	enum enDatabaseType
	{
		DBT_ACCESS,
		DBT_MYSQL
	};

	class CApplicationSettings
	{
	public:
		CApplicationSettings(void);
		virtual ~CApplicationSettings(void);

		enDatabaseType GetDatabaseType();
		void SetDatabaseType(enDatabaseType NewType);

		CString GetDatabaseName();
		void SetDatabaseName(CString NewName);

		CString GetDatabasePath();
		void SetDatabasePath(CString NewPath);

		CString GetDatabaseServer();
		void SetDatabaseServer(CString NewServer);

		CString BuildConnectionString(CString User, CString Password);
		CString BuildConnectionString();

	protected:
		HelpersLib::CCfgReg _reg;	
	};

	extern CApplicationSettings g_ApplicationSettings;
}