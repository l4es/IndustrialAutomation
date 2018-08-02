#pragma once

#define _ATL_DISABLE_DEPRECATED
#include <atlbase.h>

#include "Global.h"

namespace HelpersLib
{
	class HELPERS_LIB_API CCfgReg
	{
	private:
		ATL::CRegKey _reg;
	public:
		CCfgReg(void);
		~CCfgReg(void);

		void Open(tstring Product, bool ReadOnly=false, bool UsePersonalSetting=true);

		tstring ReadString(tstring ValueName, tstring Default=tstring());
		void WriteString(tstring ValueName, tstring Value);

		DWORD ReadDWORD(tstring ValueName, DWORD Default=NULL);
		void WriteDWORD(tstring ValueName, DWORD Value);

		float ReadFloat(tstring ValueName, float Default=NULL);
		void WriteFloat(tstring ValueName, float Value);

		BOOL ReadBinary(tstring ValueName, vector<BYTE>& Data);
		void WriteBinary(tstring ValueName, vector<BYTE>& Data);
	};
}
