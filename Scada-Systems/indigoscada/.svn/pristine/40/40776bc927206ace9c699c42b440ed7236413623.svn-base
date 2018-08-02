/**********************************************************************
--- Qt Architect generated file ---
File: ReceipeCfg.cpp
Last generated: Thu Apr 20 13:36:32 2000
*********************************************************************/
#include <qt.h>
#include "ReceipeCfg.h"
#define Inherited ReceipeCfgData
#include "main.h"
#include "dbase.h"
#include "common.h"
#include "NewDlg.h"
#include "driver.h"
#include "sptypes.h"
#include "IndentedTrace.h"


#define WDG_TAG 1


ReceipeCfg::ReceipeCfg
(QWidget * parent,
const char *name):Inherited (parent, name), pTimer (new QTimer (this))
{
	setCaption (tr ("Receipe Configuration"));
	connect (GetConfigureDb(),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));
	connect (GetCurrentDb(),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));

	GetConfigureDb()->DoExec(this,"select NAME from RECEIPE order by NAME asc;",tList);
	GetConfigureDb()->DoExec(this,"select NAME,COMMENT from UNITS order by NAME asc;",tUnitList); // get the list of units
	GetCurrentDb()->DoExec(this,"select NAME,TAGNAME from TAGS_DB order by NAME asc;",tTagList); // get the list of sample points and tags
	ButtonState (false);
	ApplyPressed = false;
	//connect (pTimer, SIGNAL (timeout ()), this, SLOT (DoSelChange ()));	// wire up the item selection timer
	//

	Comment->setMaxLength(MAX_LENGHT_OF_STRING);

	//
	//set the tab order
	// 
	QWidget *pL[] = {
		Name,
		Comment,
		UnitsList, UnitButton,
		SampleList, LimitsButton,
		ApplyButton,
		NewButton,
		DeleteButton,
		0
	};
	SetTabOrder (this, pL);

	connect (SampleList,SIGNAL (clicked ( QListViewItem *) ),this, SLOT (Clicked(QListViewItem *)));
	connect (SampleList,SIGNAL (returnPressed(QListViewItem*)),	this, SLOT (returnPressed(QListViewItem*)));

	// Create widgets and put them into a stack:
	//widgetStack = new QWidgetStack( this ); ...to in the next release....
}
ReceipeCfg::~ReceipeCfg ()
{
}

void ReceipeCfg::Help ()
{
	QSHelp("ConfigureReceipe");
}

void ReceipeCfg::Exit()
{
	IT_IT("ReceipeCfg::Exit");
	
	if(ApplyPressed)
	{
		//broadcast
		GetDispatcher()->DoExec(NotificationEvent::CMD_MONITOR_STOP);
		DOAUDIT(tr("Stop Monitor"));
		GetDispatcher()->DoExec(NotificationEvent::CMD_MONITOR_START);
		DOAUDIT(tr("Start Monitor"));
	}

	reject();
}

