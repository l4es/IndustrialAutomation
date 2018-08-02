#pragma once
#include "Global.h"

namespace DVF	//Doc-View Framework
{
	class CBaseDocument;
	class CBaseView
	{
		friend class CBaseDocument;

	public:
		inline CBaseDocument* GetDocument(){return _Document;};

	protected:
		virtual void OnUpdate(){};
		virtual void OnInitialUpdate(){};

	private:
		CBaseDocument*	_Document;
	};
}