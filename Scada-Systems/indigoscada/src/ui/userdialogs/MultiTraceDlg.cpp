/**********************************************************************
--- Qt Architect generated file ---
File: MultiTraceDlg.cpp
Last generated: Wed Jun 28 10:28:28 2000
*********************************************************************/
#include "MultiTraceDlg.h"
#define Inherited MultiTraceDlgData
#include <qt.h>
#include "dbase.h"
#include "main.h"

MultiTraceDlg::MultiTraceDlg
(
QWidget* parent,
const char* name
)
:
Inherited( parent, name )
{
	connect (GetCurrentDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (CurrentQueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//
	GetCurrentDb()->DoExec(this,"select NAME,TAGNAME from TAGS_DB;",tList); // get the list of sample points and tags
	//
}
MultiTraceDlg::~MultiTraceDlg()
{
}
void MultiTraceDlg::Load(const QString &s)
{
	// clear all checks
	// load magic - valid ?
	// load the rest
	QFile fs(s);
	if(fs.open(IO_ReadOnly))
	{
		//
		QDataStream is(&fs);
		QString sp; // sample point
		QString tg; // tag
		//
		is >> sp;
		if(sp == "MULTITRACE") // magic phrase
		{
			//
			SpDict::iterator i = Items.begin();
			for(; !(i == Items.end()); i++)
			{
				ItemDict::iterator j = (*i).second.begin();
				for(; !(j == (*i).second.end()); j++)
				{   
					(*j).second->setOn(0); // un check                    
				};
			};
			//
			while(!is.atEnd()) // look for selected items
			{
				is >> sp >> tg;
				SpDict::iterator i = Items.find(sp);
				if(!(i == Items.end()))
				{
					ItemDict::iterator j = (*i).second.find(tg);
					if(!(j == (*i).second.end()))
					{
						(*j).second->setOn(true);
					};
				};
			};
		};
	};
};
void MultiTraceDlg::Save(const QString &s)
{
	// save the magic number
	// save name / tags of items that are checked
	QFile fs(s);
	if(fs.open(IO_WriteOnly))
	{
		//
		QDataStream os(&fs);
		os << QString("MULTITRACE"); // magic number
		SpDict::iterator i = Items.begin();
		//
		for(; !(i == Items.end()); i++)
		{
			ItemDict::iterator j = (*i).second.begin();
			for(; !(j == (*i).second.end()); j++)
			{   
				if((*j).second->isOn())                     
				{
					os << (*i).first << (*j).first; // write out the check items only
				};
			};
		};
	};		
};
void MultiTraceDlg::CurrentQueryResponse (QObject *p,const QString &, int id, QObject* caller) // current value responses
{
	if(p != this) return;
	switch(id)
	{
		case tList:
		{
			//
			// build the sample point tag list - build as a tree
			int n = GetCurrentDb()->GetNumberResults();
			QString last; // last name
			QCheckListItem *item = 0;
			//
			Items.clear();
			//    
			for(int i = 0; i < n; i++,GetCurrentDb()->FetchNext())
			{
				if(last != GetCurrentDb()->GetString("NAME"))
				{
					//
					last = GetCurrentDb()->GetString("NAME");   
					ItemDict d;
					SpDict::value_type pr(last,d);
					Items.insert(pr);
					//     
					item = new QCheckListItem(List,last); // created as a controller
				};
				//
				//
				QCheckListItem*pi =  new QCheckListItem(item, GetCurrentDb()->GetString("TAGNAME"),QCheckListItem::CheckBox);
				SpDict::iterator j = Items.find(last);
				//
				if(!(j == Items.end()))
				{
					ItemDict::value_type pr(GetCurrentDb()->GetString("TAGNAME"),pi);     
					(*j).second.insert(pr);
				};
				//
				//
			};
			List->triggerUpdate();
		};
		break;
		default:
		break;
	};
};

