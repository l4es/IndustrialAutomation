#pragma once

#ifdef GetUserName
	#undef GetUserName
#endif

namespace Helpers
{
	class CApplicationSettings
		: public CSingleton<CApplicationSettings>
	{
	public:
		//Настройка хранятся в реестре в закодированном виде
		CString GetDBUserName();	//Получить логин пользователя для входа в БД
		CString GetDBUserPass();	//Получить пароль пользователя для входа в БД

	protected:
		//Запись в реестре
		struct sUserPass
		{
			wchar_t User[128];
			wchar_t Password[128];
		};
		HelpersLib::CCfgReg _reg;

	protected:
		CApplicationSettings(void);
		~CApplicationSettings(void);
		DEF_SINGLETON(CApplicationSettings);

		//Закодировать структуру
		BOOL EncodeUserPass(sUserPass SrcData, vector<BYTE> &DstData);
		//Декодировать структуру
		BOOL DecodeUserPass(vector<BYTE> &SrcData, sUserPass& DstData);
	};
}