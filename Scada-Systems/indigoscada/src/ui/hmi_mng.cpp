/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2014 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
#include <qt.h>
#include "hmi_mng.h"
#include "qwt_thermo.h"
#include "single_point_led.h"
#include "double_point_led.h"
#include <qlcdnumber.h>
#include "realtimedb.h"
#include "dispatch.h"
#include "helper_functions.h"
#include "pswitch.h"
#include "ptank.h"
#include "pthermometer.h"
#include "inspect.h"
#include "plcdnumber.h"
#include "psinglepointled.h"
#include "pdoublepointled.h"
#include "elswitch.h"

void HMI_manager::setParent( QDialog *parent )
{
    dialog_parent = parent;

	connect(qApp->mainWidget(),SIGNAL(UpdateTags()),this,SLOT(UpdateTags()));
	connect(qApp->mainWidget(),SIGNAL(UpdateSamplePoint()),this,SLOT(UpdateSamplePoint()));

	// connect to the database

	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));

	connect (GetDispatcher (),
	SIGNAL (ReceivedNotify(int, const char *)), this,
	SLOT (ReceivedNotify(int, const char *)));

	setInitialValuesAndLimits();
}

//	QColor white(0xff, 0xff, 0xff);
//	QColor red(0xff, 0x00, 0x00);
//	QColor yellow(0xff, 0xff, 0x00);
//	QColor green(0x00, 0xff, 0x00);
//	QColor blue(0x00, 0x00, 0xff);

