#pragma once

namespace Helpers
{
	class CErrorReporter
		:public CSingleton<CErrorReporter>
	{
	public:
		void ShowError(IErrorPtr pErr);
	protected:
		CErrorReporter(void);
		virtual ~CErrorReporter(void);
		DEF_SINGLETON(CErrorReporter)		
	};
	typedef CSingletonPtr<CErrorReporter> CErrorReporterPtr;
}