/**********************************************************************
--- Qt Architect generated file ---
File: SampleCfg.cpp
Last generated: Thu Apr 13 16:15:49 2000
The sample configuration is possibly the most complex dialog because there 
are several inter dependent relationships to be handled.
*********************************************************************/
#include "SampleCfg.h"
#define Inherited SampleCfgData
#include "dbase.h"
#include "main.h"
#include "common.h"
#include "NewDlg.h"
#include "TagCfg.h"
#include "TagCfgDigital.h"
#include "driver.h"
#include "ActionConfigure.h"
#include "sptypes.h"
#include "IndentedTrace.h"

SampleCfg::SampleCfg
(QWidget * parent,
const char *name):Inherited (parent, name),
pTimer (new QTimer (this)),pConfig (0),
pCfgDriver(0)
{
	IT_IT("SampleCfg::SampleCfg");
	
	setCaption (tr ("Sample Point Configuration"));
	unit_type = tr(NONE_STR);
	//
	connect (GetConfigureDb(),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	connect (GetResultDb(),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//
	GetConfigureDb()->DoExec(this,"select NAME,COMMENT from SAMPLE order by NAME asc;",tList);
	GetConfigureDb()->DoExec(this,"select NAME from UNITS order by NAME asc;",tUnitList);
	ButtonState (false);
	//
	connect (pTimer, SIGNAL (timeout ()), this, SLOT (DoSelChange ()));	// wire up the item selection timer
		
	InputIndex->insertItem(tr(NONE_STR));
			
	Type->insertItem (tr(NONE_STR));
	//
	Comment->setMaxLength(MAX_LENGHT_OF_STRING);
	//set the tab order
	// 
	QWidget *pL[] =
	{
		List, Comment, Unit, Type, InputIndex, Units,
		Enabled, Fileable,LogMode,Retrigger,AlarmThreshold, 0
	};
	SetTabOrder (this, pL);
}
SampleCfg::~SampleCfg ()
{
	IT_IT("SampleCfg::~SampleCfg");
	
	GetConfigureDb()->disconnect(this);
}
void SampleCfg::Help ()
{
	IT_IT("SampleCfg::Help");
	
	QSHelp("ConfigureSamplePoints");
}
void SampleCfg::SelChanged (int)
{
	IT_IT("SampleCfg::SelChanged");
	
	pTimer->stop ();		// cancel any timer action 
	if (List->currentItem())
	{
		Name->setText((List->currentItem())->text(0));
		pTimer->start (100, TRUE);	// after inactivity fetch the record - we wan to avoid too much activity
	};
}
void SampleCfg::Limits ()
{
	IT_IT("SampleCfg::Limits");

	QString Ty;
	Ty = Type->currentText();

	if(Ty == TYPE_M_SP_TB_1 || Ty == TYPE_M_SP_NA_1 || Ty == TYPE_M_DP_TB_1 || Ty == TYPE_M_DP_NA_1)
	{
		//Digital single or double point
		TagCfgDigital dlg (this, Name->text (),Type->currentText(),unit_type,"(default)",Unit->currentText());	// edit the alarm limits
		dlg.exec ();
	}
	else //Analog type
	{
		// edit the tag table
		TagCfg dlg (this, Name->text (),Type->currentText(),unit_type,"(default)",Unit->currentText());	// edit the alarm limits
		dlg.exec ();
	}
}
void SampleCfg::RemoveConfig()
{
	IT_IT("SampleCfg::RemoveConfig");
	
	if(pConfig) // delete the old specific configuration widget
	{
		pConfig->hide();
		pConfig->close(true); // close and delete
		pConfig = 0;
	};
};
void SampleCfg::ShowConfig() // create the specific config
{
	IT_IT("SampleCfg::ShowConfig");
	
	RemoveConfig();

	if(pCfgDriver)
	{
		// was there a change in sample point type
		// the config widget has the frame widget as its parent
		pConfig = pCfgDriver->GetSpecificConfig(ConfigFrame,Name->text(), Type->currentText()); //specific config for sample point of type
		if(pConfig)
		{
			pConfig->show();
			//
			// make the connections
			connect(this,SIGNAL(LoadConfig(const QString &)),pConfig,SLOT(Load(const QString &))); //
			connect(this,SIGNAL(SaveConfig(const QString &)),pConfig,SLOT(Save(const QString &))); //
			//
			#ifdef DEPRECATED_IEC104_CONFIG
			QString cmd = "select IKEY,DVAL from PROPS where SKEY ='SAMPLEPROPS' and IKEY='"+Name->text()+"';"; //commented out on 02-12-09
			#else
			QString cmd = "select NAME,IOA,PARAMS from TAGS where NAME ='"+ Name->text() +"';";
			#endif
			GetConfigureDb()->DoExec(this,cmd,tConfigLoad);
			// ask for the configuration
		}
	}

};
void SampleCfg::TypeChanged (int)
{
	IT_IT("SampleCfg::TypeChanged");
	//
	if(pCfgDriver)
	{
		CurrentTypeList.clear(); // get the list of permitted indices
		pCfgDriver->GetInputList(Type->currentText(), CurrentTypeList,Unit->currentText(),Name->text());    
		if(CurrentTypeList.count() < 1) CurrentTypeList << tr(NONE_STR); // handle empty list 
		InputIndex->clear(); // put them in the list box
		GetConfigureDb()->DoExec(this,
		"select IPINDEX from SAMPLE where UNIT='" +
		Unit->currentText() + "' and QTYPE='" + Type->currentText() + "';",tIpIndex);
	};
	
	IT_COMMENT1("Tmp Input Index %s", (const char *)tmp_ip_index);
	//
	ShowConfig();
	//
}
void SampleCfg::Rename()
{
	IT_IT("SampleCfg::Rename");
	
	NewDlg dlg (List, this);
	dlg.setCaption (tr ("Rename Sample Point") + " - " + Name->text());
	if (dlg.exec ())
	{
		// rename the sample point configuration
		GetConfigureDb()->DoExec(0,
		"update SAMPLE set NAME='"+dlg.GetName()+"' where NAME='"+Name->text()+"';",0);
		//
		// update the tags
		GetConfigureDb()->DoExec(0,
		"update TAGS set NAME='"+dlg.GetName()+"' where NAME='"+Name->text()+"';",0);
		//
		// update any properties
		QString cmd = "update PROPS set IKEY='" + dlg.GetName() + "' where SKEY ='SAMPLEPROPS' and IKEY='"+Name->text()+"';";
		GetConfigureDb()->DoExec(this,cmd,tConfigLoad);
		//
		// Unit configurations and alarm groups and receipes cannot be easily updated
		// it is expected that rename is used after auto generation of sample points upon
		// unit creation
		// 
		DOAUDIT(tr("Rename:") + Name->text() + tr(" to ") + dlg.GetName());
		Name->setText(dlg.GetName()); // set the name
		//
		List->currentItem()->setText(0,dlg.GetName());
	};
};
void SampleCfg::Delete ()
{
	IT_IT("SampleCfg::Delete");
	
	if (!YESNO
	(tr("Delete Sample Point"), tr("Delete the Sample Point - Are You Sure ?")))
	{
		//
		QString cmd = GetConfigureDb ()->Delete ("SAMPLE", "NAME", Name->text ());
		GetConfigureDb ()->DoExec (0,cmd,0);
		// delete the tag configuration
		cmd ="delete from TAGS where NAME='" + Name->text () + "';";
		GetConfigureDb ()->DoExec (0,cmd,0);
		//
		// delete any properties
		cmd ="delete from PROPS where SKEY='SAMPLEPROPS' and IKEY='" + Name->text () + "';";
		GetConfigureDb ()->DoExec (this,cmd,tDelete);
		//
		// clean up the current values database
		cmd ="delete from TAGS_DB where NAME='" + Name->text () + "';";
		GetCurrentDb ()->DoExec (0,cmd,0);
		//
		cmd ="delete from CVAL_DB where NAME='" + Name->text () + "';";
		GetCurrentDb ()->DoExec (0,cmd,0);
		//
		cmd ="drop table " + Name->text () + ";";
		GetResultDb ()->DoExec (0,cmd,0);
		GetHistoricResultDb()->DoExec (0,cmd,0);

		if(pCfgDriver)
		{
			DOAUDIT(tr("Drop Unit Table:") + Name->text ());
			pCfgDriver->DropOneSpecTable(Name->text()); 
		}
		//
		cmd ="delete from ACTIONS where NAME='" + Name->text () + "';";
		GetConfigureDb ()->DoExec (this,cmd,tDelete);
		//

		DOAUDIT(tr("Delete:") + Name->text());
		//
		if((List->childCount() > 0) && List->currentItem())
		{
			QListViewItem* p = List->currentItem();
			List->takeItem(p);
			delete p;
			List->setCurrentItem (List->firstChild());
			SelChanged (0);
			//Name->setFocus ();
		};
		//
	};
}
void SampleCfg::UnitChanged (int)
{
	IT_IT("SampleCfg::UnitChanged");
	
	// unit has changed get the available input indices from the driver
	if(Unit->currentText() != tr(NONE_STR))
	{ 
		QString cmd = "select UNITTYPE from UNITS where NAME='" +  Unit->currentText() + "';";
		GetConfigureDb ()->DoExec (this,cmd,tUnitType);
	};
	pCfgDriver = 0;
}
void SampleCfg::Actions ()
{
	IT_IT("SampleCfg::Actions");
	
	// open the action editor
	ActionConfigure dlg(this,Name->text());
	dlg.exec();
}
void SampleCfg::Apply ()
{
	IT_IT("SampleCfg::Apply");
	
	Build ();
	//
	tmp_type = Type->currentText();
	//
	if(pConfig)
	{
		// save the configuration
		emit SaveConfig (Name->text ());
	};
	//
	QString cmd =
	GetConfigureDb()->Update ("SAMPLE", "NAME", Name->text ());	// generate the update record
	GetConfigureDb()->DoExec (this, cmd,tApply);	
	ButtonState (false);
	//
	// Make sure the tag list is correct
	// query for the current tag list
	// make sure the tags and current tags are all upto date
	// 
	GetConfigureDb()->DoExec(this,"select * from TAGS where NAME='"+Name->text()+"';",tTagList); // get the current tag list
	//  
	if(tmp_ip_index != InputIndex->currentText())
	{
		// the input index changed - update the type list
		int id = CurrentTypeList.findIndex(InputIndex->currentText()); // remove the now used index          
		if(id != -1)
		{
			CurrentTypeList.remove(CurrentTypeList.at(id)); // delete it 
		};
		CurrentTypeList << tmp_ip_index; // add the old (now free) index
		tmp_ip_index = InputIndex->currentText(); // save the current index
	};
	//
	// Update the comment field - for the current item
	// 
	if(List->currentItem())
	{
		(List->currentItem())->setText(1,Comment->text());
	};
	// 
	DOAUDIT(tr("Apply:") + Name->text());
	//
}
void SampleCfg::New ()
{
	IT_IT("SampleCfg::New");
	
	//
	// Save any existing configuration for any existing point
	//
	//
	NewDlg dlg (List, this);
	dlg.setCaption (tr ("New Sample Points"));
	if (dlg.exec ())
	{
		// delete the config widget
		RemoveConfig();
		// 
		SetComboItem (Unit, tr(NONE_STR));
		InputIndex->clear ();
		InputIndex->insertItem(tr(NONE_STR));
		InputIndex->setCurrentItem (0);
		// 
		Comment->setText (tr(NONE_STR));
		(void) new QListViewItem(List,dlg.GetName (),tr(NONE_STR));	// add to the list box
		Enabled->setChecked (0);
		Fileable->setChecked (0);
		Retrigger->setChecked(0);
		AlarmThreshold->setValue(0);
		//
		SetListViewItem (List, dlg.GetName ());	// make it the selected item 
		Name->setText (dlg.GetName ());
		//
		Build ();		// build the record 
		GetConfigureDb ()->AddToRecord ("NAME", Name->text ());
		QString cmd = GetConfigureDb ()->Insert ("SAMPLE");	// generate the update record
		GetConfigureDb ()->DoExec (this, cmd, tNew);	// lock the db and action the request
		//
		ButtonState (false);
		DOAUDIT(tr("New:") + dlg.GetName());
	}
}
void SampleCfg::DoSelChange ()
{
	IT_IT("SampleCfg::DoSelChange");

	QString cmd = "select * from SAMPLE where NAME='"+Name->text()+"';";
	GetConfigureDb ()->DoExec (this, cmd, tItem);	// lock the db and action the request
	ButtonState (false);
};
void SampleCfg::Build () // build the update record - the insert needs the name record addedivityoints listamples text);;dtod text,enddate text);
{
	IT_IT("SampleCfg::Build ");
	
	GetConfigureDb()->ClearRecord();	// clear the record
	GetConfigureDb()->AddToRecord("COMMENT", Comment->text ());
	GetConfigureDb()->AddToRecord("UNIT", Unit->currentText ());
	GetConfigureDb()->AddToRecord("QTYPE", Type->currentText ());
	GetConfigureDb()->AddToRecord("IPINDEX", InputIndex->currentText ());
	GetConfigureDb()->AddToRecord("UNITS", Units->currentText ());
	GetConfigureDb()->AddBool("ENABLED",Enabled->isChecked ());
	GetConfigureDb()->AddBool("FILEABLE",Fileable->isChecked ());
	GetConfigureDb()->AddBool("LOGMODE",LogMode->isChecked());
	GetConfigureDb()->AddBool("RETRIGGER",Retrigger->isChecked());
	GetConfigureDb()->AddToRecord("ALARMTHRESHOLD",AlarmThreshold->text());
};
void SampleCfg::IpIndexChanged(int)
{
	IT_IT("SampleCfg::IpIndexChanged");
};
void SampleCfg::QueryResponse (QObject *p, const QString &, int State, QObject*caller) // notify transaction completerequestrt needs the name record addedivityoints listamples text);;dtod text,enddate text);
{
	if(p != this) return;

	IT_IT("SampleCfg::QueryResponse");
	
	switch (State)
	{
		case tList:
		{
			// fill the name list box
			List->clear();
			int n = GetConfigureDb()->GetNumberResults();
			for(int i = 0; i < n; i++,GetConfigureDb()->FetchNext())
			{
				(void) new QListViewItem(List,GetConfigureDb ()->GetString("NAME"),UndoEscapeSQLText(GetConfigureDb()->GetString("COMMENT")));
			};
			List->setCurrentItem (List->firstChild());
			List->setFocus();
			SelChanged(0); // force the top item to fetch
			//
		};
		break;			// we have chained a transaction
		case tUnitList:
		{
			GetConfigureDb ()->FillComboBox (Unit, "NAME");
			Unit->insertItem (tr(NONE_STR));
			SetComboItem(Unit,tr(NONE_STR));
			ButtonState (true);
		};
		break;
		case tItem:
		{
			//  
			// fill the fields  
			//
			Comment->setText (UndoEscapeSQLText(GetConfigureDb()->GetString ("COMMENT")));
			Units->setEditText (GetConfigureDb ()->GetString ("UNITS"));
			Enabled->setChecked (GetConfigureDb ()->GetBool ("ENABLED"));
			Fileable->setChecked (GetConfigureDb ()->GetBool ("FILEABLE"));
			LogMode->setChecked (GetConfigureDb ()->GetBool("LOGMODE"));
			Retrigger->setChecked (GetConfigureDb ()->GetBool("RETRIGGER"));
			AlarmThreshold->setValue(GetConfigureDb ()->GetInt("ALARMTHRESHOLD"));
			//Comment->setFocus ();
			List->setFocus();
			ButtonState (true);
			//
			// save the input index and type until we have sorted out the Unit -> Unit Class -> type list -> input indices
			//
			tmp_ip_index = GetConfigureDb ()->GetString ("IPINDEX");
			tmp_type = GetConfigureDb ()->GetString ("QTYPE");
			//    
			//    
			IT_COMMENT2("Handling tItem new type , index %s %s", (const char *) tmp_ip_index, (const char *)tmp_type);
						
			// Now query the driver DLL for what the input type options are and what input indices are available
			if(Unit->currentText() != GetConfigureDb ()->GetString ("UNIT"))
			{
				// this is an expensive operation - so avoid where possible  
				SetComboItem (Unit, GetConfigureDb ()->GetString ("UNIT"));
				UnitChanged (0);
			}
			else
			{
				// unit type has not changed
				// has the input type changed
				SetComboItem(Type,tmp_type); // set to the target type
				TypeChanged(0); // get the list of permitted input indices
				//
			};
		};
		break;
		case tUnitType:
		{
			unit_type = tr(NONE_STR);
			if(GetConfigureDb ()->GetNumberResults() > 0)
			{
				unit_type = GetConfigureDb()->GetString("UNITTYPE"); // store it
				pCfgDriver = FindDriver(unit_type); // get the driver interface object
				//
				// set the permitted types
				// 
				Type->clear();
				//
				if(pCfgDriver)
				{ // what types ar epermitted
					pCfgDriver->SetTypeList(Type, Unit->currentText());
				}
				else
				{
					// there is no driver - set all to none
					Type->insertItem(tr(NONE_STR));
					tmp_ip_index = tr(NONE_STR);
					tmp_type = tr(NONE_STR);
					InputIndex->clear();
					InputIndex->insertItem(tr(NONE_STR));
				};
				//
				SetComboItem(Type,tmp_type); // set to the target type
				TypeChanged(0); // handle the change of type - this set the permitted indices
				//
			};
		};
		break;
		case tConfigLoad:
		{
			#ifdef DEPRECATED_IEC104_CONFIG
			if(Name->text() == GetConfigureDb()->GetString("IKEY")) // may be several queued up
			#else
			if(Name->text() == GetConfigureDb()->GetString("NAME")) // may be several queued up
			#endif
			{
				emit LoadConfig (Name->text()); // ask it to load - we pass in the obejct name
				// it a given that the current configuration record is the property record
				// pass even if nothing as the dialog shoudl provide defaults when no data is found
			};
		};
		break;
		case tTagList:
		{
			// got the current tag list - look up what it should be 
			int n = GetConfigureDb()->GetNumberResults();
			QStringList list;
			if(pCfgDriver)
			{
				pCfgDriver->GetTagList(Type->currentText(),list,Unit->currentText(),Name->text());
				QStringList currentList; // current list of tags
				//
				if(n)
				{
					for(int j = 0; j < n; j++,GetConfigureDb()->FetchNext())
					{
						currentList << GetConfigureDb()->GetString("TAG");
					};
					pCfgDriver->CheckTags(Name->text(), currentList, list);
				}
				else
				{
					//we are creating a new sample point
					pCfgDriver->CreateSamplePoint(Name->text(), list, " ");
				}
			};
			ButtonState (true);
		};
		break;
		case tIpIndex:
		{
			//
			// Edit the list - remove those in use
			int n = GetConfigureDb ()->GetNumberResults();
			InputIndex->clear();
			if(n > 0)
			{
				for(int i= 0; i < n; i++,GetConfigureDb()->FetchNext())
				{
					int id = CurrentTypeList.findIndex(GetConfigureDb()->GetString("IPINDEX"));           
					if(id != -1)
					{
						CurrentTypeList.remove(CurrentTypeList.at(id)); // delete it 
					};
				};
			};
			//
			for(unsigned i = 0; i < CurrentTypeList.count(); i++)
			{
				InputIndex->insertItem(CurrentTypeList[i]);
			};
			//
			InputIndex->insertItem(tmp_ip_index);
			SetComboItem(InputIndex,tmp_ip_index);  
			//
		};
		break;
		case tNew:
		ButtonState (true);
		default:
		break;
	};
	//cerr << "Exit QueryResponse " << endl;
};
void SampleCfg::Changed(QListViewItem *)
{
	IT_IT("SampleCfg::Changed");

	SelChanged(0);
};

void SampleCfg::ListClicked(int, QListViewItem*, const QPoint &, int)
{
	IT_IT("SampleCfg::ListClicked");

	SelChanged(0);
};