void HMI_manager::setInitialValuesAndLimits() 
{
	{
		QObjectList *l = dialog_parent->queryList( "PSwitch" );

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			QString name = obj->name();
			
			((PSwitch*)obj)->setPSwitchValue(false);

			int idx = name.find('_');
			name.truncate(idx);

			WidgetDict::value_type pr(name, obj);
			PSwitch_dictionary.insert(pr); // put in the dictionary
		}

		delete l; // delete the list, not the objects
	}

	{
		QObjectList *l = dialog_parent->queryList( "Breaker" );

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			QString name = obj->name();
			
			((Breaker*)obj)->setBreakerValue(false);

			int idx = name.find('_');
			name.truncate(idx);

			WidgetDict::value_type pr(name, obj);
			Breaker_dictionary.insert(pr); // put in the dictionary
		}

		delete l; // delete the list, not the objects
	}

	{
		QObjectList *l = dialog_parent->queryList( "QwtThermo" );

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			QString name = obj->name();

			int idx = name.find('_');
			name.truncate(idx);

			WidgetDict::value_type pr(name, obj);
			QwtThermo_dictionary.insert(pr); // put in the dictionary
			
			QString widget_type = "QwtThermo";

			// get the alarm limits
			QString cmd = "select * from TAGS where NAME='"+name+"' and RECEIPE='"+GetReceipeName()+"';";
			GetConfigureDb()->DoExec(this, cmd, tTagLimits, widget_type, name);

			/*
			double val = 0.0;

			((QwtThermo*)obj)->setMinValue(-500.0);

			((QwtThermo*)obj)->setMaxValue(1000.0);

			((QwtThermo*)obj)->setValue(val);
			*/
		}

		delete l; // delete the list, not the objects
	}

	{
		QObjectList *l = dialog_parent->queryList( "QLCDNumber" );

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			QString name = obj->name();

			double val = 0.0;

			((QLCDNumber*)obj)->display(val);

			int idx = name.find('_');
			name.truncate(idx);

			WidgetDict::value_type pr(name, obj);
			QLCDNumber_dictionary.insert(pr); // put in the dictionary
		}

		delete l; // delete the list, not the objects
	}

	{
		QObjectList *l = dialog_parent->queryList( "PLCDNumber" );

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			QString name = obj->name();

			double val = 0.0;

			((PLCDNumber*)obj)->display(val);

			int idx = name.find('_');
			name.truncate(idx);

			WidgetDict::value_type pr(name, obj);
			PLCDNumber_dictionary.insert(pr); // put in the dictionary
		}

		delete l; // delete the list, not the objects
	}

	{
		QObjectList *l = dialog_parent->queryList( "SinglePointLed" );

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			QString name = obj->name();

			((SinglePointLed*)obj)->setColor(Qt::white);
			((SinglePointLed*)obj)->on();

			int idx = name.find('_');
			name.truncate(idx);

			WidgetDict::value_type pr(name, obj);
			SinglePointLed_dictionary.insert(pr); // put in the dictionary
		}

		delete l; // delete the list, not the objects
	}

    {
		QObjectList *l = dialog_parent->queryList( "DoublePointLed" );

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			QString name = obj->name();

			((DoublePointLed*)obj)->setColor(Qt::white);
			((DoublePointLed*)obj)->on();

			int idx = name.find('_');
			name.truncate(idx);

			WidgetDict::value_type pr(name, obj);
			DoublePointLed_dictionary.insert(pr); // put in the dictionary
		}

		delete l; // delete the list, not the objects
	}

	{
		QObjectList *l = dialog_parent->queryList( "QPushButton" );

		QObjectListIt it( *l ); // iterate over the buttons

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			QString name = obj->name();

			//By default commands are not enabled
			((QPushButton*)obj)->setEnabled( FALSE );

			int idx = name.find('_');
			name.truncate(idx);

			WidgetDict::value_type pr(name, obj);
			QPushButton_dictionary.insert(pr); // put in the dictionary
		}

		delete l; // delete the list, not the objects
	}

	{
		QObjectList *l = dialog_parent->queryList( "PTank" );

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			QString name = obj->name();

			int idx = name.find('_');
			name.truncate(idx);

			WidgetDict::value_type pr(name, obj);
			PTank_dictionary.insert(pr); // put in the dictionary

			QString widget_type = "PTank";

			// get the alarm limits
			QString cmd = "select * from TAGS where NAME='"+name+"' and RECEIPE='"+GetReceipeName()+"';";
			GetConfigureDb()->DoExec(this, cmd, tTagLimits, widget_type, name);

			/*
			((PTank*)obj)->setMinValue(-500.0);
			((PTank*)obj)->setMaxValue(1000.0);
			((PTank*)obj)->setValue(0);
			*/
		}

		delete l; // delete the list, not the objects
	}

	{
		QObjectList *l = dialog_parent->queryList( "PThermometer" );

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			QString name = obj->name();

			int idx = name.find('_');
			name.truncate(idx);

			WidgetDict::value_type pr(name, obj);
			PThermometer_dictionary.insert(pr); // put in the dictionary

			QString widget_type = "PThermometer";

			// get the alarm limits
			QString cmd = "select * from TAGS where NAME='"+name+"' and RECEIPE='"+GetReceipeName()+"';";
			GetConfigureDb()->DoExec(this, cmd, tTagLimits, widget_type, name);

			/*
			((PThermometer*)obj)->setMinValue(-500.0);
			((PThermometer*)obj)->setMaxValue(1000.0);
			((PThermometer*)obj)->setValue(0);
			*/
		}

		delete l; // delete the list, not the objects
	}

	{
		QObjectList *l = dialog_parent->queryList( "PSinglePointLed" );

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			QString name = obj->name();

			((PSinglePointLed*)obj)->on();
			((PSinglePointLed*)obj)->setOnColor(white);

			int idx = name.find('_');
			name.truncate(idx);

			WidgetDict::value_type pr(name, obj);
			PSinglePointLed_dictionary.insert(pr); // put in the dictionary
		}

		delete l; // delete the list, not the objects
	}

	{
		QObjectList *l = dialog_parent->queryList( "PDoublePointLed" );

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			QString name = obj->name();

			((PDoublePointLed*)obj)->on();
			((PDoublePointLed*)obj)->setOnColor(white);

			int idx = name.find('_');
			name.truncate(idx);

			WidgetDict::value_type pr(name, obj);
			PDoublePointLed_dictionary.insert(pr); // put in the dictionary
		}

		delete l; // delete the list, not the objects
	}
}