void ReceipeCfg::Delete ()
{
	if (!YESNO ("Delete Receipe", "Delete the Receipe - Are You Sure ?"))
	{
		QString cmd = "delete from RECEIPE where NAME='"+Name->currentText() +"';";
		GetConfigureDb ()->DoExec (this,cmd,tDelete);
		DOAUDIT(tr("Delete:") + Name->currentText());
		Name->removeItem(Name->currentItem());
		Name->setCurrentItem (0);
		SelChanged (0);
		Name->setFocus ();
	};
}
void ReceipeCfg::SetAll() // set all sample points
{
	QListViewItemIterator st (SampleList);
	for (; st.current (); st++)
	{
		QCheckListItem *p = (QCheckListItem *) (st.current ());
		p->setOn (true);
	};
};
void ReceipeCfg::SelChanged (int)
{
//	pTimer->stop ();		// cancel any timer action  
//	if (Name->count () > 0)
//	{
//		pTimer->start (100, TRUE);	// after inactivity fetch the record - we wan to avoid too much activity
//	};
}
void ReceipeCfg::DoSelChange ()
{
	QString cmd ="select * from RECEIPE where NAME ='" + Name->currentText () + "';";	// retrive the record
	GetConfigureDb ()->DoExec (this, cmd,tItem);
	ButtonState (false);
};
void ReceipeCfg::Apply ()
{
	Build ();
	QString cmd =
	GetConfigureDb ()->Update ("RECEIPE", "NAME", Name->currentText ());	// generate the update record
	GetConfigureDb ()->DoExec (this, cmd, tApply);	// lock the db and action the request
	ButtonState (false);
	DOAUDIT(tr("Apply:") + Name->currentText());
}
void ReceipeCfg::New ()
{
	// Get the new name
	NewDlg dlg (Name, this);
	if (dlg.exec ())
	{
		// 
		//
		Comment->setText (tr(NONE_STR));
		//
		// Clear the checks from the list
		//
		ClearAll();
		//
		// leave the language - so it defaults to current
		// 
		Name->insertItem (dlg.GetName ());	// add to the list box
		SetComboItem (Name, dlg.GetName ());	// make it the selected item 
		//
		Build ();		// build the record 
		GetConfigureDb ()->AddToRecord ("NAME", Name->currentText ());
		QString cmd = GetConfigureDb ()->Insert ("RECEIPE");	// generate the insert record
		GetConfigureDb ()->DoExec (this, cmd, tNew);	// lock the db and action the request
		ButtonState (false);
		//
		DOAUDIT(tr("New:") + dlg.GetName());
		//
	};
}
void ReceipeCfg::ClearAll ()
{
	QListViewItemIterator it (UnitsList);
	for (; it.current (); it++)
	{
		QCheckListItem *p = (QCheckListItem *) (it.current ());
		p->setOn (false);
	};
	QListViewItemIterator st (SampleList);
	for (; st.current (); st++)
	{
		QCheckListItem *p = (QCheckListItem *) (st.current ());
		p->setOn (false);
	};
};
void ReceipeCfg::ConfigureUnit ()
{
	if(!(Name->currentText()).isEmpty())
	{
		//
		// Get the unit class 
		// open the configuration dialog for the unit
		// get the unit type
		QListViewItem *p = UnitsList->currentItem();
		if(p)
		{
			QString cmd = "select UNITTYPE from UNITS where NAME='" + p->text(0) + "';";
			GetConfigureDb ()->DoExec (this, cmd, tUnitsType);	// action the request
		};
	}
}
#include "TagCfg.h"
void ReceipeCfg::AlarmLimits ()
{
	if(!(Name->currentText()).isEmpty())
	{
		//
		// Open the tag config dialog
		// 
		QCheckListItem* p = (QCheckListItem* ) SampleList->currentItem();
		if(p)
		{
			if(p->childCount())
			{
				QString cmd = "select * from SAMPLE where NAME='" + p->text(0) + "';";
				GetConfigureDb ()->DoExec (this, cmd, tTags);	// lock the db and action the request
			}
			else
			{
				QString parname = ((QCheckListItem *)p->parent())->text();
				QString cmd = "select * from SAMPLE where NAME='" + parname + "';";
				GetConfigureDb ()->DoExec (this, cmd, tTags);	// lock the db and action the request
			}
		};
	}
}
void ReceipeCfg::DoTags()
{
	QListViewItem *p = SampleList->currentItem();
	if(p)
	{
		if(p->childCount())
		{
			TagCfg dlg(this,p->text(0),sp_type,unit_type,Name->currentText(),sp_unit);
			if(dlg.exec())
			{
				DOAUDIT(tr("Alarm Limits Edited:") + p->text(0));
			};
		}
		else
		{
			TagCfg dlg(this,((QCheckListItem *)p->parent())->text(),sp_type,unit_type,Name->currentText(),sp_unit);
			if(dlg.exec())
			{
				DOAUDIT(tr("Alarm Limits Edited:") + ((QCheckListItem *)p->parent())->text());
			};
		}
	};
};
void ReceipeCfg::DoUnit()
{
	IT_IT("ReceipeCfg::DoUnit");
	
	Driver *dp = FindDriver(unit_type);
	if(dp)
	{
		QListViewItem *p = UnitsList->currentItem();
		if(p)
		{
			DOAUDIT(tr("Unit Configure:")+ p->text(0));
			dp->UnitConfigure(this,p->text(0),Name->currentText()); // add on the receipe name too
		};
	};
};
//

