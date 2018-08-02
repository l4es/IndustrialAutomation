/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
/*
*Header For: Simulator module
*Purpose: the simulator module is used for testing 
*it generates random data 
*/
#include <qt.h>
#include "simulator.h"
#include "IndentedTrace.h"

void SimulatorInstance::Tick() // one second timer tick
{
	IT_IT("SimulatorInstance::Tick");
	
	if(fFail) return; // failed so stop now 

	SPdict::iterator i = Sps.begin();

	for(;!(i == Sps.end()); i++)
	{
		// countdown all sample points
		SimulatorSP &s = (*i).second;

		if(s.interval > 0)
		{
			IT_COMMENT2("Looking at %s Countdown %d", (const char *)(*i).first, s.countdown);

			--s.countdown;

			if(s.countdown < 1)
			{
				//s.countdown = s.interval; //APA commented
				s.countdown = 16; //APA replace previous line!
				// when count == 0 reload and generate value 
				// post this to results database

				if(fTrace)
				{
					Trace(tr("Measuring ") + (*i).first);
				}

				double v = 0;

				if(s.sd > 0)
				{ 
					// we want +/- sd variation
					v = s.sd - ((double)rand() / (double)RAND_MAX * (2 * s.sd));
				}

				v = v + s.mean;

				switch(s.type)
				{
					case 2:
					{
						int digit = (int)v;
						digit = digit%2;
						//
						//  digital value 
						//
						IT_COMMENT2("Post Digital Value  %s %lf", (const char *)(*i).first, digit);

						PostValue((*i).first,VALUE_TAG, digit); // post the value up  
					}
					break;
					case 1:
					{
						//
						//  analog value 
						//
						IT_COMMENT2("Post Analog Value  %s %lf", (const char *)(*i).first, v);

						PostValue((*i).first,VALUE_TAG,v); // post the value up  
					}
					break;
					case 0:
					{
						//
						//counts - generate 4 tags
						//
						IECValueList list;
						//
						IECValue v0("t03",&v);
						double f = v/2.0;
						IECValue v1("t05",&f);
						f = v/4.0;
						IECValue v2("t10",&f);
						f = v/16.0;
						IECValue v3("t50",&f);
						//
						list.insert(list.end(),v0);
						list.insert(list.end(),v1);
						list.insert(list.end(),v2);
						list.insert(list.end(),v3); 
						
						IT_COMMENT2("Post Counts Value   %s %lf", (const char *)(*i).first, v);
						//
						PostList((*i).first,list); // post the lis tof values up
						//
					};
					break;
					default:
					break;
				};
				// test against alarm limits for the current receipe
				// update current values database
				// log any alarm
				//
			};
		};
	}; 
};
/*
*Function: Start
*Inputs:none
*Outputs:none
*Returns:none
*/
void SimulatorInstance::Start()
{
	
	IT_IT("SimulatorInstance::Start");
	
	// get the list of enabled sample points owed by this instance of this driver
	// Get the unit specific configuration - The simulator has no unit level configuration
	// Resolve receipe selection for unit configuration
	//
	if(GetReceipeName() == "(default)")
	{
		QString cmd = "select * from SAMPLE where UNIT='" + Name + "' and ENABLED=1;"; 
		GetConfigureDb()->DoExec(this,cmd,tList);
	}
	else
	{
		QString cmd = 
		"select * from SAMPLE where UNIT='" + Name + 
		"' and NAME in (" + DriverInstance::FormSamplePointList() + ");"; 
		// only the enable sample points
		GetConfigureDb()->DoExec(this,cmd,tList);
	};
	//
	//
	//  
	QTimer *pTimer = new QTimer(this);
	pTimer->connect(pTimer,SIGNAL(timeout()),this,SLOT(Tick()));
	pTimer->start(1000); // start with a 1 second timer
	//
	//
	if(fTrace)
	{
		Trace(tr("Started"));
	}
	//
};  // start 
/*
*Function:Stop
*Inputs:none
*Outputs:none
*Returns:none
*/
void SimulatorInstance::Stop()
{
	IT_IT("SimulatorInstance::Stop");
	
	if(fTrace)
	{
		Trace(tr("Stopped"));
	}
}; 
/*
*Function:Command
*Inputs:unit id, command string
*Outputs:none
*Returns:none
*/
void SimulatorInstance::Command(const QString & name, BYTE cmd, LPVOID lpPa, DWORD pa_length, DWORD ipindex)
{
	IT_IT("SimulatorInstance::Command");
}; // process a command for a named unit 
/*
*Function:QueryResponse
*Inputs:client object, command, transaction id
*Outputs:none
*Returns:none
*/
void SimulatorInstance::QueryResponse (QObject *p, const QString &c, int id, QObject*caller) // handles database responses
{
	if(p != this) return;

	IT_IT("SimulatorInstance::QueryResponse");

	switch(id)
	{
		case tList: // list of sample points controlled by the unit
		{
			// copy the list to local dictionary 
			// now get the properties for each of the sample points
			// 
			int n = GetConfigureDb()->GetNumberResults();
			for(int i = 0; i < n; i++,GetConfigureDb()->FetchNext())
			{
				//
				// this is the list of enabled sample points for this device
				// For this implimentation we only need the name and type. 
				// Get the properties which define the specific config 
				// 
				// Put into a dictionary
				//
				QString name = GetConfigureDb()->GetString("NAME");
				SimulatorSP sp; // the sample point
				if(GetConfigureDb()->GetString("QTYPE") == TYPE_M_ME_TF_1)
				{
					sp.type = 1;
				}
				else if(GetConfigureDb()->GetString("QTYPE") == TYPE_M_SP_TB_1)
				{
					sp.type = 2;
				}
				else if(GetConfigureDb()->GetString("QTYPE") == TYPE_M_IT_NA_1)
				{
					sp.type = 0;
				}
				
				//
				SPdict::value_type pr(name,sp); 
				Sps.insert(pr);
			};
			// 
			//  extract the properties for a single unit in one go, probably
			//   
			
			QString cmd = "select IKEY,DVAL from PROPS where SKEY='SAMPLEPROPS' and IKEY in (select NAME from SAMPLE where UNIT='"+Name+"');";
			GetConfigureDb()->DoExec(this,cmd,tProperties);
			// 
		};
		break;
		case tProperties:
		{
			//
			// got the properties - here this defines the sample interval , mean and sd
			// find dictionary entry 
			if(GetConfigureDb()->Ok())
			{
				int n = GetConfigureDb()->GetNumberResults();
				for(int j = 0; j < n; j++,GetConfigureDb()->FetchNext()) // may get a boat load of properties back
				{    
					SPdict::iterator i = Sps.find(GetConfigureDb()->GetString("IKEY"));
					if(!(i == Sps.end()))
					{
						QString s = UndoEscapeSQLText(GetConfigureDb()->GetString("DVAL"));
						QTextIStream is(&s);
						QString a;  
						is >> a; // interval
						(*i).second.interval = QStringToInterval(a);
						(*i).second.countdown = (*i).second.interval;
						//
						is >> a;
						(*i).second.mean = a.toDouble();
						//
						is >> a;
						(*i).second.sd = a.toDouble();
					};
				};
			};
		};
		break;
		default:
		break;
	};
};
/*
*Function: Simulator
*Inputs:parent object, object (unit) name
*Outputs:none
*Returns:none
*/
Simulator::Simulator(QObject *parent,const QString &name) : Driver(parent,name) 
{
	IT_IT("Simulator::Simulator");
	
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
};
/*
*Function:~Simulator
*Inputs:none
*Outputs:none
*Returns:none
*/
Simulator::~Simulator()
{
	IT_IT("Simulator::~Simulator");

	pDriver = 0;
};
/*
*Function: UnitConfigure
*Inputs:parent widget, unit name
*Outputs:none
*Returns:none
*/
void Simulator::UnitConfigure(QWidget *parent, const QString &name, const QString &receipe) // configure a unit
{
	IT_IT("Simulator::UnitConfigure");
	
	SimulatorUnit dlg(parent,name);
	dlg.exec();
};
/*
*Function: SetTypeList - sets the permitted types supported by this unit
*Inputs:combo box , unit name
*Outputs:none
*Returns:none
*/
void Simulator::SetTypeList(QComboBox *pCombo, const QString &unitname) // set the type list for unit type
{
	IT_IT("Simulator::SetTypeList");
	
	pCombo->insertItem(TYPE_M_SP_NA_1);
	pCombo->insertItem(TYPE_M_DP_NA_1);
	pCombo->insertItem(TYPE_M_ME_NA_1);
	pCombo->insertItem(TYPE_M_ME_NB_1);
	pCombo->insertItem(TYPE_M_ME_NC_1);
	pCombo->insertItem(TYPE_M_IT_NA_1);
	pCombo->insertItem(TYPE_M_SP_TB_1);
	pCombo->insertItem(TYPE_M_DP_TB_1);
	pCombo->insertItem(TYPE_M_ME_TD_1);
	pCombo->insertItem(TYPE_M_ME_TE_1);
	pCombo->insertItem(TYPE_M_ME_TF_1);
	pCombo->insertItem(TYPE_M_IT_TB_1);
};
/*
*Function: SetInputList
*Inputs:combobox,unit name, sample point
*Outputs:none
*Returns:none
*/
void Simulator::GetInputList(const QString &type, QStringList &list,const QString &, const QString &)
{ 
	IT_IT("Simulator::GetInputList");
	
	// set the list of available input indices

	list.clear();

	if(type == TYPE_M_IT_NA_1)
	{
		list << "01" << "02" << "03" << "04" << "05" << "06" << "07" << "08" 
		<< "09" << "10" << "11" << "12" << "13" << "14" << "15" << "16";         
	}
	else if(type == TYPE_M_ME_TF_1)
	{
		list << "01" << "02" << "03" << "04" << "05" << "06" << "07" << "08";         
	}
	else if(type == TYPE_M_SP_TB_1)
	{
		list << "01" << "02" << "03" << "04" << "05" << "06" << "07" << "08";         
	}
}; 