void HMI_manager::doUpdateTags(QString &s, double &v, WidgetDict &dict)
{
	WidgetDict::iterator j =  dict.find(s);

	if(!(j == dict.end()))
	{
		QObject *obj = (*j).second;

		if(obj->isA( "QwtThermo" ))
		{
			((QwtThermo*)obj)->setValue(v);
		}
		else if(obj->isA( "QLCDNumber" ))
		{
			((QLCDNumber*)obj)->display(v);
		}
		else if(obj->isA( "PLCDNumber" ))
		{
			((PLCDNumber*)obj)->display(v);
		}
		else if(obj->isA( "SinglePointLed" ))
		{
			int i = (int)v;

			switch(i)
			{
				case 0:
				{
					//Green means state off
					((SinglePointLed*)obj)->setColor(Qt::green);
					((SinglePointLed*)obj)->on();
				}
				break;
				case 1:
				{
					//	Red means state on
					((SinglePointLed*)obj)->setColor(Qt::red);
					((SinglePointLed*)obj)->on();
				}
				break;
				case 2:
				{
					//Yellow is not used for Single point
					((SinglePointLed*)obj)->setColor(Qt::yellow);
					((SinglePointLed*)obj)->on();
				}
				break;
				case 3:
				{
					//Yellow is not used for Single point
					((SinglePointLed*)obj)->setColor(Qt::yellow);
					((SinglePointLed*)obj)->on();
				}
				break;
				default:
					//White means HMI state none or Invalid
					((SinglePointLed*)obj)->setColor(Qt::white);
					((SinglePointLed*)obj)->on();
				break;
			}
		}
		else if(obj->isA( "DoublePointLed" ))
		{
			int i = (int)v;

			switch(i)
			{
				case 0:
				{
					//Yellow means Indeterminate or Intermediate state
					((DoublePointLed*)obj)->setColor(Qt::yellow);
					((DoublePointLed*)obj)->on();
				}
				break;
				case 1:
				{
					//Green means Determinate state off
					((DoublePointLed*)obj)->setColor(Qt::green);
					((DoublePointLed*)obj)->on();
				}
				break;
				case 2:
				{
					//Red means Determinate state on
					((DoublePointLed*)obj)->setColor(Qt::red);
					((DoublePointLed*)obj)->on();
				}
				break;
				case 3:
				{
					//Yellow means Indeterminate state
					((DoublePointLed*)obj)->setColor(Qt::yellow);
					((DoublePointLed*)obj)->on();
				}
				break;
				default:
					//White means HMI state none or Invalid
					((DoublePointLed*)obj)->setColor(Qt::white);
					((DoublePointLed*)obj)->on();
				break;
			}
		}
		else if(obj->isA( "PSinglePointLed" ))
		{
			int i = (int)v;

			switch(i)
			{
				case 0:
				{
					//Green means state off
					((PSinglePointLed*)obj)->on();
					((PSinglePointLed*)obj)->setOnColor(green);
				}
				break;
				case 1:
				{
					//	Red means state on
					((PSinglePointLed*)obj)->on();
					((PSinglePointLed*)obj)->setOnColor(red);
				}
				break;
				case 2:
				{
					//Yellow is not used for Single point
					((PSinglePointLed*)obj)->on();
					((PSinglePointLed*)obj)->setOnColor(yellow);
				}
				break;
				case 3:
				{
					//Yellow is not used for Single point
					((PSinglePointLed*)obj)->on();
					((PSinglePointLed*)obj)->setOnColor(yellow);
				}
				break;
				default:
					//White means HMI state none or Invalid
					((PSinglePointLed*)obj)->on();
					((PSinglePointLed*)obj)->setOnColor(white);
				break;
			}
		}
		else if(obj->isA( "PDoublePointLed" ))
		{
			int i = (int)v;

			switch(i)
			{
				case 0:
				{
					//Yellow means Indeterminate or Intermediate state
					((PDoublePointLed*)obj)->on();
					((PDoublePointLed*)obj)->setOnColor(yellow);
				}
				break;
				case 1:
				{
					//Green means Determinate state off

					((PDoublePointLed*)obj)->on();
					((PDoublePointLed*)obj)->setOnColor(green);
				}
				break;
				case 2:
				{
					//Red means Determinate state on
					((PDoublePointLed*)obj)->on();
					((PDoublePointLed*)obj)->setOnColor(red);
				}
				break;
				case 3:
				{
					//Yellow means Indeterminate state								
					((PDoublePointLed*)obj)->on();
					((PDoublePointLed*)obj)->setOnColor(yellow);
				}
				break;
				default:
					//White means HMI state none or Invalid
					((PDoublePointLed*)obj)->on();
					((PDoublePointLed*)obj)->setOnColor(white);
				break;
			}
		}
		else if(obj->isA( "PSwitch" ))
		{
			int i = (int)v;

			switch(i)
			{
				case 0:
				{
					//Green means state off
					((PSwitch*)obj)->setPSwitchValue(false);
				}
				break;
				case 1:
				{
					//	Red means state on
					((PSwitch*)obj)->setPSwitchValue(true);
				}
				break;
				default:
					//White means HMI state none or Invalid
					((PSwitch*)obj)->setPSwitchValueInvalid(true);
				break;
			}
		}
		else if(obj->isA( "Breaker" ))
		{
			int i = (int)v;

			switch(i)
			{
				case 0:
				{
					//Green means state off
					((Breaker*)obj)->setBreakerValue(false);
				}
				break;
				case 1:
				{
					//	Red means state on
					((Breaker*)obj)->setBreakerValue(true);
				}
				break;
				default:
					//White means HMI state none or Invalid
					((Breaker*)obj)->setBreakerValueInvalid(true);
				break;
			}
		}
		else if(obj->isA( "PTank" ))
		{
			((PTank*)obj)->setValue(v);
		}
		else if(obj->isA( "PThermometer" ))
		{
			((PThermometer*)obj)->setValue(v);
		}
		else
		{
			//error
		}
	}
}

