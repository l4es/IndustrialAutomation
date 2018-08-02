#pragma once

#ifdef GetUserName
#undef GetUserName
#endif

using namespace std;

namespace ModuleArchive
{
namespace Helpers
{
	enum enDatabaseType
	{
		DBT_ACCESS,
		DBT_MYSQL
	};

	class CApplicationSettings
		: public CSingleton<CApplicationSettings>
	{
	public:
		CApplicationSettings(void);
		virtual ~CApplicationSettings(void);

		CString GetDBUserName();
		CString GetDBUserPass();
		void SetDBUser(CString Name, CString Pass);

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
		HelpersLib::CCfgReg _DBReg;	
		HelpersLib::CCfgReg _UsersReg;	

		struct sUserPass
		{
			wchar_t User[128];
			wchar_t Password[128];
		};

		DEF_SINGLETON(CApplicationSettings);

		BOOL EncodeUserPass(sUserPass SrcData, vector<BYTE> &DstData);
		BOOL DecodeUserPass(vector<BYTE> &SrcData, sUserPass& DstData);
	};

	extern CApplicationSettings g_ApplicationSettings;
}

}