void Simulator::GetTagList(const QString &type, QStringList &list,const QString &,const QString &) // returns the permitted tags for a given type for this unit
{
	IT_IT("Simulator::GetTagList");
	
	list.clear();
	if(type == TYPE_M_IT_NA_1)
	{
		list << "t03" << "t05" << "t10" << "t50";
	}
	else if(type == TYPE_M_ME_TF_1 || type == TYPE_M_ME_NB_1)
	{
		list << VALUE_TAG;
	}
	else if(type == TYPE_M_SP_TB_1)
	{
		list << VALUE_TAG;
	}
};
/*
*Function: GetSpecificConfig
*Inputs:sample point name, sample point type
*Outputs:none
*Returns:widget to handle specific input
*/
QWidget * Simulator::GetSpecificConfig(QWidget *parent, const QString &spname, const QString &sptype) 
{
	IT_IT("Simulator::GetSpecificConfig");
	
	if(sptype == TYPE_M_IT_NA_1)
	{
		return new SimulatorCounts(parent,spname);
	}
	else if(sptype == TYPE_M_ME_TF_1)
	{
		return new SimulatorAnalog(parent,spname);
	}
	else if(sptype == TYPE_M_SP_TB_1)
	{
		return new SimulatorDigital(parent,spname);
	}

	return 0;
}; //specific config for sample point of type
//
// Actual driver stuff
// 
/*
*Function: Start
*Inputs:none
*Outputs:none
*Returns:none
*/
void Simulator::Start() // start everything under this driver's control
{
	IT_IT("Simulator::Start");
	
	//
	// create instances for each simulator unit - get the receipe qualifier
	// request each to start
	// form the database request transaction
	// post it to the database - the magic is in the response handler
	// get all enabled units
	// 
	// form up the list of enabled units
	//
	QString cmd = "select * from UNITS where UNITTYPE='Simulator' and NAME in(" + DriverInstance::FormUnitList()+ ");";
	GetConfigureDb()->DoExec(this,cmd,tListUnits);
	//
	//
};
/*
*Function: Stop
*Inputs:none
*Outputs:none
*Returns:none
*/
void Simulator::Stop() // stop everything under this driver's control
{
	IT_IT("Simulator::Stop");
	
	// ask each instance to stop 
	IDict::iterator i = Instances.begin();
	for(; !(i == Instances.end());i++)
	{
		(*i).second->Stop();
	};
	//
	// now delete them
	//     
	i = Instances.begin();
	for(; !(i == Instances.end());i++)
	{
		delete (*i).second;
	};
	//  
	Instances.clear();
};
/*
*Function: Command
*Inputs:target unit, command
*Outputs:none
*Returns:none
*/
void Simulator::Command(const QString & unit, BYTE cmd, LPVOID lpPa, DWORD pa_lenght, DWORD ipindex) // process a command for a named unit 
{
	IT_IT("Simulator::Command");
	
	IDict::iterator i = Instances.find(unit);
	if(!(i == Instances.end()))
	{
		(*i).second->Command(unit, cmd, lpPa, pa_lenght, ipindex); // pass on the command
	};
};
/*
*Function: QueryResponse
*this is the callback from the database interface
*Inputs:Object owning response, SQL command string, transaction id
*Outputs:none
*Returns:none
*/
void Simulator::QueryResponse (QObject *p, const QString &c, int id, QObject*caller)
{
	if(p != this) return;

	IT_IT("Simulator::QueryResponse");

	switch(id)
	{
		case tListUnits:
		{
			int n = GetConfigureDb()->GetNumberResults();
			if(n > 0)
			{
				for(int i = 0; i < n; i++,GetConfigureDb()->FetchNext())
				{
					SimulatorInstance *p = new SimulatorInstance(this,GetConfigureDb()->GetString("NAME"));
					IDict::value_type pr(GetConfigureDb()->GetString("NAME"),p);
					Instances.insert(pr);
					p->Start(); // kick it off 
				};
			};
		};
		break;
		default:
		break;
	};
};
/*
*Function:CreateNewUnit
*Inputs:none
*Outputs:none
*Returns:none
*/
void Simulator::CreateNewUnit(QWidget *parent, const QString &name, int n_inputs) // create a new unit - quick configure
{
	IT_IT("Simulator::CreateNewUnit");
	
	//
	// generate the sample points for the new unit 
	// 
	// generate all the inputs possible 
	// 
	for(int i = 1 ; i <= n_inputs; i++)
	{
		QString n;
		n.sprintf("%02d",i);
		QString spname = name+"Counts"+n;
		QString cmd = 
		"insert into SAMPLE values('"        +spname+
		"','Simulated Counts Input Number "  + n + 
		"','"+name+"','"TYPE_M_IT_NA_1"','N/cuft',1,1,'"+n+"',1,0,0);";
		GetConfigureDb()->DoExec(0,cmd,0); // post it off
		//
		QStringList l;
		GetTagList(TYPE_M_IT_NA_1,l,"",""); 
		CreateSamplePoint(spname, l,"00:01:00 1000 100");
	}

	for(i = 1 ; i <= n_inputs/2; i++)
	{
		QString n;
		n.sprintf("%02d",i);
		//
		QString spname = name+"Adc"+n;
		QString cmd = "insert into SAMPLE values('"+spname+"','Simulated Analogue Input Number "+ n + 
		"','"+name+"','"TYPE_M_ME_TF_1"','V',1,1,'"+n+"',0,0,0);" ;
		GetConfigureDb()->DoExec(0,cmd,0); // post it off
		//
		QStringList l;
		GetTagList(TYPE_M_ME_TF_1,l,"",""); 
		CreateSamplePoint(spname, l,"00:01:00 100 20");
	}

	for(i = 1 ; i <= n_inputs/2; i++)
	{
		QString n;
		n.sprintf("%02d",i);
		//
		QString spname = name+"Dig"+n;
		QString cmd = "insert into SAMPLE values('"+spname+"','Simulated Digital Input Number "+ n + 
		"','"+name+"','"TYPE_M_SP_TB_1"','spi',1,1,'"+n+"',0,0,0);" ;
		GetConfigureDb()->DoExec(0,cmd,0); // post it off
		//
		QStringList l;
		GetTagList(TYPE_M_SP_TB_1,l,"",""); 
		CreateSamplePoint(spname, l,"00:01:00 100 20");
	}
};
/*
*Function:GetDriverEntry
*Inputs:parent object
*Outputs:none
*Returns:driver interface 
*/
Simulator * Simulator::pDriver = 0;
extern "C"
{ 
	#ifdef WIN32
	SIMULATOR_API Driver *  _cdecl _GetDriverEntry(QObject *parent); 
	SIMULATOR_API void _cdecl _Unload();
	#endif

	Driver * _cdecl _GetDriverEntry(QObject *parent) 
	{
		IT_IT("_GetDriverEntry - Simulator");
		
		if(!Simulator::pDriver )
		{
			Simulator::pDriver = new Simulator(parent,"Simulator");
		};
		return Simulator::pDriver;
	};
	/*
	*Function: _Unload
	*clean up before DLL unload. and QObjects must be deleted or we get a prang
	*Inputs:none
	*Outputs:none
	*Returns:none
	*/
	void _cdecl _Unload()
	{
		IT_IT("_Unload  - Simulator");
		
		if(Simulator::pDriver) delete Simulator::pDriver;
		Simulator::pDriver = 0;
	};
};