void HMI_manager::UpdateTags()
{
	//IT_IT("HMI_manager::UpdateTags");

	//Here we have set of records from TAGS_DB table
	//
	int n = GetCurrentDb()->GetNumberResults();

	GetCurrentDb()->GotoBegin();
	//
	for(int i = 0; i < n ; i++,GetCurrentDb()->FetchNext())
	{
		QString s = GetCurrentDb()->GetString("NAME");
		double v = atof((const char*)(GetCurrentDb()->GetString("VAL")));

		doUpdateTags(s, v, QwtThermo_dictionary);
		doUpdateTags(s, v, QLCDNumber_dictionary);
		doUpdateTags(s, v, PLCDNumber_dictionary);
		doUpdateTags(s, v, SinglePointLed_dictionary);
		doUpdateTags(s, v, DoublePointLed_dictionary);
		doUpdateTags(s, v, PSinglePointLed_dictionary);
		doUpdateTags(s, v, PDoublePointLed_dictionary);
		doUpdateTags(s, v, PSwitch_dictionary);
		doUpdateTags(s, v, Breaker_dictionary);
		doUpdateTags(s, v, PTank_dictionary);
		doUpdateTags(s, v, PThermometer_dictionary);
	}
};

void HMI_manager::doUpdateSamplePoint(QString &s, int state, int ack_flag, WidgetDict &dict)
{
	WidgetDict::iterator j =  dict.find(s);

	if(!(j == dict.end()))
	{
		QObject *obj = (*j).second;

		if(obj->isA( "SinglePointLed" ))
		{
			if(ack_flag)
			{
				((SinglePointLed*)obj)->startFlash();
			}
			else
			{
				((SinglePointLed*)obj)->stopFlash();
			}

			if(state == NoLevel)
			{
				//White means HMI state none or NO or Invalid
				((SinglePointLed*)obj)->setColor(Qt::white);
				((SinglePointLed*)obj)->on(); 
			}

			if(state == FailureLevel)
			{ //Blue means Communication driver error state or Invalid
					((SinglePointLed*)obj)->setColor(Qt::blue);
					((SinglePointLed*)obj)->on(); 
			}
		}
		else if(obj->isA( "DoublePointLed" ))
		{
			if(ack_flag)
			{
				((DoublePointLed*)obj)->startFlash();
			}
			else
			{
				((DoublePointLed*)obj)->stopFlash();
			}

			if(state == NoLevel)
			{
				//White means HMI state none or NO or Invalid
				((DoublePointLed*)obj)->setColor(Qt::white);
				((DoublePointLed*)obj)->on(); 
			}

			if(state == FailureLevel)
			{	    //Blue means Communication driver error state or Invalid
					((DoublePointLed*)obj)->setColor(Qt::blue);
					((DoublePointLed*)obj)->on(); 
			}
		}
		else if(obj->isA( "PSinglePointLed" ))
		{
			if(ack_flag)
			{
				((PSinglePointLed*)obj)->startFlash();
			}
			else
			{
				((PSinglePointLed*)obj)->stopFlash();
			}

			if(state == NoLevel)
			{
				//White means HMI state none or NO or Invalid
				((PSinglePointLed*)obj)->on();
				((PSinglePointLed*)obj)->setOnColor(white);
			}

			if(state == FailureLevel)
			{   //Blue means Communication driver error state or Invalid
				((PSinglePointLed*)obj)->on();
				((PSinglePointLed*)obj)->setOnColor(blue);
			}
		}
		else if(obj->isA( "PDoublePointLed" ))
		{
			if(ack_flag)
			{
				((PDoublePointLed*)obj)->startFlash();
			}
			else
			{
				((PDoublePointLed*)obj)->stopFlash();
			}

			if(state == NoLevel)
			{
				//White means HMI state none or NO or Invalid
				((PDoublePointLed*)obj)->on();
				((PDoublePointLed*)obj)->setOnColor(white);
			}

			if(state == FailureLevel)
			{   //Blue means Communication driver error state or Invalid
				((PDoublePointLed*)obj)->on();
				((PDoublePointLed*)obj)->setOnColor(blue);
			}
		}
		else if(obj->isA( "PSwitch" ))
		{
			/*
			if(ack_flag)
			{
				((PSwitch*)obj)->startFlash();
			}
			else
			{
				((PSwitch*)obj)->stopFlash();
			}
			*/

			if(state == NoLevel)
			{
				//White means HMI state none or NO or Invalid
				((PSwitch*)obj)->setPSwitchValueInvalid(false);
			}

			if(state == FailureLevel)
			{   //Blue means Communication driver error state or Invalid
				((PSwitch*)obj)->setPSwitchValueInvalid(false);
			}
		}
		else if(obj->isA( "Breaker" ))
		{
			if(state == NoLevel)
			{
				//White means HMI state none or NO or Invalid
				((Breaker*)obj)->setBreakerValueInvalid(false);
			}

			if(state == FailureLevel)
			{   //Blue means Communication driver error state or Invalid
				((Breaker*)obj)->setBreakerValueInvalid(false);
			}
		}
		else
		{
			//error
		}
	}
}



