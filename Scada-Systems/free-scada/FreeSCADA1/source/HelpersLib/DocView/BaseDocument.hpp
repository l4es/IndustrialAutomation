#pragma once
#include "BaseView.hpp"

#include "Global.h"
#include "Singleton.hpp"

namespace DVF	//Doc-View Framework
{
	using namespace std;
	class CBaseDocument
	{
	public:
		typedef list<CBaseView*> TVBaseView;

		CBaseDocument(void){}
		~CBaseDocument(void){}

		inline void AddView(CBaseView* View)
		{
			typedef TVBaseView::iterator it;
			it i=find(_Views.begin(), _Views.end(), View);
			if(i == _Views.end())
			{
				View->_Document = this;
				_Views.push_back(View);
				View->OnInitialUpdate();
			}
		}

		inline void DelView(CBaseView* View)
		{
			typedef TVBaseView::iterator it;
			it i=find(_Views.begin(), _Views.end(), View);
			if(i != _Views.end())
				_Views.erase(i);
		}

		inline void UpdateAllViews(CBaseView* Sender = NULL)
		{
			//Вызвать функцию OnUpdate у всех видов кроме вызвавшего эту функцию
			typedef TVBaseView::iterator it;
			for(it i=_Views.begin(); i!=_Views.end(); i++)
			{
				if(*i != Sender)
					(*i)->OnUpdate();
			}
		};
	private:
		TVBaseView	_Views;
	};
};