//
void ReceipeCfg::Build () // build the update record - the insert needs the name record addedt, units text, samples text);dtod text,enddate text);
{
	GetConfigureDb ()->ClearRecord ();	// clear the record
	GetConfigureDb ()->AddToRecord ("COMMENT", Comment->text ());
	//
	// now build the unit list
	QString uo;
	QTextOStream us (&uo);
	//
	QListViewItemIterator it (UnitsList);
	int nu = 0;
	for (; it.current (); it++)
	{
		QCheckListItem *p = (QCheckListItem *) (it.current ());
		if (p->isOn ())
		{
			us << p->text () << " ";	// add to the list
			nu++;
		};
	};
	//
	QString so;
	QTextOStream ss (&so);
	//
//	QListViewItemIterator st (SampleList);
//	int ns = 0;
//	for (; st.current (); st++)
//	{
//		QCheckListItem *p = (QCheckListItem *) (st.current ());
//		if (p->isOn ())
//		{
//			ss << p->text () << " ";	// add to the list
//			ns++;
//		};
//	};
			
	//
	int ns = 0;
	for(ItemDict::iterator j = spIdx.begin();!(j == spIdx.end()); j++)
	{
		if ((*j).second->isOn())
		{
			ss << (*j).second->text () << " ";	// add to the list
			ns++;
		};
	};

	// 
	QString s;
	s.setNum (nu);
	s = s + " " + uo;
	GetConfigureDb ()->AddToRecord ("UNITS", s);
	//
	s = "";
	s.setNum (ns);
	s = s + " " + so;
	GetConfigureDb ()->AddToRecord ("SAMPLES", s);
	//
};
void ReceipeCfg::QueryResponse (QObject *p, const QString &, int State, QObject*caller) // notify transaction completerequestrt needs the name record addedt, units text, samples text);dtod text,enddate text);
{
	if(p != this) return;

	IT_IT("ReceipeCfg::QueryResponse");

	switch (State)
	{
		//
		case tList:
		{
			// fill the name list box
			GetConfigureDb ()->FillComboBox (Name, "NAME");
			Name->setCurrentItem (0);
			SelChanged (0);
			Name->setFocus ();
			ButtonState (true);
		};
		break;
		case tItem:
		{
			// fill the fields     
			Comment->setText (UndoEscapeSQLText(GetConfigureDb ()->GetString ("COMMENT")));
			ClearAll ();
			//
			{
				//
				QString s = GetConfigureDb ()->GetString ("UNITS");
				QTextIStream is (&s);
				while (!is.atEnd ())
				{
					//
					QString a;
					is >> a;
					a = a.stripWhiteSpace ();
					//
					// find the item and check it
					QListViewItemIterator it (UnitsList);
					for (; it.current (); it++)
					{
						QCheckListItem *p = (QCheckListItem *) (it.current ());
						if (p->text () == a)
						{
							p->setOn (true);
							break;
						};
					};
				};
			};
			// 
			{
				//
				QString s = GetConfigureDb ()->GetString ("SAMPLES");
				QTextIStream is (&s);
				while (!is.atEnd ())
				{
					//
					QString a;
					is >> a;
					a = a.stripWhiteSpace ();
					//
					QListViewItemIterator it (SampleList);
					for (; it.current (); it++)
					{
						QCheckListItem *p = (QCheckListItem *) (it.current ());
						if (p->text () == a)
						{
							p->setOn (true);
							//set tags
							//SpDict::iterator i =  Items.find(p->text());
							//if(!(i == Items.end()))
							//{
							//	for(ItemDict::iterator j = (*i).second.begin();!(j == (*i).second.end()); j++)
							//	{
							//		(*j).second->setOn (true);
							//	};
							//};
							break;
						};
					};
				};
			};
			//
			Comment->setFocus ();
			ButtonState (true);
		};
		break;
		case tUnitsType:
		{
			IT_COMMENT("tUnitsType");
			
			//
			// we now have the unit type - we can now open the unit configuration 
			// but not on this call or the database will block 
			unit_type = GetConfigureDb()->GetString("UNITTYPE");
			QTimer::singleShot(10,this,SLOT(DoUnit()));
		};
		break;
		case tTags: 
		{
			sp_type = GetConfigureDb()->GetString("QTYPE");
			sp_unit =  GetConfigureDb()->GetString("UNIT");
			QString cmd = "select UNITTYPE from UNITS where NAME='" + GetConfigureDb()->GetString("UNIT") + "';";
			GetConfigureDb()->DoExec(this,cmd,tCfgTag);
		};
		break;
		case tCfgTag:
		{
			unit_type = GetConfigureDb()->GetString("UNITTYPE");
			QTimer::singleShot(10,this,SLOT(DoTags())); // action it
		};
		break;
		case tUnitList:
		{
			unsigned n = GetConfigureDb ()->GetNumberResults ();
			//
			UnitsList->clear ();
			//
			if (n)
			{
				for (unsigned i = 0; i < n; i++,GetConfigureDb()->FetchNext())
				{
					QCheckListItem *p = new QCheckListItem(UnitsList,GetConfigureDb ()->GetString ("NAME"),QCheckListItem::CheckBox);
					p->setText(1,UndoEscapeSQLText(GetConfigureDb()->GetString ("COMMENT")));
				};
			};
		};
		break;
		case tTagEnabled:
		{
			unsigned n = GetConfigureDb ()->GetNumberResults ();

			for (unsigned i = 0; i < n ; i++,GetConfigureDb()->FetchNext())
			{
				SpDict::iterator i =  Items.find(GetConfigureDb()->GetString("NAME"));
				if(!(i == Items.end()))
				{
					ItemDict::iterator j = (*i).second.find(GetConfigureDb()->GetString("TAG"));
					
					if(!(j == (*i).second.end()))
					{
						if(GetConfigureDb()->GetInt("ENABLED"))
						{
							(*j).second->setEnabled (true);
							(*j).second->setOn(true);
						}
						else
						{
							(*j).second->setEnabled (true);
							(*j).second->setOn(false);
						}
					}
				};
			};
			
			SampleList->triggerUpdate();
			ButtonState (true);
		}
		break;
		case tSampleList:
		{
			unsigned n = GetConfigureDb ()->GetNumberResults ();
			//
			//
			for (unsigned i = 0; i < n ; i++,GetConfigureDb()->FetchNext())
			{
				ItemDict::iterator j = spIdx.find(GetConfigureDb()->GetString("NAME"));
				//
				if(!(j == spIdx.end()))
				{
					(*j).second->setText(1,UndoEscapeSQLText(GetConfigureDb()->GetString ("COMMENT")));
				};
			};

			QString cmd ="select * from RECEIPE where NAME ='" + Name->currentText () + "';";	// retrive the record
			GetConfigureDb ()->DoExec (this, cmd,tItem);

			GetConfigureDb()->DoExec(this,"select NAME,TAG,ENABLED from TAGS where RECEIPE='"+Name->currentText()+"' order by NAME asc;",tTagEnabled); // get the list of enabled tags
		};
		break;
		case tTagList://start initialization of list
		{
			// build the sample point tag list - build as a tree

			SampleList->clear();

			int n = GetCurrentDb()->GetNumberResults();
			QString last = QString::null; 
			QCheckListItem *item = 0;
			//
			Items.clear();
			//
			for(int i = 0; i < n; i++, GetCurrentDb()->FetchNext())
			{
				if(last != GetCurrentDb()->GetString("NAME"))
				{
					//
					last = GetCurrentDb()->GetString("NAME");   
					ItemDict d;
					SpDict::value_type pr(last,d);
					Items.insert(pr);
					//     
					item = new QCheckListItem(SampleList,last,QCheckListItem::CheckBox); 

					ItemDict::value_type prs(last, item);
					spIdx.insert(prs);
				};
				//
				//
				QCheckListItem*pi =  new QCheckListItem(item, GetCurrentDb()->GetString("TAGNAME"),QCheckListItem::CheckBox);
				pi->setEnabled (false);
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

			GetConfigureDb()->DoExec(this,"select NAME,COMMENT from SAMPLE order by NAME asc;",tSampleList); // get the list of sample points
		};
		break;
		case tUnchekTag:
		{
			if(GetConfigureDb()->GetNumberResults() == 1)
			{
				QString cmd = "update TAGS set ENABLED=0 where NAME='" + GetConfigureDb()->GetString("NAME")+
				"' and TAG='"+GetConfigureDb()->GetString("TAG")+ "' and RECEIPE ='"+GetConfigureDb()->GetString("RECEIPE")+"';";
				GetConfigureDb()->DoExec(0,cmd,0);
			}

			ButtonState(true);
		}
		break;
		case tTagItem:
		{
			if(GetConfigureDb()->GetNumberResults() == 1)
			{

				QString cmd = "update TAGS set ENABLED=1 where NAME='" + GetConfigureDb()->GetString("NAME")+
				"' and TAG='"+GetConfigureDb()->GetString("TAG")+ "' and RECEIPE ='"+GetConfigureDb()->GetString("RECEIPE")+"';";
				GetConfigureDb()->DoExec(0,cmd,0);
			}
			else
			{
				QSTransaction &t = GetConfigureDb()->CurrentTransaction();
			
				//																				//curren receipe name
				QString cmd = "insert into TAGS values ('"+t.Data1+"','"+t.Data2+"'"TAG_VALS"'"+Name->currentText()+"',1,0,'','');";
				GetConfigureDb()->DoExec(0,cmd,0);
			};

			ButtonState(true);
		}
		break;
		case tApply:
		{
			for(ItemDict::iterator j = spIdx.begin();!(j == spIdx.end()); j++)
			{
				if ((*j).second->isOn())
				{
					QString activeTags = "select * from TAGS where NAME='"
					 +(*j).second->text () + "' and TAG='";

					SpDict::iterator i =  Items.find((*j).second->text ());
					if(!(i == Items.end()))
					{
						for(ItemDict::iterator k = (*i).second.begin();!(k == (*i).second.end()); k++)
						{	
							if((*k).second->isOn())
							{																	//nome ricetta corrente
								QString cmd = activeTags + (*k).second->text() + "' and RECEIPE='"+Name->currentText()+"';";
								GetConfigureDb ()->DoExec (this,cmd,tTagItem,(*j).second->text (),(*k).second->text());
								ButtonState(false);
							}
							else
							{
								QString cmd = activeTags + (*k).second->text() + "' and RECEIPE='"+Name->currentText()+"';";
								GetConfigureDb ()->DoExec (this,cmd,tUnchekTag);
								ButtonState(false);
							}
						};
					};
				}
				else
				{
					QString activeTags = "select * from TAGS where NAME='"
					 +(*j).second->text () + "' and TAG='";

					SpDict::iterator i =  Items.find((*j).second->text ());
					if(!(i == Items.end()))
					{
						for(ItemDict::iterator k = (*i).second.begin();!(k == (*i).second.end()); k++)
						{																		//nome ricetta corrente
							QString cmd = activeTags + (*k).second->text() + "' and RECEIPE='"+Name->currentText()+"';";
							GetConfigureDb ()->DoExec (this,cmd,tUnchekTag);
							ButtonState(false);
						};
					};
				}
			};

			ApplyPressed = true;
		}
		break;
		case tDelete:
		case tNew:
		ButtonState (true);
		default:
		break;
	};
};


void  ReceipeCfg::Clicked(QListViewItem * item)
{
	QCheckListItem* p = (QCheckListItem*)item;

	if(p)
	{
		if (!p->isOn())
		{
			SpDict::iterator i =  Items.find(p->text());
			if(!(i == Items.end()))
			{
				for(ItemDict::iterator j = (*i).second.begin();!(j == (*i).second.end()); j++)
				{
					(*j).second->setOn(false);
					(*j).second->setEnabled (false);
				};
			};
		}
		else
		{
			SpDict::iterator i =  Items.find(p->text());
			if(!(i == Items.end()))
			{
				for(ItemDict::iterator j = (*i).second.begin();!(j == (*i).second.end()); j++)
				{
					(*j).second->setEnabled (true);
				};
			};
		}
	}
}


void  ReceipeCfg::returnPressed(QListViewItem * item)
{

	QCheckListItem* p = (QCheckListItem*)item;

	if (!p->isOn())
	{
		SpDict::iterator i =  Items.find(p->text());
		if(!(i == Items.end()))
		{
			for(ItemDict::iterator j = (*i).second.begin();!(j == (*i).second.end()); j++)
			{
				(*j).second->setOn(false);
				(*j).second->setEnabled (false);
			};
		};
	}
	else
	{
		SpDict::iterator i =  Items.find(p->text());
		if(!(i == Items.end()))
		{
			for(ItemDict::iterator j = (*i).second.begin();!(j == (*i).second.end()); j++)
			{
				(*j).second->setEnabled (true);
			};
		};
	}
}