/*
*Function: Query Response - handle transactions with the current values database
*Inputs:
*Outputs:none
*Returns:none
*/
void HMI_manager::UpdateSamplePoint() // handle updated sample points
{
//	IT_IT("HMI_manager::UpdateSamplePoint");

	//Here we have a set of records from CVAL_DB table
	
	int n = GetCurrentDb()->GetNumberResults();

	GetCurrentDb()->GotoBegin();
	//
	for(int i = 0; i < n ; i++,GetCurrentDb()->FetchNext())
	{
		QString s = GetCurrentDb()->GetString("NAME");

		int state = GetCurrentDb()->GetInt("STATE");

		int ack_flag = GetCurrentDb()->GetInt("ACKFLAG");

		doUpdateSamplePoint(s, state, ack_flag, SinglePointLed_dictionary);

		doUpdateSamplePoint(s, state, ack_flag, DoublePointLed_dictionary);
		
		doUpdateSamplePoint(s, state, ack_flag, PSinglePointLed_dictionary);
			
		doUpdateSamplePoint(s, state, ack_flag, PDoublePointLed_dictionary);

		doUpdateSamplePoint(s, state, ack_flag, PSwitch_dictionary);

		doUpdateSamplePoint(s, state, ack_flag, Breaker_dictionary);
	}
};


void HMI_manager::sendCommand() 
{
	QObjectList *l = dialog_parent->queryList("QPushButton");

	QObjectListIt it( *l ); // iterate over the buttons

	QObject *obj;

	while((obj = it.current()) != 0) 
	{
		// for each found object...
		++it;

		if(((QPushButton*)obj)->isDown())
		{
			Sleep(100);
			QString name = obj->name();

			int idx = name.find('_');
			name.truncate(idx);
			sample_point_name = name;
			
			QTimer::singleShot(10,this,SLOT(DoButtonCommand()));
	
			break;
		}
	}

	delete l; // delete the list, not the objects
}


void HMI_manager::pSwitchToggledSendCommand()
{
	QObjectList *l = dialog_parent->queryList("PSwitch");

	QObjectListIt it( *l ); // iterate over the switches

	QObject *obj;

	while((obj = it.current()) != 0) 
	{
		// for each found object...
		++it;

		if(((PSwitch*)obj)->hasFocus())
		{
			if(((PSwitch*)obj)->getPSwitchValue())
			{
				value_for_command.sprintf("1");
			}
			else
			{
				value_for_command.sprintf("0");
			}
		
			Sleep(100);

			QString name = obj->name();

			int idx = name.find('_');
			name.truncate(idx);
			sample_point_name = name;

			QTimer::singleShot(10,this,SLOT(Do_pSwitchCommand()));

			break;
		}
	}

	delete l; // delete the list, not the objects
}


/*
*Function:RightClicked(const QString &name) 
show the active object menu
*Inputs:select object
*Outputs:none
*Returns:none
*/
void HMI_manager::RightClicked(QString &class_name, QString &widget_name) // show the inspection window
{
	IT_IT("HMI_manager::RightClicked");

	//  
	// create the inspection window
	//

	bool ack = false;
	bool found = false;

	if(class_name == QString("SinglePointLed"))
	{
		QObjectList *l = dialog_parent->queryList("SinglePointLed");

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			if(widget_name == obj->name())
			{
				int idx = widget_name.find('_');
				widget_name.truncate(idx);
				sample_point_name = widget_name;
				
				found = true;

				break;
			}
		}

		delete l; // delete the list, not the objects

		if(found)
		{
			ack = ((SinglePointLed*)obj)->GetFlash(); // need an ack or wot ?

			InspectMenu((SinglePointLed*)obj, sample_point_name, ack);

			return;
		}
	}
	else if(class_name == QString("DoublePointLed"))
	{
		QObjectList *l = dialog_parent->queryList("DoublePointLed");

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			if(widget_name == obj->name())
			{
				int idx = widget_name.find('_');
				widget_name.truncate(idx);
				sample_point_name = widget_name;
				
				found = true;

				break;
			}
		}

		delete l; // delete the list, not the objects

		if(found)
		{
			ack = ((DoublePointLed*)obj)->GetFlash(); // need an ack or wot ?

			InspectMenu((DoublePointLed*)obj, sample_point_name, ack);

			return;
		}
	}
	else if(class_name == QString("PSwitch"))
	{
		QObjectList *l = dialog_parent->queryList("PSwitch");

		QObjectListIt it( *l ); // iterate over the switches

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			if(widget_name == obj->name())
			{
				int idx = widget_name.find('_');
				widget_name.truncate(idx);
				sample_point_name = widget_name;
				
				found = true;

				break;
			}
		}

		delete l; // delete the list, not the objects

		if(found)
		{
			InspectMenu((PSwitch*)obj, sample_point_name, ack);

			return;
		}
	}
	else if(class_name == QString("Breaker"))
	{
		QObjectList *l = dialog_parent->queryList("Breaker");

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			if(widget_name == obj->name())
			{
				int idx = widget_name.find('_');
				widget_name.truncate(idx);
				sample_point_name = widget_name;
				
				found = true;

				break;
			}
		}

		delete l; // delete the list, not the objects

		if(found)
		{
			InspectMenu((Breaker*)obj, sample_point_name, ack);

			return;
		}
	}
	else if(class_name == QString("PSinglePointLed"))
	{
		QObjectList *l = dialog_parent->queryList("PSinglePointLed");

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			if(widget_name == obj->name())
			{
				int idx = widget_name.find('_');
				widget_name.truncate(idx);
				sample_point_name = widget_name;
				
				found = true;

				break;
			}
		}

		delete l; // delete the list, not the objects

		if(found)
		{
			ack = ((PSinglePointLed*)obj)->GetFlash(); // need an ack or wot ?

			InspectMenu((PSinglePointLed*)obj, sample_point_name, ack);

			return;
		}
	}
	else if(class_name == QString("PDoublePointLed"))
	{
		QObjectList *l = dialog_parent->queryList("PDoublePointLed");

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			if(widget_name == obj->name())
			{
				int idx = widget_name.find('_');
				widget_name.truncate(idx);
				sample_point_name = widget_name;
				
				found = true;

				break;
			}
		}

		delete l; // delete the list, not the objects

		if(found)
		{
			ack = ((PDoublePointLed*)obj)->GetFlash(); // need an ack or wot ?

			InspectMenu((PDoublePointLed*)obj, sample_point_name, ack);

			return;
		}
	}
	else if(class_name == QString("QwtThermo"))
	{
		QObjectList *l = dialog_parent->queryList("QwtThermo");

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			if(widget_name == obj->name())
			{
				int idx = widget_name.find('_');
				widget_name.truncate(idx);
				sample_point_name = widget_name;
				
				found = true;

				break;
			}
		}

		delete l; // delete the list, not the objects

		if(found)
		{
			InspectMenu((QwtThermo*)obj, sample_point_name, ack);

			return;
		}
	}
	else if(class_name == QString("PTank"))
	{
		QObjectList *l = dialog_parent->queryList("PTank");

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			if(widget_name == obj->name())
			{
				int idx = widget_name.find('_');
				widget_name.truncate(idx);
				sample_point_name = widget_name;
				
				found = true;

				break;
			}
		}

		delete l; // delete the list, not the objects

		if(found)
		{
			InspectMenu((PTank*)obj, sample_point_name, ack);

			return;
		}
	}
	else if(class_name == QString("PThermometer"))
	{
		QObjectList *l = dialog_parent->queryList("PThermometer");

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			if(widget_name == obj->name())
			{
				int idx = widget_name.find('_');
				widget_name.truncate(idx);
				sample_point_name = widget_name;
				
				found = true;

				break;
			}
		}

		delete l; // delete the list, not the objects

		if(found)
		{
			InspectMenu((PThermometer*)obj, sample_point_name, ack);

			return;
		}
	}
	else if(class_name == QString("PLCDNumber"))
	{
		QObjectList *l = dialog_parent->queryList("PLCDNumber");

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			if(widget_name == obj->name())
			{
				int idx = widget_name.find('_');
				widget_name.truncate(idx);
				sample_point_name = widget_name;
				
				found = true;

				break;
			}
		}

		delete l; // delete the list, not the objects

		if(found)
		{
			InspectMenu((PLCDNumber*)obj, sample_point_name, ack);

			return;
		}
	}
}

void HMI_manager::QueryResponse (QObject *p, const QString &c, int id, QObject*caller) // handles database responses
{
	if(p != this) return;
	switch(id)
	{
		case tUnit:
		{
			QSTransaction &t = GetConfigureDb()->CurrentTransaction();

			if(GetConfigureDb()->GetNumberResults() > 0)
			{
				unsigned char parametri[sizeof(dispatcher_extra_params)];
				dispatcher_extra_params* params = (dispatcher_extra_params *) parametri;

				memset(parametri, 0, sizeof(dispatcher_extra_params));

				QString command_value = t.Data1;
				
				QString samplePointName = t.Data2;
							
				if(command_value.length() > 0)
				{
					params->value = atof((const char*)command_value);
				}
				else
				{
					break;
				}
				
				QString unit_name = GetConfigureDb()->GetString("UNIT");

				strcpy(params->string1, (const char *)unit_name); //driver instance
				strcpy(params->string2, (const char *)samplePointName);
				strcpy(params->string3, (const char *)command_value); //For writing the string

				struct cp56time2a actual_time;
				get_utc_host_time(&actual_time);
				params->time_stamp = actual_time;
				
				//Generate IEC command
				
				GetDispatcher()->DoExec(NotificationEvent::CMD_SEND_COMMAND_TO_UNIT, (char *)parametri, sizeof(dispatcher_extra_params));  //broadcast to all tcp clients
			}
		} 
		break;
		case tTagLimits:
		{
			//
			QSTransaction &t = GetConfigureDb()->CurrentTransaction();

			int n = GetConfigureDb()->GetNumberResults();

			GetConfigureDb()->GotoBegin();
			//
			for(int i = 0; i < n ; i++, GetConfigureDb()->FetchNext())
			{
				QString widget_type = t.Data1;
				QString sample_point_name = t.Data2;
				//
				
				{
					QObjectList *l = dialog_parent->queryList(widget_type);

					QObjectListIt it( *l ); // iterate

					QObject *obj;

					while((obj = it.current()) != 0) 
					{
						// for each found object...
						++it;

						QString name = obj->name();

						int idx = name.find('_');
						name.truncate(idx);

						if(sample_point_name == name)
						{
							if(widget_type == QString("QwtThermo"))
							{
								double val = 0.0;

								((QwtThermo*)obj)->setValue(val);

								if(GetConfigureDb()->GetInt("LAENABLE"))
								{
									QString min = GetConfigureDb()->GetString("LOWERALARM");
									((QwtThermo*)obj)->setMinValue(atof((const char*)min));
								}
								else
								{
									((QwtThermo*)obj)->setMinValue(-500.0);
								}

								if(GetConfigureDb()->GetInt("UAENABLE"))
								{
									QString max = GetConfigureDb()->GetString("UPPERALARM");
									((QwtThermo*)obj)->setMaxValue(atof((const char*)max));
								}
								else
								{
									((QwtThermo*)obj)->setMaxValue(1000.0);
								}
							}
							else if(widget_type == QString("PTank"))
							{
								double val = 0.0;

								((PTank*)obj)->setValue(val);

								if(GetConfigureDb()->GetInt("LAENABLE"))
								{
									QString min = GetConfigureDb()->GetString("LOWERALARM");
									((PTank*)obj)->setMinValue(atof((const char*)min));
								}
								else
								{
									((PTank*)obj)->setMinValue(-500.0);
								}

								if(GetConfigureDb()->GetInt("UAENABLE"))
								{
									QString max = GetConfigureDb()->GetString("UPPERALARM");
									((PTank*)obj)->setMaxValue(atof((const char*)max));
								}
								else
								{
									((PTank*)obj)->setMaxValue(1000.0);
								}
							}
							else if(widget_type == QString("PThermometer"))
							{
								double val = 0.0;

								((PThermometer*)obj)->setValue(val);

								if(GetConfigureDb()->GetInt("LAENABLE"))
								{
									QString min = GetConfigureDb()->GetString("LOWERALARM");
									((PThermometer*)obj)->setMinValue(atof((const char*)min));
								}
								else
								{
									((PThermometer*)obj)->setMinValue(-500.0);
								}

								if(GetConfigureDb()->GetInt("UAENABLE"))
								{
									QString max = GetConfigureDb()->GetString("UPPERALARM");
									((PThermometer*)obj)->setMaxValue(atof((const char*)max));
								}
								else
								{
									((PThermometer*)obj)->setMaxValue(1000.0);
								}
							}
						}
					}

					delete l; // delete the list, not the objects
				}
			}
		}
		break;
		default:
		break;
	};
};

void HMI_manager::DoButtonCommand()
{
	IT_IT("HMI_manager::DoButtonCommand");

	if(QMessageBox::information(NULL,tr("Command confirmation"),tr("Send the command - Are You Sure?"),QObject::tr("Yes"),QObject::tr("No"),0,1) == 0)
	{
		bool found = false;
		QString edit_child_name;
		
		QObjectList *l = dialog_parent->queryList( "QLineEdit" );

		QObjectListIt it( *l ); // iterate

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			edit_child_name = obj->name();

			QString truncated_edit_child_name = edit_child_name;

			int idx = truncated_edit_child_name.find('_');
			truncated_edit_child_name.truncate(idx);

			if(sample_point_name == truncated_edit_child_name)
			{
				found = true;
				break;
			}
		}

		delete l; // delete the list, not the objects
		
		if(found)
		{
			QLineEdit* edit = (QLineEdit *) dialog_parent->child(edit_child_name, "QLineEdit");

			if(edit)
			{
				QString v = edit->text();

				value_for_command.sprintf("%s", (const char*)v);
			}
			else
			{
				return;
			}

			QString cmd = "select UNIT from SAMPLE where NAME='"+ sample_point_name +"';";

			GetConfigureDb()->DoExec(this, cmd, tUnit, value_for_command, sample_point_name); // kick it off
		}
	}
};

void HMI_manager::Do_pSwitchCommand()
{
	IT_IT("HMI_manager::Do_pSwitchCommand");

	if(QMessageBox::information(NULL,tr("Command confirmation"),tr("Send the command - Are You Sure?"),QObject::tr("Yes"),QObject::tr("No"),0,1) == 0)
	{
		QString cmd = "select UNIT from SAMPLE where NAME='"+ sample_point_name +"';";

		GetConfigureDb()->DoExec(this, cmd, tUnit, value_for_command, sample_point_name); // kick it off
	}
	else
	{
		QObjectList *l = dialog_parent->queryList("PSwitch");

		QObjectListIt it( *l ); // iterate over the switches

		QObject *obj;

		while((obj = it.current()) != 0) 
		{
			// for each found object...
			++it;

			if(((PSwitch*)obj)->hasFocus())
			{
				((PSwitch*)obj)->undoToggle();

				break;
			}
		}

		delete l; // delete the list, not the objects
	}
};


/*
*Function:CurrentNotify
*Inputs:notification code
*Outputs:none
*Returns:none
*/
void HMI_manager::ReceivedNotify(int ntf, const char * data)
{
	IT_IT("HMI_manager::ReceivedNotify");
	
	switch(ntf)
	{
		case NotificationEvent::CMD_LOGOUT:
		{
			QObjectList *l = dialog_parent->queryList( "QPushButton" );

			QObjectListIt it( *l ); // iterate over the buttons

			QObject *obj;

			while((obj = it.current()) != 0) 
			{
				// for each found object...
				++it;

				QString name = obj->name();

				((QPushButton*)obj)->setEnabled( FALSE );
			}

			delete l; // delete the list, not the objects
		}
		break;
		case NotificationEvent::CMD_LOGON:
		{
			QObjectList *l = dialog_parent->queryList( "QPushButton" );

			QObjectListIt it( *l ); // iterate over the buttons

			QObject *obj;

			while((obj = it.current()) != 0) 
			{
				// for each found object...
				++it;

				QString name = obj->name();

				if(GetUserDetails().privs & PRIVS_ACK_ALARMS)
				{
					((QPushButton*)obj)->setEnabled(TRUE);
				}
				else
				{
					((QPushButton*)obj)->setEnabled( FALSE );
				}
			}

			delete l; // delete the list, not the objects
		}
		break;
		default:
		break;
	}
};


#include <time.h>
#include <sys/timeb.h>

void HMI_manager::get_utc_host_time(struct cp56time2a* time)
{
	struct timeb tb;
	struct tm	*ptm;

    ftime (&tb);
	ptm = gmtime(&tb.time);
		
	time->hour = ptm->tm_hour;					//<0..23>
	time->min = ptm->tm_min;					//<0..59>
	time->msec = ptm->tm_sec*1000 + tb.millitm; //<0..59999>
	time->mday = ptm->tm_mday; //<1..31>
	time->wday = (ptm->tm_wday == 0) ? ptm->tm_wday + 7 : ptm->tm_wday; //<1..7>
	time->month = ptm->tm_mon + 1; //<1..12>
	time->year = ptm->tm_year - 100; //<0..99>
	time->iv = 0; //<0..1> Invalid: <0> is valid, <1> is invalid
	time->su = (u_char)tb.dstflag; //<0..1> SUmmer time: <0> is standard time, <1> is summer time

    return;
}

